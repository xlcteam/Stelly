#if COMPASS_TYPE == LEGO_COMPASS

uint16_t north;

void setup_compass(void)
{
    Wire.begin();
    north = 0;
}

inline uint16_t compass_angle(void)
{
    return (compass_angle_raw() - north + 360) % 360;
}

uint16_t compass_angle_raw(void)
{
    uint16_t a, b;
    Wire.beginTransmission(0x01);
    Wire.write(0x44);
    Wire.endTransmission();
    Wire.requestFrom(0x01, 2);
    if (Wire.available() >= 2) {
        a = Wire.read();
        b = Wire.read();
        while (Wire.available()) Wire.read();
    }

    return a + b * 256;
}

inline uint16_t compass_north(void)
{
    return north;
}

inline void compass_set_north_val(uint16_t value)
{
    north = value;
}

inline void compass_set_north(void)
{
    north = compass_angle_raw();
}

bool centered(uint16_t range)
{
    uint16_t c = (uint16_t) compass_angle();
    return ((c >= (360 - range)) || (c <= range));
}

#elif COMPASS_TYPE == HMC6352_COMPASS

RelativeHMC6352 compass = RelativeHMC6352();

void setup_compass(void) {;}

inline uint16_t compass_angle(void)
{
    return (uint16_t) compass.angle();
}

inline uint16_t compass_angle_raw(void)
{
    return (uint16_t) compass.angle_raw();
}

inline uint16_t compass_north(void)
{
    return (uint16_t) compass.real_north();
}

inline void compass_set_north_val(uint16_t value)
{
    compass.set_north_val((float)value);
}

inline void compass_set_north(void)
{
    compass.set_north();
}

bool centered(uint16_t range)
{
    uint16_t c = (uint16_t) compass_angle();
    return ((c >= (360 - range)) || (c <= range));
}

#elif COMPASS_TYPE == MPU6050_COMPASS

MPU6050 mpu;
uint16_t packetSize;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
float euler[3];
float ypr[3];
uint16_t north;

void setup_compass(void)
{
    Wire.begin();
    north = 0;
    mpu.initialize();
    if (mpu.testConnection() == 0 || mpu.dmpInitialize() != 0) {
        vic_println("MPU6050 setup crashed!");
        return;
    }
    mpu.setXGyroOffset(16);
    mpu.setYGyroOffset(-54);
    mpu.setZGyroOffset(21);
    mpu.setXAccelOffset(-2865);
    mpu.setYAccelOffset(-1282);
    mpu.setZAccelOffset(2091);

    mpu.setDMPEnabled(true);
    packetSize = mpu.dmpGetFIFOPacketSize();
}

inline uint16_t compass_angle(void)
{
    return (compass_angle_raw() - north + 360) % 360;
}

uint16_t compass_angle_raw(void)
{
    mpu.resetFIFO();
    while (mpu.getFIFOCount() < packetSize) ;

    mpu.getFIFOBytes(fifoBuffer, packetSize);
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    return (uint16_t)(ypr[0] * 180/M_PI + 360.0) % 360;
}

inline uint16_t compass_north(void)
{
    return north;
}

inline void compass_set_north_val(uint16_t value)
{
    north = value;
}

inline void compass_set_north(void)
{
    north = compass_angle_raw();
}

bool centered(uint16_t range)
{
    uint16_t c = (uint16_t) compass_angle();
    return ((c >= (360 - range)) || (c <= range));
}

#endif
 
/* VIC FUNCTIONS */

inline void vic_compass_angle(void)
{
    vic_printf("%u\n", compass_angle());
}

inline void vic_compass_north(void)
{
    vic_printf("%u\n", (uint16_t) compass_north());
}

inline void vic_compass_set_north(void)
{
    compass_set_north();
}
