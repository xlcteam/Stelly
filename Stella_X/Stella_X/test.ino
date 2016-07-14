void test_start()
{
    compass_set_north();
    lcd.clear();
    lcd.print("test");
    led_set(0, 0);
    Serial.println("test_start");
}

void test_motors()
{
    static uint8_t action = 1;
    static uint32_t time = millis();
    if (action == 1)
        move_up(SPEED);
    else if (action == 2)
        move_back(SPEED);
    else if (action == 3)
        move_left(SPEED);
    else
        move_right(SPEED);
    if (millis() - time > 1000) {
        action = (action + 1) % 4;
    }
}

char dir = 'L';
void test_loop()
{
    test_motors();
    /*
    uint8_t l_dir = line_sensors_dir();
    if (l_dir == 2) {
        dir = 'R';
    } else if (l_dir == 6) {
        dir = 'L';
    }
    if (dir == 'L') {
        move_left(230);
    } else if (dir == 'R') {
        move_right(230);
    }
    */
}
