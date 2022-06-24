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

    double currentangle()
    {
        mpu.update();
        Serial.print("Current Angle: ");
        Serial.println(mpu.getAngleZ());
        return mpu.getAngleZ();
    }
};
