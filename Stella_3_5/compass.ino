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
    float c = compass.angle();
    return ((c >= (360.0 - range)) || (c <= range));
}

void centralize() 
{
    int c = (int) compass.angle();
    if (c > 180) {
        c = 360 - c;
        rrotate(speed_rotate/2 + (c*speed_rotate/180) );
    } else {
        lrotate(speed_rotate/2 + (c*speed_rotate/180) ); //20,18
    }
}



