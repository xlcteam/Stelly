/**
 * version 0.1
 * made by xlc team 2019
*/
#include <Wire.h>
#include <Adafruit_MCP23017.h>

Adafruit_MCP23017 mcp_1;
Adafruit_MCP23017 mcp_2;
Adafruit_MCP23017 mcp_3;
uint16_t values_1;
uint16_t values_d_1;
uint16_t values_2;
uint16_t values_d_2;
uint16_t values_3;
uint16_t values_d_3;
uint8_t index_sensor;
uint8_t detected;
float x_result;
float y_result;
const float x_for_angle[48] {
  0,
  0.1305,
  0.2588,
  0.3827,
  0.5,
  0.6088,
  0.7071,
  0.7934,
  0.866,
  0.9239,
  0.9659,
  0.9914,
  1,
  0.9914,
  0.9659,
  0.9239,
  0.866,
  0.7934,
  0.7071,
  0.6088,
  0.5,
  0.3827,
  0.2588,
  0.1305,
  0,
  -0.1305,
  -0.2588,
  -0.3827,
  -0.5,
  -0.6088,
  -0.7071,
  -0.7934,
  -0.866,
  -0.9239,
  -0.9659,
  -0.9914,
  -1,
  -0.9914,
  -0.9659,
  -0.9239,
  -0.866,
  -0.7934,
  -0.7071,
  -0.6088,
  -0.5,
  -0.3827,
  -0.2588,
  -0.1305
};
const float y_for_angle[48] {
  1,
  0.9914,
  0.9659,
  0.9239,
  0.866,
  0.7934,
  0.7071,
  0.6088,
  0.5,
  0.3827,
  0.2588,
  0.1305,
  0,
  -0.1305,
  -0.2588,
  -0.3827,
  -0.5,
  -0.6088,
  -0.7071,
  -0.7934,
  -0.866,
  -0.9239,
  -0.9659,
  -0.9914,
  -1,
  -0.9914,
  -0.9659,
  -0.9239,
  -0.866,
  -0.7934,
  -0.7071,
  -0.6088,
  -0.5,
  -0.3827,
  -0.2588,
  -0.1305,
  0,
  0.1305,
  0.2588,
  0.3827,
  0.5,
  0.6088,
  0.7071,
  0.7934,
  0.866,
  0.9239,
  0.9659,
  0.9914
};
int radToDeg(float rad) { 
  return rad*(180/3.14);
  }
int vectorAngle(int x, int y) {
  if (x == 0)
    return (y > 0) ? 90 : (y == 0) ? 0 : 270;
  else if (y == 0)
    return (x >= 0) ? 0 : 180;
  int ret = radToDeg(atanf((float)y / x));
  if (x < 0 && y < 0)
    ret = 180 + ret;
  else if (x < 0)
    ret = 180 + ret;
  else if (y < 0)
    ret = 270 + (90 + ret);
  return ret;
}
void setup()
{
  Serial.begin(115200);
  Wire.begin();
  mcp_1.begin(0x20);
  mcp_2.begin(0x21);
  mcp_3.begin(0x22);
  for (uint8_t i = 0; i < 16; i++) {
    mcp_1.pinMode(i, INPUT);
    mcp_1.pullUp(i, 1);
    mcp_2.pinMode(i, INPUT);
    mcp_2.pullUp(i, 1);
    mcp_3.pinMode(i, INPUT);
    mcp_3.pullUp(i, 1);
  }
}

void loop()
{
  x_result = 0;
  y_result = 0;
  index_sensor = 0;
  values_1 = mcp_1.readGPIOAB();
  values_2 = mcp_2.readGPIOAB();
  values_3 = mcp_3.readGPIOAB();
  values_1 |= values_1 >> 1;
  values_2 |= values_2 >> 1;
  values_3 |= values_3 >> 1;
  for (uint16_t mask = 0b1; mask != 0b0; mask <<= 1) {
    if ((values_1 & mask) > 0) {
      x_result += x_for_angle[index_sensor];
      y_result += y_for_angle[index_sensor];
      detected++;
    }
    index_sensor++;
  }
  for (uint16_t mask = 0b1; mask != 0b0; mask <<= 1) {
    if ((values_2 & mask) > 0) {
      x_result += x_for_angle[index_sensor];
      y_result += y_for_angle[index_sensor];
      detected++;
    }
    index_sensor++;
  }
  for (uint16_t mask = 0b1; mask != 0b0; mask <<= 1) {
    if ((values_3 & mask) > 0) {
      x_result += x_for_angle[index_sensor];
      y_result += y_for_angle[index_sensor];
      detected++;
    }
    index_sensor++;
  }
  if (detected > 2) { //filter
    Serial.println(vectorAngle(x_result, y_result));//print angle 0-360
  }
  else{
     Serial.println(~0);    //no line
    }
}
