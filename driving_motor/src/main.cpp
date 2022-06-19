#include <Arduino.h>

#include <drive.h>

// total translation of the rover
//  double total_translation_x,total_translation_y;

// Motors motors;

Drive driveUnity;

void setup()
{

    driveUnity.setup();
    // delay to initialise everyything properly
    delay(2000);
}

void loop()
{
    delay(500);
    //  optical_measurements();
    //  Serial.println("Forward distance:");
    //  Serial.println(driveUnity.coord_y);

    // optical_measurements();

    // 666 coordinate_y difference translates to around 15cm of translation
    driveUnity.forward_distance(4, 20);
    driveUnity.turn(90, true);
    driveUnity.forward_distance(4, 20);
    driveUnity.turn(90, false);

    if (driveUnity.coord_y < 66600)
    {

        //   driveUnity.forward_distance(2, 1000);
    }
    else
    {
        driveUnity.turn(90, true);
        driveUnity.turn(90, false);
        driveUnity.brake();
        Serial.println("Heading angle");
        Serial.println(roverUnity.head_angle);
    }
}

// void loop(){

// total_translation_x,total_translation_y = optical_measurements();

//     //speed is to be between -10 and 10
//     int rover_speed = 5;

//     //translation in centimetres
//     int rover_translation = 10000;

//     //relative turning angle compared to current position
//     int rover_rotation_angle = 0;

//     //the straightness error
//     int error = 0; //calculate this based on total_translations and knowledge on the current direction

//     //motors_control(rover_speed,rover_translation,rover_rotation_angle,error);
//     if(coord_y > 10000){
//         motors.brake();
//     }else{
//         Serial.println("total translation x:");
//         Serial.println(coord_x);
//        motors.forward(3,coord_x);
//         //motors.turn(90);
//         //motors.turn(90,false);
//     }
// }
