#include <main_motor.h>
#include <main_sensor.h>

class Drive {
    public:
        int heading_angle;
        int coord_x, coord_y;
        Motors RoverMotors;
        int elapsed_rover_distance = 0 ;



        //call this in the void setup() in main.cpp
        void setup(){
            optical_setup();
        };
        //drive forward indefinitely, call it inside the void loop() in main.cpp, it is NON-BLOCKING
        void forward() {
            double straightness_error = 0; //TODO: set this error equal to the dx output from the rover
            
            
        };
        //drive forward a certain distance, call it inside void loop() in main.cpp, it is BLOCKING
        void forward_distance(int distance){
            double straightness_error = 0; //TODO: set this error equal to the dx output from the rover
            elapsed_rover_distance = 0; 
            while (elapsed_rover_distance < distance){
                RoverMotors.forward(3,straightness_error);
                elapsed_rover_distance = 0; //TODO: set this equal to the cumulative rover.dy since forward function begun
            };
                RoverMotors.brake();
        }

};