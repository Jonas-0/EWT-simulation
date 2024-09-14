
int steps_per_second = 0;
double time_steps = 0.0;
bool no_movement = false;

int time_step()
{
    gv_global_time += gv_time_step;
    gv_time_steps ++;
    update_time();
    heat_map_update_time();
    return 0;
}

int time_control_update()
{
    time_steps+=0.0167*steps_per_second;
    if(time_steps >= 1.0)
    {
        time_steps-=1.0;
        gv_global_time += gv_time_step;
        gv_time_steps ++;
        update_time();
        heat_map_update_time();
    }
    
    return 0;
}

int draw_time_control()
{
    DrawText("time control", 10, GetScreenHeight()-86, 20, DARKGRAY);
    if(GuiButton((Rectangle){ 10, GetScreenHeight()-50, 80, 30 }, "[t]ime step"))time_step();
    GuiSpinner((Rectangle){ 172, GetScreenHeight()-50, 140, 30 },"steps per  \nseconds    ",&steps_per_second,0,10,false);
    GuiToggle((Rectangle){ 320, GetScreenHeight()-50, 80, 30 }, "no movement",&wave_center_no_movement);
    GuiToggle((Rectangle){ 408, GetScreenHeight()-50, 80, 30 }, "refl. only",&differential_reflections_only);
    return 0;
}