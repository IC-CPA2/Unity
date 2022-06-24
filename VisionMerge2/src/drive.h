#include <main_motor.h>
#include <main_sensor.h>

class Drive
{

private:
    Motors RoverMotors;
    int elapsed_rover_distance = 0;
    double straightness_error = 0;
    double translation_prop = 0.019; // 0.0225;

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
    void forward(int speed, double motor_prop, double kp, double ki, double kd)
    {

        optical_distance_moved();
        straightness_error = roverUnity.required_head_angle - roverUnity.head_angle;
        RoverMotors.forward(speed, straightness_error, motor_prop, kp, ki, kd);
        coord_x = translation_prop * roverUnity.pos_x;
        coord_y = translation_prop * roverUnity.pos_y;
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
        if (turnLeft)
        {
            roverUnity.required_head_angle = roverUnity.required_head_angle + angle_degrees;
        }
        else
        {
            roverUnity.required_head_angle = roverUnity.required_head_angle - angle_degrees;
        }

        // ensure that the required head angle is within the range of 0 <= angle < 360

        if (roverUnity.required_head_angle >= 360)
        {
            roverUnity.required_head_angle = roverUnity.required_head_angle - 360;
        }
        else if (roverUnity.required_head_angle < 0)
        {
            roverUnity.required_head_angle = 360 - roverUnity.required_head_angle;
        }

        while (abs(turned_angle) < abs(angle_degrees))
        {
            turned_angle = turned_angle + optical_angle_turned();
            Serial.println(turned_angle);

            // TODO: implement this optical_angle_turned() function based on dy and dx changes in given optical flow sensing period
            RoverMotors.turn(turnLeft); // TODO: implement this .turn(turnLeft) method into Motors class, it just simply starts spinning the wheels into opposite directions!
        }

        // roverUnity.head_angle = roverUnity.head_angle + turned_angle;

        roverUnity.head_angle = roverUnity.required_head_angle;

        // RoverMotors.turn_angle(angle_degrees, turnLeft);

        // roverUnity.head_angle = roverUnity.head_angle + angle_degrees;

        heading_angle = roverUnity.head_angle;

        RoverMotors.brake();
    };
    //(-10 < speed < 10) drive forward a certain distance, call it inside void loop() in main.cpp, it is BLOCKING
    void forward_distance(int speed, double distance, double motor_prop, double kp, double ki, double kd)
    {

        double elapsed_rover_distance = 0.0;

        while (elapsed_rover_distance * translation_prop < distance)
        {
            optical_distance_moved();
            straightness_error = roverUnity.required_head_angle - roverUnity.head_angle;
            RoverMotors.forward(speed, straightness_error, motor_prop, kp, ki, kd);
            elapsed_rover_distance = elapsed_rover_distance + roverUnity.dy;
            coord_x = translation_prop * roverUnity.pos_x;
            coord_y = translation_prop * roverUnity.pos_y;
        };
        RoverMotors.brake();
    }
    // navigate to the next (neigbouring!) tile in the array of neighbouring tile coords by the navigation algorithm NOTE: currently only supports 90degree rotations
    void navigate_to_neighbouring_tile(int speed, int next_tile[2], double motor_prop, double kp, double ki, double kd)
    {

        // state machine here to determine whether or not to turn first and then move

        // current head_angle vs which direction the tile is into

        double required_head_angle = 0;

        double turn_left = true;

        // for 90 degree rotations, one of them is always 0

        // int translation_dir_x = (next_tile[0] - roverUnity.tile_x) / abs(next_tile[0] - roverUnity.tile_x);
        // int translation_dir_y = (next_tile[1] - roverUnity.tile_y) / abs(next_tile[1] - roverUnity.tile_y);
        Serial.println("Actual tile for x");
        Serial.println(roverUnity.tile_x);
        Serial.println("Actual tile for y");
        Serial.println(roverUnity.tile_y);
        Serial.println("Required tile for x");
        Serial.println(next_tile[0]);
        Serial.println("Required tile for y");
        Serial.println(next_tile[1]);
        int translation_dir_x = (next_tile[0] - roverUnity.tile_x);
        int translation_dir_y = (next_tile[1] - roverUnity.tile_y);
        Serial.println("Translation for x");
        Serial.println(translation_dir_x);
        Serial.println("Translation for y");
        Serial.println(translation_dir_y);

        Serial.println("Checkpoint 1");
        if (translation_dir_x != 0)
        {
            if (translation_dir_x > 0)
            {
                required_head_angle = 0;
            }
            else
            {
                required_head_angle = 180;
            }
        }
        if (translation_dir_y != 0)
        {
            if (translation_dir_y > 0)
            {
                required_head_angle = 90;
            }
            else
            {
                required_head_angle = 270;
            }
        }
        Serial.println("Checkpoint 2");
        Serial.println("Required head angle?");
        Serial.println(required_head_angle);

        // calculate how much to turn

        double turning_angle = required_head_angle - roverUnity.head_angle;
        Serial.println("Turning angle?");
        Serial.println(turning_angle);
        // calculate where to turn

        if (turning_angle >= 0)
        {
            turn_left = true;
        }
        else
        {
            turn_left = false;
        }
        Serial.println("Turn left?");
        Serial.println(turn_left);
        // turn into the required direction

        if (turn_left)
        {
            roverUnity.required_head_angle = roverUnity.required_head_angle + turning_angle;
        }
        else
        {
            roverUnity.required_head_angle = roverUnity.required_head_angle - turning_angle;
        }
        Serial.println("Checkpoint 3");
        // ensure that the required head angle is within the range of 0 <= angle < 360

        if (roverUnity.required_head_angle >= 360)
        {
            roverUnity.required_head_angle = roverUnity.required_head_angle - 360;
        }
        else if (roverUnity.required_head_angle < 0)
        {
            roverUnity.required_head_angle = 360 - roverUnity.required_head_angle;
        }
        Serial.println("Required head angle after normalising?");
        Serial.println(roverUnity.required_head_angle);
        // initialise turned angle
        double turned_angle = 0;
        Serial.println("Checkpoint 4");
        while (abs(turned_angle) < abs(turning_angle))
        {
            turned_angle = turned_angle + optical_angle_turned();
            Serial.println(turned_angle);

            // TODO: implement this optical_angle_turned() function based on dy and dx changes in given optical flow sensing period
            RoverMotors.turn(turn_left); // TODO: implement this .turn(turnLeft) method into Motors class, it just simply starts spinning the wheels into opposite directions!
        }
        Serial.println("Checkpoint 5");
        RoverMotors.brake();

        // roverUnity.head_angle = roverUnity.head_angle + turned_angle;

        roverUnity.head_angle = roverUnity.required_head_angle;

        // RoverMotors.turn_angle(angle_degrees, turnLeft);

        // roverUnity.head_angle = roverUnity.head_angle + angle_degrees;

        heading_angle = roverUnity.head_angle;

        // then move
        Serial.println("Checkpoint 6");
        // THE ERROR IS HERE!!!
        while (roverUnity.tile_x != next_tile[0] && roverUnity.tile_y != next_tile[1])
        {
            Serial.println("------------");
            optical_distance_moved();
            straightness_error = roverUnity.required_head_angle - roverUnity.head_angle;
            RoverMotors.forward(speed, straightness_error, motor_prop, kp, ki, kd);
            delay(250);
            RoverMotors.brake();
            Serial.println(roverUnity.dx);
            Serial.println(roverUnity.dy);
            Serial.println(roverUnity.pos_x);
            Serial.println(roverUnity.pos_y);
            Serial.println(roverUnity.tile_x);

            Serial.println(roverUnity.tile_y);

            Serial.println("Required head angle:");
            Serial.println(roverUnity.required_head_angle);
            Serial.println("Actual head angle:");
            Serial.println(roverUnity.head_angle);
            Serial.println("------------");
            delay(750);
        }
        Serial.println("Checkpoint 7");
    }
};