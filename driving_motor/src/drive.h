#include <main_motor.h>
#include <main_sensor.h>

class Drive
{

private:
    Motors RoverMotors;
    int elapsed_rover_distance = 0;
    double straightness_error = 0;

public:
    double heading_angle;
    // x- and y positions of the rover
    double coord_x, coord_y;

    // call this in the void setup() in main.cpp
    void setup()
    {
        optical_setup();
    };

    //(-10 < speed < 10) drive forward indefinitely, call it inside the void loop() in main.cpp, it is NON-BLOCKING
    void forward(int speed)
    {

        optical_distance_moved();
        straightness_error = roverUnity.dx;
        RoverMotors.forward(speed, straightness_error);
        coord_x = roverUnity.pos_x;
        coord_y = roverUnity.pos_y;
    };
    // call this to brake the rover immediately
    void brake()
    {
        optical_distance_moved();
        RoverMotors.brake();
    };
    // call this to turn the rover a certain amount of degrees
    void turn(int angle_degrees, bool turnLeft)
    {
        double turned_angle = 0;

        // ENABLE THIS PART OF THE CODE FOR ROTATION FEEDBACK FROM THE OFS

        while (abs(turned_angle) < abs(angle_degrees))
        {
            turned_angle = turned_angle + optical_angle_turned();
            Serial.println(turned_angle);

            // TODO: implement this optical_angle_turned() function based on dy and dx changes in given optical flow sensing period
            RoverMotors.turn(turnLeft); // TODO: implement this .turn(turnLeft) method into Motors class, it just simply starts spinning the wheels into opposite directions!
        }
        roverUnity.head_angle = roverUnity.head_angle + turned_angle;

        // RoverMotors.turn_angle(angle_degrees, turnLeft);

        // roverUnity.head_angle = roverUnity.head_angle + angle_degrees;

        heading_angle = roverUnity.head_angle;

        RoverMotors.brake();
    };
    //(-10 < speed < 10) drive forward a certain distance, call it inside void loop() in main.cpp, it is BLOCKING
    void forward_distance(int speed, int distance)
    {

        int elapsed_rover_distance = 0;
        while (elapsed_rover_distance < distance)
        {
            optical_distance_moved();
            straightness_error = roverUnity.dx;
            RoverMotors.forward(speed, straightness_error);
            elapsed_rover_distance = elapsed_rover_distance + roverUnity.dy;
            coord_x = roverUnity.pos_x;
            coord_y = roverUnity.pos_y;
        };
        RoverMotors.brake();
    }
};
