/******************************************************************************
TB6612.cpp
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

#include "Sparkfun_TB6612.h"

Motor::Motor(PinName pwm, PinName dir1, PinName dir2, PinName standby, int offset) : _pwm(pwm), _dir1(dir1), _dir2(dir2), _standby(standby) {

    _offset = offset;
}
  
void Motor::drive(int speed)
{
    enable_motor();
    speed = speed * _offset;
    if (speed>=0) fwd(speed);
    else rev(-speed);
}

void Motor::drive(int speed, int duration)
{
  drive(speed);
  wait_ms(duration);
}

void Motor::fwd(int speed)
{
   _dir1 = 1;
   _dir2 = 0;
   _pwm = speed; // NOTE, speed is -255,255 Arduino analog.
}

void Motor::rev(int speed)
{
   _dir1 = 0;
   _dir2 = 1;
   _pwm = speed; // NOTE, speed is -255,255 Arduino analog.
}

void Motor::brake()
{
   _dir1 = 1;
   _dir2 = 1;
   _pwm = 0.0f; // NOTE, speed is -255,255 Arduino analog.
}

void Motor::standby()
{
    _standby = 0;
}

void Motor::enable_motor() {
    _standby = 1;
}

void forward(Motor motor1, Motor motor2, int speed)
{
	motor1.drive(speed);
	motor2.drive(speed);
}

void forward(Motor motor1, Motor motor2)
{
	motor1.drive(DEFAULTSPEED);
	motor2.drive(DEFAULTSPEED);
}

void back(Motor motor1, Motor motor2, int speed)
{
	int temp = abs(speed);
	motor1.drive(-temp);
	motor2.drive(-temp);
}
void back(Motor motor1, Motor motor2)
{
	motor1.drive(-DEFAULTSPEED);
	motor2.drive(-DEFAULTSPEED);
}
void left(Motor left, Motor right, int speed)
{
	int temp = abs(speed)/2;
	left.drive(-temp);
	right.drive(temp);
	
}


void right(Motor left, Motor right, int speed)
{
	int temp = abs(speed)/2;
	left.drive(temp);
	right.drive(-temp);
	
}
void brake(Motor motor1, Motor motor2)
{
	motor1.brake();
	motor2.brake();
}