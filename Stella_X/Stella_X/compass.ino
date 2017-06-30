#if USE_LEGO_COMPASS == 1

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

#else

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
