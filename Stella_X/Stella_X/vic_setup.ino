void setup_vic_funcs()
{
    /* buttons */
    vic_fn_add("btn", vic_btn_state);
    vic_fn_add("btna", vic_all_btns_state);

    /* leds */
    vic_fn_add("led", vic_led_set);
    vic_fn_add("leda", vic_leds_set_all);

    /* compass */
    vic_fn_add("ca", vic_compass_angle);
    vic_fn_add("cn", vic_compass_north);
    vic_fn_add("csn", vic_compass_set_north);

    /* IR seekers */
    vic_fn_add("IR", vic_read_IR);
    vic_fn_add("IRr", vic_read_IR_raw);
    vic_fn_add("IRra", vic_read_IR_raw_all);
    vic_fn_add("IRa", vic_read_IR_all);
    vic_fn_add("IRd", vic_dir_to_ball);

    /* line sensors */
    vic_fn_add("lS", vic_print_line_sensors);

    /* camera */
#if USE_PIXY == 1
    vic_fn_add("cam", vic_camera_data);
#endif

    /* dribbler & kicker */
    vic_fn_add("d1", vic_dribbler_on);
    vic_fn_add("d0", vic_dribbler_off);
    vic_fn_add("dk", vic_dribbler_kick);
    vic_fn_add("db", vic_ball_in_dribbler);

    /* motors */
    vic_fn_add("mA", vic_mA);
    vic_fn_add("mB", vic_mB);
    vic_fn_add("mC", vic_mC);

    vic_fn_add("halt", halt);
    vic_fn_add("brake", brake);

    vic_fn_add("u", vic_move_up);
    vic_fn_add("r", vic_move_right);
    vic_fn_add("b", vic_move_back);
    vic_fn_add("l", vic_move_left);
    vic_fn_add("ur", vic_move_up_right);
    vic_fn_add("br", vic_move_back_right);
    vic_fn_add("bl", vic_move_back_left);
    vic_fn_add("ul", vic_move_up_left);

    vic_fn_add("rePID", vic_restart_PID);
    vic_fn_add("mv", vic_move);
}

void setup_vic_vars()
{
    vic_var_set("tskid", XSTR(START_TASK));
    vic_var_bind("tskid", (void *)&task_id, VIC_VAR_UINT8);
    vic_var_set("lsthresh", XSTR(LINE_THRESH));
    vic_var_bind("lsthresh", (void *)&light_pwm, VIC_VAR_UINT8);
    vic_var_set("dT", "0");
    vic_var_bind("dT", (void *)&delta_time, VIC_VAR_UINT16);
    vic_var_set("uli", XSTR(LINE_USE_INT));
    vic_var_bind("uli", (void *)&line_use_int, VIC_VAR_UINT8);
    vic_var_set("upixy", XSTR(USE_PIXY));
    vic_var_bind("upixy", (void *)&use_pixy, VIC_VAR_UINT8);
}
