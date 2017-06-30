uint8_t IR_sensors_pins[IR_SENSORS_COUNT] = { IR_BOTTOM2_PIN, IR1_PIN, IR2_PIN,
    IR3_PIN, IR4_PIN, IR5_PIN, IR6_PIN, IR7_PIN, IR8_PIN, IR9_PIN, IR10_PIN,
    IR11_PIN, IR_BOTTOM3_PIN, IR_BOTTOM1_PIN };
uint16_t IR_vals_far[IR_SENSORS_COUNT] =
    { 816, 1018, 1016, 1016, 1015, 1016, 1016,
      1019, 1018, 1019, 1016, 1016, 1015, 1016 };
    /*{ 813, 1008, 1009, 1011, 1014, 1015, 1017,
      1016, 1016, 1015, 1012, 1015, 1015, 1013 };*/
uint16_t IR_vals_near[IR_SENSORS_COUNT] =
      { 96, 207, 209, 204, 206, 231, 202,
        206, 202, 204, 217,207, 157, 96};
    /*{ 116, 114, 115, 117, 117, 116, 117,
      115, 109, 111, 113, 117, 140, 163 };*/

void setup_IR_sensors()
{
    for (uint8_t i = 0; i < IR_SENSORS_COUNT; i++) {
        pinMode(IR_sensors_pins[i], INPUT);
    }
}

inline uint16_t read_IR_raw(uint8_t index)
{
    return analogRead(IR_sensors_pins[index]);
}

uint16_t read_IR(uint8_t i)
{
    uint16_t val = read_IR_raw(i);
    val = constrain(val, IR_vals_near[i], IR_vals_far[i]);
    return map(val, IR_vals_near[i], IR_vals_far[i], 0, 1024);
}

void read_IR_all(uint16_t *out_data)
{
    for (uint8_t i = 0; i < IR_SENSORS_COUNT; i++) {
        out_data[i] = read_IR(i);
    }
}

void read_IR_raw_all(uint16_t *out_data)
{
    for (uint8_t i = 0; i < IR_SENSORS_COUNT; i++) {
        out_data[i] = read_IR_raw(i);
    }
}

uint8_t max_IR()
{
    uint16_t data[IR_SENSORS_COUNT];
    uint16_t max_index = 0;

    read_IR_all(data);
    for (uint8_t i = 0; i < IR_SENSORS_COUNT - 2; i++) { // last 2 are special
        if (data[i] < data[max_index]) {
            max_index = i;
        }
    }

    return max_index;
}

/* returns angle to ball (if far, then increased by 1000)
   ~0: no ball
*/
uint16_t dir_to_ball()
{
    uint16_t data[IR_SENSORS_COUNT];
    uint8_t nearest = 0;
    read_IR_all(data);

    for (uint8_t i = 0; i < IR_SENSORS_COUNT - 2; i++) {
        if (data[i] < data[nearest]) {
            nearest = i;
        }
    }

    if (data[nearest] > IR_NO_BALL) {
        return ~0;
    }

    if (ball_in_dribbler() && data[0] < IR_NO_BALL) {
        return 0;
    }

    if (data[nearest] > IR_FAR_THRESH) {
        return 1000 + nearest * 30;
    }

    if (nearest == 1) {
        if (data[12] > IR_FAR_THRESH) {
            return 30;
        } else {
            return 15;
        }
    } else if (nearest == 11) {
        if (data[13] > IR_FAR_THRESH) {
            return 330;
        } else {
            return 345;
        }
    }
    return nearest * 30;
}

/* VIC FUNCTIONS */

void vic_read_IR(void)
{
    uint8_t index;
    if (vic_args("%hhu", &index) == 1) {
        vic_printf("%u\n", read_IR(index));
    }
}

void vic_read_IR_raw(void)
{
    uint8_t index;
    if (vic_args("%hhu", &index) == 1) {
        vic_printf("%u\n", read_IR_raw(index));
    }
}

void vic_read_IR_all(void)
{
    uint16_t data[IR_SENSORS_COUNT];
    read_IR_all(data);
    for (uint8_t i = 0; i < IR_SENSORS_COUNT - 1; i++) {
        vic_printf("%4u ", data[i]);
    }
    vic_printf("%4u\n", data[IR_SENSORS_COUNT - 1]);
}

void vic_read_IR_raw_all(void)
{
    uint16_t data[IR_SENSORS_COUNT];
    read_IR_raw_all(data);
    for (uint8_t i = 0; i < IR_SENSORS_COUNT - 1; i++) {
        vic_printf("%4u ", data[i]);
    }
    vic_printf("%4u\n", data[IR_SENSORS_COUNT - 1]);
}

void vic_dir_to_ball(void)
{
    uint16_t dir = dir_to_ball();
    if (dir == ~(uint16_t)0) {
        vic_println("no ball");
    } else if (dir > 1000) {
        vic_printf("%u far\n", dir - 1000);
    } else {
        vic_printf("%u near\n", dir);
    }
}
