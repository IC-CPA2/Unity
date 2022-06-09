#include <Arduino.h>
#include <main_motor.h>
#include <main_sensor.h>



//total translation of the rover
double total_translation_x,total_translation_y;

Motors motors;

void setup(){

optical_setup();


}


void loop(){

total_translation_x,total_translation_y = optical_measurements();


    //speed is to be between -10 and 10
    int rover_speed = 5;

    //translation in centimetres
    int rover_translation = 10000;

    //relative turning angle compared to current position
    int rover_rotation_angle = 0;

    //the straightness error 
    int error = 0; //calculate this based on total_translations and knowledge on the current direction

    //motors_control(rover_speed,rover_translation,rover_rotation_angle,error);
    if(coord_y > 10000){
        motors.brake();
    }else{
        Serial.println("total translation x:");
        Serial.println(coord_x);
       motors.forward(3,coord_x);
        //motors.turn(90);
        //motors.turn(90,false);
    }
}
