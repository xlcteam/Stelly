LiquidCrystal lcd(
    LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

void setup_display()
{
    lcd.begin(16, 2);
    actual_screen = 0;
}

void print_display_idle()
{
    delay(LCD_DELAY);
    lcd.clear();
    lcd.display();

    lcd.print("------Idle------");
}

void print_display_1(void)
{
    lcd.print("IR   L C   D P");
    lcd.setCursor(0, 1);
    uint16_t dir = dir_to_ball();
    if (dir != ~(uint16_t)0) {
        if (dir >= 1000) {
            lcd.print(dir - 1000);
            lcd.print('F');
        } else {
            lcd.print(dir);
            lcd.print('N');
        }
    }
    lcd.setCursor(5, 1);
    lcd.print(line_for_display());
    lcd.setCursor(7, 1);
    lcd.print(compass_angle());
    lcd.setCursor(11, 1);
    lcd.print(ball_in_dribbler());
    lcd.setCursor(13, 1);
    if (use_pixy == 1) {
        pixyViSy.update();
        lcd.print(pixyViSy.getAction());
    } else if (use_pixy == 2) {
        pixyViSy.updateBall();
        lcd.print(pixyViSy.getBallAngle());
    }
}

void print_display_2(void)
{
    lcd.print("IR");
    uint16_t dir = dir_to_ball();
    if (dir != ~(uint16_t)0) {
        if (dir >= 1000) {
            lcd.print(dir - 1000);
        } else {
            lcd.print(dir);
        }
    }
    lcd.setCursor(5, 0);
    lcd.print(min(read_IR(0), 999));
    lcd.setCursor(9, 0);
    lcd.print(min(read_IR(1), 999));
    lcd.setCursor(13, 0);
    lcd.print(min(read_IR(2), 999));
    lcd.setCursor(0, 1);
    lcd.print(min(read_IR(3), 999));
    lcd.setCursor(4, 1);
    lcd.print(min(read_IR(4), 999));
    lcd.setCursor(8, 1);
    lcd.print(min(read_IR(5), 999));
    lcd.setCursor(12, 1);
    lcd.print(min(read_IR(6), 999));
}

void print_display_3(void)
{
    lcd.print("->");
    uint16_t dir = dir_to_ball();
    if (dir != ~(uint16_t)0) {
        if (dir >= 1000) {
            lcd.print(dir - 1000);
        } else {
            lcd.print(dir);
        }
    }
    lcd.setCursor(5, 0);
    lcd.print(min(read_IR(7), 999));
    lcd.setCursor(9, 0);
    lcd.print(min(read_IR(8), 999));
    lcd.setCursor(13, 0);
    lcd.print(min(read_IR(9), 999));
    lcd.setCursor(0, 1);
    lcd.print(min(read_IR(10), 999));
    lcd.setCursor(4, 1);
    lcd.print(min(read_IR(11), 999));
    lcd.setCursor(8, 1);
    lcd.print(min(read_IR(12), 999));
    lcd.setCursor(12, 1);
    lcd.print(min(read_IR(13), 999));
}

void print_display_4(void)
{
    lcd.print("WL  1 2 3 4 5 6");
    lcd.setCursor(0, 1);
    lcd.print(light_pwm);
    uint8_t i;
    for (i = 0; i < LINE_SENSORS_COUNT; i++) {
        lcd.setCursor(4 + 2 * i, 1);
        lcd.print(read_line_sensor(i));
    }
    lcd.setCursor(4 + 2 * i, 0);
    lcd.print("D");
    lcd.setCursor(4 + 2 * i, 1);
    lcd.print(line_sensors_dir());
}

void print_display_screen(void)
{
    static uint32_t time = millis();
    if (millis() - time > LCD_DELAY) {
        lcd.clear();
        lcd.display();

        switch (actual_screen) {
            case 1:
                print_display_1();
                break;
            case 2:
                print_display_2();
                break;
            case 3:
                print_display_3();
                break;
            case 4:
                print_display_4();
                break;
            default:
                lcd.print("SCREEN ERROR");
                break;
        }
        time = millis();
    }
}
