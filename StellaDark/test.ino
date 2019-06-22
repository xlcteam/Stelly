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
void test_motors() {
	motorA.go(255);
	motorB.go(255);
	motorC.go(255);
	delay(1000);
	motorA.go(100);
	motorB.go(100);
	motorC.go(100);
	delay(1000);
	motorA.go(0);
	motorB.go(0);
	motorC.go(0);
	delay(1000);
	motorA.go(-100);
	motorB.go(-100);
	motorC.go(-100);
	delay(1000);
	motorA.go(-255);
	motorB.go(-255);
	motorC.go(-255);
	delay(1000);
}
void test_frequency() {
	static bool position_in_program = 0;
	static long last_time = 0;
	if (position_in_program == 1) {
		Serial.print("frequency is:");
		Serial.println(millis() - last_time);
		position_in_program = 0;
	}
	else {
		last_time = millis();
		position_in_program = 1;
	}
}
void test_team() {
	pixyViSy.update(PIXYVISY_TEAM);
	Serial.print("Dist: ");
	Serial.println(team_distance());
	Serial.print("Angle: ");
	Serial.println( team_angle());
	Serial.print("Dx: ");
	Serial.println( team_Dx());
	Serial.println("////////////////////////////////////////////");
	delay(150);
}
