RelativeHMC6352 compass = RelativeHMC6352();

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

void centralizing() 
{
  int c = (int)compass_angle();
    if (c > 180) {
      c = 360 - c;
      rotate(SPEED_ROTATE_CENTRALIZE/2 + (c*SPEED_ROTATE_CENTRALIZE/180));
   } else {
      rotate(-(SPEED_ROTATE_CENTRALIZE/2 + (c*SPEED_ROTATE_CENTRALIZE/180)));
      } 
  }
