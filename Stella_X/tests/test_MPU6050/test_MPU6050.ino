#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <Wire.h>

MPU6050 mpu;
bool dmpReady = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
float euler[3];
float ypr[3];

void setup()
{
    Wire.begin();

    Serial.begin(115200);

    mpu.initialize();

    if (mpu.testConnection() == true) {
        Serial.println("connection succesful");
    }
    else {
        Serial.println("connection failed");
    }

    devStatus = mpu.dmpInitialize();

    if (devStatus == 0) {
        mpu.setXGyroOffset(15);
        mpu.setYGyroOffset(-53);
        mpu.setZGyroOffset(20);
        mpu.setXAccelOffset(-2865);
        mpu.setYAccelOffset(-1322);
        mpu.setZAccelOffset(2091);

        mpu.setDMPEnabled(true);
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else {
        Serial.println("error ");
        Serial.println(devStatus);
    }
}

void loop()
{
    if (dmpReady == false) {
        return;
    }

    uint32_t time_start = micros();
    mpu.resetFIFO();
    while (mpu.getFIFOCount() < packetSize) ;

    mpu.getFIFOBytes(fifoBuffer, packetSize);
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    uint32_t time_end = micros();

    Serial.print("ypr\t");
    Serial.print(ypr[0] * 180/M_PI);
    Serial.print("\t");
    Serial.print(ypr[1] * 180/M_PI);
    Serial.print("\t");
    Serial.print(ypr[2] * 180/M_PI);
    Serial.print("\ttime\t");
    Serial.println(time_end - time_start);
}
