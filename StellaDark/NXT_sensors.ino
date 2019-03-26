#define TCAADDR 0x70

int compass() {
  tcaselect(0);
  int compass;
  compass = compass_angle();

  if (compass_angle() > 180) {
    compass = 360 - compass_angle();
    compass = compass * -1;
  }
  return compass;
}

void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}
// COMPASS
int16_t _north = 0;


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

void compass_set_north()
{
   tcaselect(0);
    _north = compass_raw();
}

uint16_t compass_angle()
{
    uint16_t relative_angle = (compass_raw() - _north + 360) % 360;

    if (relative_angle < 0)
        relative_angle += 360;

    return relative_angle;
}

uint16_t compass_real_north()
{
    return _north;
}
int dir,s1,s3,s5,s7,s9;

// IR_SEEKER
//seeker.read(dir,s1,s3,s5,s7,s9);
void HTIRSeekerV2(uint8_t *dir, uint8_t *s1, uint8_t *s2, uint8_t *s3, uint8_t *s4,
                    uint8_t *s5)
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
        while (Wire.available()) Wire.read();
    }
}
int IRseeker1(uint8_t *dir, uint8_t *s1, uint8_t *s3, uint8_t *s5, uint8_t *s7, uint8_t *s9) {
  tcaselect(1);
  HTIRSeekerV2(dir, s1, s3, s5, s7, s9);

}

int IRseeker2(uint8_t *dir, uint8_t *s1, uint8_t *s3, uint8_t *s5, uint8_t *s7, uint8_t *s9) {
  tcaselect(2);
  HTIRSeekerV2(dir, s1, s3, s5, s7, s9);

}
