#define TCAADDR 0x70
#define TCA_COMPASS 0
#define TCA_IR_FRONT 1
#define TCA_IR_BACK 2

void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// COMPASS

int16_t compass() {
    int16_t compass = compass_angle();
    if (compass > 180) {
        compass -= 360;
    }

    return compass;
}

int16_t _compass_north = 0;

void compass_set_north()
{
   tcaselect(TCA_COMPASS);
   _compass_north = compass_raw();
}

uint16_t compass_raw()
{
    uint16_t a, b;
    Wire.beginTransmission(0x02 >> 1);
    Wire.write(0x44); 
    Wire.endTransmission();
    Wire.requestFrom(0x02 >> 1, 2);
    if (Wire.available() >= 2) {
        a = Wire.read();
        b = Wire.read();
        while (Wire.available()) Wire.read();
    }

    return a + b * 256;
}

uint16_t compass_angle()
{
    tcaselect(TCA_COMPASS);
    return (compass_raw() - _compass_north + 360) % 360;
}

uint16_t compass_real_north()
{
    return _compass_north;
}

// IR_SEEKER

void HTIRSeekerV2(uint8_t *dir, uint8_t *s1, uint8_t *s2,
                  uint8_t *s3, uint8_t *s4, uint8_t *s5)
{
    Wire.beginTransmission(0x10 >> 1);
    Wire.write(0x49);
    Wire.endTransmission();
    Wire.requestFrom(0x10 >> 1, 6);
    if (Wire.available() >= 6) {
        *dir = Wire.read();
        *s1  = Wire.read();
        *s2  = Wire.read();
        *s3  = Wire.read();
        *s4  = Wire.read();
        *s5  = Wire.read();
        while (Wire.available()) {
            Wire.read();
        }
    }
}

void IRseeker1(uint8_t *dir, uint8_t *s1, uint8_t *s3,
              uint8_t *s5, uint8_t *s7, uint8_t *s9)
{
    tcaselect(TCA_IR_FRONT);
    HTIRSeekerV2(dir, s1, s3, s5, s7, s9);
}

void IRseeker2(uint8_t *dir, uint8_t *s1, uint8_t *s3,
              uint8_t *s5, uint8_t *s7, uint8_t *s9)
{
    tcaselect(TCA_IR_BACK);
    HTIRSeekerV2(dir, s1, s3, s5, s7, s9);
}
