void serial_compass()
{
    vic_println(compass.angle());
}

void compass_load()
{
    compass.set_north();
}

void compass_default()
{
    vic_println(compass.real_north());
}

void serial_compass_loop_start()
{
    compass_loop = 1;
}

void serial_compass_loop_stop()
{
    compass_loop = 0;
}

boolean centered(int range)
{
    int c = compass_angle;
    return ((c >= (360.0 - range)) || (c <= range));
}

void centralize() 
{
    int c = (int) compass_angle;
    if (c > 180) {
        c = 360 - c;
        rrotate(min(speed_rotate/2 + (c*speed_rotate/180), speed));
    } else {
        lrotate(min(speed_rotate/2 + (c*speed_rotate/180), speed)); //20,18
    }
}



