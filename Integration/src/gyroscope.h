#include "Wire.h"
// This is the library for Gyroscope
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

class GyroScope
{
public:
    void setup()
    {
        Serial.begin(115200);
        Wire.begin();

        byte status = mpu.begin();
        Serial.print(F("MPU6050 status: "));
        Serial.println(status);
        while (status != 0)
        {
        } // stop everything if could not connect to MPU6050

        Serial.println(F("Calculating offsets, do not move MPU6050"));
        delay(1000);
        mpu.calcOffsets(true, true); // gyro and accelero
        Serial.println("Done!\n");
    }

    double currentangle(){
        mpu.update();
        float modified_angle = mpu.getAngleZ()/2;
        int i = abs(mpu.getAngleZ()/2/360);
        if(modified_angle > 360){
            modified_angle = modified_angle - 360*i;
            Serial.print("Current Angle: ");
            Serial.println(modified_angle);
        }
        if (modified_angle < 0){
            modified_angle = modified_angle + 360 *(i+1);
            Serial.print("Current Angle: ");
            Serial.println(modified_angle);
        }
        
        return modified_angle;
    }

    double anglediff(){
        mpu.update();
        double initial = mpu.getAngleZ();
        double final = mpu.getAngleZ();
        return final - initial;
        Serial.print("Angle difference: ");
        Serial.println(final-initial);
    }


};