

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
#define AIN1 21 //18
#define BIN1 17
#define AIN2 14 //5
#define BIN2 16
#define PWMA 22 //19
#define PWMB 4
#define STBY 9

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = -1; //motor right
const int offsetB = 1; //motor left

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motorRight = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motorLeft = Motor(BIN1, BIN2, PWMB, offsetB, STBY);


//for storing speed values

struct speed {
  int left, right;
};


//definition of the motor class

class Motors{

private:

typedef struct speed MotorSpeeds;

int convert_to_duration(int distance,int speed){
  double prop = 1; //need to measure this experimentally
  double x = prop * (distance/speed);
  return (int)x; //might not work
}


//TODO: implement straightness control

MotorSpeeds speed_straightness_control(int speed, int error){

MotorSpeeds speeds;

//corrigate path if needed

// if(error != 0 ){

// } else {

// }

speeds.left = speed; //change these
speeds.right = speed; //change these

return speeds;

}

void drive_straight(int speed, int duration,int error){

  MotorSpeeds speeds;

  speeds = speed_straightness_control(speed, error);

  
  // Use of the drive function which takes as arguements the speed
  // and optional duration.  A negative speed will cause it to go
  // backwards.  Speed can be from -255 to 255.  Also use of the
  // brake function which takes no arguements.

  motorRight.drive(speeds.right, duration);
  motorLeft.drive(speeds.left, duration);

}

void drive_steer(int angle, bool left){

motorLeft.brake();
motorRight.brake();
//conversion from angle to duration

int turning_prop = 12.2;

int duration = angle*turning_prop; // change this

int speed = 150;

int turn_direction = -1;
//which way to turn
if (left){
  turn_direction = 1;
}

  motorLeft.drive(turn_direction*speed, 1);

  motorRight.drive(-turn_direction*speed, duration);

  motorLeft.brake();
  motorRight.brake();

}



public:
  int speed;
  int distance;
  int turning_angle;
  int error;
  
//method to move forward
  void forward(int speed,int error){
  int duration = 1000; //default value

 //convert speed from range(-10,10) to (-255,255)
  speed = speed * (255/10);
  //generate duration to control the motors
  duration = convert_to_duration(distance, speed);
 
  drive_straight(speed,duration, error);
  }

  //method to brake

  void brake(){
    motorLeft.brake();
    motorRight.brake();
  }

//TODO: implement turning function
  void turn(int turning_angle, bool left){

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