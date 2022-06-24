//#include <Arduino.h> maybe we need it???
#include <SPI.h>

#include <cmath>

#include <math.h>

// these pins may be different on different boards

#define PIN_SS 5    // originally 5
#define PIN_MISO 19 // originally 19
#define PIN_MOSI 23
#define PIN_SCK 18 // originally 18

#define PIN_MOUSECAM_RESET 35
#define PIN_MOUSECAM_CS 5 // originally 5

#define ADNS3080_PIXELS_X 30
#define ADNS3080_PIXELS_Y 30

#define ADNS3080_PRODUCT_ID 0x00
#define ADNS3080_REVISION_ID 0x01
#define ADNS3080_MOTION 0x02
#define ADNS3080_DELTA_X 0x03
#define ADNS3080_DELTA_Y 0x04
#define ADNS3080_SQUAL 0x05
#define ADNS3080_PIXEL_SUM 0x06
#define ADNS3080_MAXIMUM_PIXEL 0x07
#define ADNS3080_CONFIGURATION_BITS 0x0a
#define ADNS3080_EXTENDED_CONFIG 0x0b
#define ADNS3080_DATA_OUT_LOWER 0x0c
#define ADNS3080_DATA_OUT_UPPER 0x0d
#define ADNS3080_SHUTTER_LOWER 0x0e
#define ADNS3080_SHUTTER_UPPER 0x0f
#define ADNS3080_FRAME_PERIOD_LOWER 0x10
#define ADNS3080_FRAME_PERIOD_UPPER 0x11
#define ADNS3080_MOTION_CLEAR 0x12
#define ADNS3080_FRAME_CAPTURE 0x13
#define ADNS3080_SROM_ENABLE 0x14
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_LOWER 0x19
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_UPPER 0x1a
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_LOWER 0x1b
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_UPPER 0x1c
#define ADNS3080_SHUTTER_MAX_BOUND_LOWER 0x1e
#define ADNS3080_SHUTTER_MAX_BOUND_UPPER 0x1e
#define ADNS3080_SROM_ID 0x1f
#define ADNS3080_OBSERVATION 0x3d
#define ADNS3080_INVERSE_PRODUCT_ID 0x3f
#define ADNS3080_PIXEL_BURST 0x40
#define ADNS3080_MOTION_BURST 0x50
#define ADNS3080_SROM_LOAD 0x60

#define ADNS3080_PRODUCT_ID_VAL 0x17

int total_rover_x = 0;
int total_rover_y = 0;

int total_rover_x1 = 0;
int total_rover_y1 = 0;

int x = 0;
int y = 0;

int a = 0;
int b = 0;

int rover_distance_x = 0;
int rover_distance_y = 0;

int rover_angle = 0;

// rover turning adjustment proportionality constant

float turning_prop = 0.108;

double abs_coord_x = 0;
double abs_coord_y = 0;

volatile byte movementflag = 0;
volatile int xydat[2];

SPIClass *vspi = NULL;

int convTwosComp(int b)
{
  // Convert from 2's complement
  if (b & 0x80)
  {
    b = -1 * ((b ^ 0xff) + 1);
  }
  return b;
}

int tdistance = 0;

void mousecam_reset()
{
  digitalWrite(PIN_MOUSECAM_RESET, HIGH);
  delay(1); // reset pulse >10us
  digitalWrite(PIN_MOUSECAM_RESET, LOW);
  delay(35); // 35ms from reset to functional
}

int mousecam_init()
{
  pinMode(PIN_MOUSECAM_RESET, OUTPUT);
  pinMode(PIN_MOUSECAM_CS, OUTPUT);

  digitalWrite(PIN_MOUSECAM_CS, HIGH);

  mousecam_reset();

  return 1;
}

void mousecam_write_reg(int reg, int val)
{
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  vspi->transfer(reg | 0x80);
  vspi->transfer(val);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(50);
}

int mousecam_read_reg(int reg)
{
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  vspi->transfer(reg);
  delayMicroseconds(75);
  int ret = vspi->transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(1);
  return ret;
}

struct MD
{
  byte motion;
  char dx, dy;
  byte squal;
  word shutter;
  byte max_pix;
};

void mousecam_read_motion(struct MD *p)
{
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  vspi->transfer(ADNS3080_MOTION_BURST);
  delayMicroseconds(75);
  p->motion = vspi->transfer(0xff);
  p->dx = vspi->transfer(0xff);
  p->dy = vspi->transfer(0xff);
  p->squal = vspi->transfer(0xff);
  p->shutter = vspi->transfer(0xff) << 8;
  p->shutter = vspi->transfer(0xff);
  p->max_pix = vspi->transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(5);
}

// pdata must point to an array of size ADNS3080_PIXELS_X x ADNS3080_PIXELS_Y
// you must call mousecam_reset() after this if you want to go back to normal operation
int mousecam_frame_capture(byte *pdata)
{
  mousecam_write_reg(ADNS3080_FRAME_CAPTURE, 0x83);

  digitalWrite(PIN_MOUSECAM_CS, LOW);

  vspi->transfer(ADNS3080_PIXEL_BURST);
  delayMicroseconds(50);

  int pix;
  byte started = 0;
  int count;
  int timeout = 0;
  int ret = 0;
  for (count = 0; count < ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y;)
  {
    pix = vspi->transfer(0xff);
    delayMicroseconds(10);
    if (started == 0)
    {
      if (pix & 0x40)
        started = 1;
      else
      {
        timeout++;
        if (timeout == 100)
        {
          ret = -1;
          break;
        }
      }
    }
    if (started == 1)
    {
      pdata[count++] = (pix & 0x3f) << 2; // scale to normal grayscale byte range
    }
  }

  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(14);

  return ret;
};

void optical_setup()
{
  pinMode(PIN_SS, OUTPUT);
  pinMode(PIN_MISO, INPUT);
  pinMode(PIN_MOSI, OUTPUT);
  pinMode(PIN_SCK, OUTPUT);
  vspi = new SPIClass(VSPI);
  vspi->begin();
  vspi->setClockDivider(SPI_CLOCK_DIV32);
  vspi->setDataMode(SPI_MODE3);
  vspi->setBitOrder(MSBFIRST);

  Serial.begin(9600); // this

  if (mousecam_init() == -1)
  {
    Serial.println("Mouse cam failed to init");
    while (1)
      ;
  }
}

char asciiart(int k)
{
  static char foo[] = "WX86*3I>!;~:,`. ";
  return foo[k >> 4];
}

byte frame[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y];

// DEFINE STRUCT ROVER

struct Rover
{
  // dx and dy are in terms of centimetres
  double dx, dy;
  double head_angle = 0;
  double required_head_angle = 0;
  double translation_prop = 15 / 666 * 666 / 15;
  // position of rover in terms of centimetre translation
  double pos_x, pos_y;
};

// struct AbsCoords
// {
//   int pos_x, pos_y;
// };

// definition of our global ROVER struct

Rover roverUnity;

void calc_abs_coords()
{
  // assign roverUnity new values here

  double dx = roverUnity.dx;
  double dy = roverUnity.dy;

  Serial.println("Rover dx");
  Serial.println(dx);
  Serial.println("Rover dy");
  Serial.println(dy);

  double head_angle_radians = roverUnity.head_angle * 2 * M_PI / 180;

  roverUnity.pos_x = sin(head_angle_radians) * dx + roverUnity.pos_x;

  roverUnity.pos_y = cos(head_angle_radians) * dy + roverUnity.pos_y;

  Serial.println("Rover HEAD ANGLE");
  Serial.println(roverUnity.head_angle);

  Serial.print('\n');

  Serial.println("position x = " + String(roverUnity.pos_x));

  Serial.println("position y = " + String(roverUnity.pos_y));
  Serial.print('\n');

  // TODO: implement calculation of absolute coordinates here based on movement reported by the optical flow sensor
};

void optical_measurements()
{
#if 0
/*
    if(movementflag){

    tdistance = tdistance + convTwosComp(xydat[0]);
    Serial.println("Distance = " + String(tdistance));
    movementflag=0;
    delay(3);
    }

  */
  // if enabled this section grabs frames and outputs them as ascii art

  if(mousecam_frame_capture(frame)==0)
  {
    int i,j,k;
    for(i=0, k=0; i<ADNS3080_PIXELS_Y; i++)
    {
      for(j=0; j<ADNS3080_PIXELS_X; j++, k++)
      {
        Serial.print(asciiart(frame[k]));
        Serial.print(' ');
      }
      Serial.println();
    }
  }
  Serial.println();
  delay(250);

#else

  // if enabled this section produces a bar graph of the surface quality that can be used to focus the camera
  // also drawn is the average pixel value 0-63 and the shutter speed and the motion dx,dy.

  int val = mousecam_read_reg(ADNS3080_PIXEL_SUM);
  MD md;
  mousecam_read_motion(&md);
  for (int i = 0; i < md.squal / 4; i++)
    Serial.print('*');
  Serial.print(' ');
  Serial.print((val * 100) / 351);
  Serial.print(' ');
  Serial.print(md.shutter);
  Serial.print(" (");
  Serial.print((int)convTwosComp(md.dx));
  Serial.print(',');
  Serial.print((int)convTwosComp(md.dy));
  Serial.println(')');

  // Serial.println(md.max_pix);

  // roverUnity.dx = roverUnity.translation_prop * convTwosComp(md.dx);
  // roverUnity.dy = roverUnity.translation_prop * convTwosComp(md.dy);

  roverUnity.dx = convTwosComp(md.dx);
  roverUnity.dy = convTwosComp(md.dy);

  roverUnity.head_angle = roverUnity.head_angle - turning_prop * roverUnity.dx;

  // roverUnity.rover_distance_x = roverUnity.distance_X + roverUnity.dx; // maybe devide by 157 ???
  // roverUnity.distance_y = roverUnity.distance_y + roverUnity.dy; // maybe devide by 157 ???

  // total_rover_x = total_rover_x1 / 157;
  // total_rover_y = total_rover_y1 / 157;

  // delay(200);

#endif
};

double optical_measurements_for_angle()
{

  double angle_turned = 0;

  // if enabled this section produces a bar graph of the surface quality that can be used to focus the camera
  // also drawn is the average pixel value 0-63 and the shutter speed and the motion dx,dy.

  int val = mousecam_read_reg(ADNS3080_PIXEL_SUM);
  MD md;
  mousecam_read_motion(&md);
  // for (int i = 0; i < md.squal / 4; i++)
  //   Serial.print('*');
  // Serial.print(' ');
  // Serial.print((val * 100) / 351);
  // Serial.print(' ');
  // Serial.print(md.shutter);
  // Serial.print(" (");
  // Serial.print((int)convTwosComp(md.dx));
  // Serial.print(',');
  // Serial.print((int)convTwosComp(md.dy));
  // Serial.println(')');

  // Serial.println(md.max_pix);

  roverUnity.dx = convTwosComp(md.dx);
  roverUnity.dy = convTwosComp(md.dy);

  angle_turned = turning_prop * roverUnity.dx;

  // roverUnity.rover_distance_x = roverUnity.distance_X + roverUnity.dx; // maybe devide by 157 ???
  // roverUnity.distance_y = roverUnity.distance_y + roverUnity.dy; // maybe devide by 157 ???

  // total_rover_x = total_rover_x1 / 157;
  // total_rover_y = total_rover_y1 / 157;

  // Serial.print('\n');

  // Serial.println("position x = " + String(roverUnity.pos_x));

  // Serial.println("position y = " + String(roverUnity.pos_y));
  // Serial.print('\n');

  // delay(200);
  return angle_turned;
};

void optical_distance_moved()
{

  optical_measurements();

  calc_abs_coords();
};

double optical_angle_turned()
{

  double angle_turned = 0;

  angle_turned = optical_measurements_for_angle();

  // do calculations based on the values stored within the roverUnity dx and dy values upon the running of the optical_measurements() function

  // Serial.println(roverUnity.dx);

  // Serial.println(angle_turned);

  return angle_turned;
};