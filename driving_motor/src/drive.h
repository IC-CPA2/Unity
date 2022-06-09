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
        void forward(speed) {
            double straightness_error = 0; //TODO: set this error equal to the dx output from the rover
            
            RoverMotors.forward(speed, straightness_error);
            
            
        };
        //call this to brake the rover immediately
        void brake(){
            RoverMotors.brake();   
            
        };
        //call this to turn the rover a certain amount of degrees
        void turn(int angle_degrees, bool turnLeft) {
            int turned_angle = 0;
            
            while(turned_angle < angle_degrees){
                turned_angle = turned_angle + optical_angle_turned(); //TODO: implement this optical_angle_turned() function based on dy and dx changes in given optical flow sensing period
            RoverMotors.turn(turnLeft); //TODO: implement this .turn(turnLeft) method into Motors class, it just simply starts spinning the wheels into opposite directions!
            }
            
            RoverMotors.brake();
            
        };
        //drive forward a certain distance, call it inside void loop() in main.cpp, it is BLOCKING
        void forward_distance(int speed, int distance){
            double straightness_error = 0; //TODO: set this error equal to the dx output from the rover
            elapsed_rover_distance = 0; 
            while (elapsed_rover_distance < distance){
                RoverMotors.forward(speed,straightness_error);
                elapsed_rover_distance = 0; //TODO: set this equal to the cumulative rover.dy since forward function begun
            };
                RoverMotors.brake();
        }

};
