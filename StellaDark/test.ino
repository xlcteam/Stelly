void test_sensors() {
  uint8_t dir, s1, s3, s5, s7, s9;
  IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
  Serial.print(dir);
  Serial.print('\t');
  IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
  Serial.print(dir);
  Serial.print('\t');
  Serial.println(compass());
  delay(50);
}
void test_motors(){
    motorA.go(255);
    motorB.go(255);
    motorC.go(255);
                  }
