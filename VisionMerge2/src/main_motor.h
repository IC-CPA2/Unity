/******************************************************************************
TestRun.ino
TB6612FNG H-Bridge Motor Driver Example code
Michelle @ SparkFun Electronics
8/20/16
https://github.com/sparkfun/SparkFun_TB6612FNG_Arduino_Library
Uses 2 motors to show examples of the functions in the library.  This causes
a robot to do a little 'jig'.  Each movement has an equal and opposite movement
so assuming your motors are balanced the bot should end up at the same place it
started.
Resources:
TB6612 SparkFun Library
Development environment specifics:
Developed on Arduino 1.6.4
Developed with ROB-9457
******************************************************************************/

// This is the library for the TB6612 that contains the class Motor and all the
// functions
#include <SparkFun_TB6612.h>

#include <stdlib.h>

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// the default pins listed are the ones used on the Redbot (ROB-12097) with
// the exception of STBY which the Redbot controls with a physical switch
#define AIN1 21 // 18
#define BIN1 17
#define AIN2 25 // 1 // 5
#define BIN2 16
#define PWMA 22 // 19
#define PWMB 4
#define STBY 9

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = -1; // motor right
const int offsetB = 1;  // motor left

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motorLeft = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motorRight = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

// for storing speed values

struct speed
{
  int left, right;
};

// definition of the motor class
class Motors
{

private:
  double last_heading_error = 0;
  double cumulative_error = 0;
  double Kd, Kp, Ki, KD, KI, correction, motor_proportionality, normalised_speed, previous_current_error, clamping;
  double dEs[4] = {0, 0, 0, 0};
  double dEs_average = 0;
  double previous_dEs_average = 0;

  typedef struct speed MotorSpeeds;

  int convert_to_duration(int distance, int speed)
  {
    double prop = 1; // need to measure this experimentally
    double x = prop * (distance / speed);
    return (int)x; // might not work
  }

  // the input error here has to be the total_x_translation during straight drive
  MotorSpeeds speed_straightness_control(int speed, double current_error, double motor_prop, double kp, double ki, double kd)
  {

    MotorSpeeds speeds;

    // adjust constant to model power differences between motor left and right

    motor_proportionality = motor_prop;

    // let's initialise the PD constants

    Kd = kd; // 4;

    Kp = kp; // 15;

    Ki = ki; // 4;

    clamping = 20;

    cumulative_error = cumulative_error + current_error; // previously 0.5 *

    // apply low-pass filter to the derivative values

    // for (int i = sizeof(dEs); i > 0; i--)
    // {

    //   dEs[i] = dEs[i - 1];
    // }

    dEs[0] = current_error;

    dEs_average = 0;

    // for (int i = 0; i < sizeof(dEs); i++)
    // {

    //   dEs_average += dEs[i];
    // }

    // dEs_average = dEs_average / sizeof(dEs);

    // let's saturate it, since the actuator(motors) can saturate as well

    if (cumulative_error > clamping)
    {
      cumulative_error = clamping;
    }
    else if (cumulative_error < -clamping)
    {
      cumulative_error = -clamping;
    }

    // corrigate path if needed

    // double corrig_const = 1 / 2; // adjust this to based on trial and error

    // double corrigation = 1 + corrig_const * log(1 + abs(error));

    // PD controller

    // KD = (dEs_average - previous_dEs_average) * speed / 255;

    KD = (current_error - previous_current_error) * speed / 255;

    KI = cumulative_error;

      correction = Kd * KD + Kp * current_error + Ki * KI;
    //correction = Kp * current_error;

    // NOT TO USE: stop the rover to restabilise in case of too high sverving

    // if (correction > 30)
    // {
    //   motorRight.brake();
    //   motorLeft.brake();
    //   delay(0.5);
    // }

    // Serial.println("\n \n");
    // Serial.println("----------");
    // Serial.println("----------");
    // // Serial.println("Correction values are KP, KI, KD:");
    // Serial.print("KP: ");
    // Serial.print(Kp * current_error);
    // Serial.print("KI: ");
    // Serial.print(KI * Ki);
    // Serial.print("KD: ");
    // Serial.print(KD * Kd);
    // Serial.print(" ---- Correction: ");
    // Serial.print(correction);

    // Serial.println("----------");
    // Serial.println("----------");

    // speeds.left = corrigation * speed;
    speeds.left = speed + correction;
    speeds.right = motor_proportionality * speed - correction;

    last_heading_error = cumulative_error;
    previous_current_error = current_error;

    previous_dEs_average = dEs_average;

    return speeds;
  }

  void drive_straight(int speed, int duration, double current_error, double motor_prop, double kp, double ki, double kd)
  {

    MotorSpeeds speeds;
    // the input error here has to be the total_x_translation during straight drive
    // Serial.println("speed is controlled, the error is:");
    // Serial.println(current_error);
    speeds = speed_straightness_control(speed, current_error, motor_prop, kp, ki, kd);

    // Use of the drive function which takes as arguements the speed
    // and optional duration.  A negative speed will cause it to go
    // backwards.  Speed can be from -255 to 255.  Also use of the
    // brake function which takes no arguements.

    motorRight.drive(speeds.right, duration);
    motorLeft.drive(speeds.left, duration);
  }

  void drive_steer(int angle, bool left)
  {

    motorLeft.brake();
    motorRight.brake();

    // reset cumulative error

    cumulative_error = 0;

    // conversion from angle to duration

    int turning_prop = 12.6;

    int duration = angle * turning_prop; // change this

    int speed = 150;

    int turn_direction = -1;
    // which way to turn
    if (left)
    {
      turn_direction = 1;
    }

    motorLeft.drive(turn_direction * speed, 1);

    motorRight.drive(-turn_direction * speed, duration);

    motorLeft.brake();
    motorRight.brake();
  }

public:
  int speed;
  int distance = 1; // IT MIGHT NOT WORK
  double turning_angle;
  double error;

  // method to move forward
  void forward(int speed, double current_error, double motor_prop, double kp, double ki, double kd)
  {
    int duration = 10; // default value

    // convert speed from range(-10,10) to (-255,255)
    speed = speed * (255 / 10);
    // generate duration to control the motors
    duration = convert_to_duration(distance, speed);

    drive_straight(speed, duration, current_error, motor_prop, kp, ki, kd);
  }

  // method to brake

  void brake()
  {
    motorLeft.brake();
    motorRight.brake();
  }

  // NOTE: this function is open-loop, do not use in production!
  void turn(bool turnLeft)
  {
    int speed = 70;

    if (turnLeft)
    {
      motorLeft.drive(speed, 1);

      motorRight.drive(-speed, 1);
    }
    else
    {
      motorLeft.drive(-speed, 1);

      motorRight.drive(speed, 1);
    }
  };

  void turn_angle(int turning_angle, bool left)
  {

    drive_steer(turning_angle, left);
  }
};

// void motors_control(int speed,int distance,int turning_angle, int error)
// {
//   //convert speed from range(-10,10) to (-255,255)
//   speed = speed * (255/10);
//   //generate duration to control the motors
//   int duration = convert_to_duration(distance, speed);
//   //drive_steer(speed,turning_angle);
//   drive_straight(speed, duration,error);

// }