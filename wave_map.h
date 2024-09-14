enum Map_display{
    AMPLITUDE,
    ENERGY,
    ENERGY_KINETIC,
    ENERGY_POTENTIAL,
};

int wave_width = 384;
int wave_height = 384; //must be wave_width
double resolution = 5.0; //length of space displayed per segment
int pixel_resolution = 6; //pixel per segment. Must be divider of wave_width
int segments = 64; //segments per width und per height. Must be wave_width/pixel_resolution
int pos_x = 0; //left edge
int pos_y = 0; //upper edge
int screen_width =  0;
Color wave_edge_color = GRAY;
Color wave_bg_color = LIGHTGRAY;
Color wave_re_line_color = BLUE;
Color wave_im_line_color = RED;
Color wave_zeroline_color = GRAY;
int wave_center_focus_old = -1;


double data[128][128]; //must be [segment][segment]
Color data_color[128][128]; //must be [segment][segment]

double max_amplitude = 0.0;
bool show_wave_heat_map = false;
//bool heat_map_uptodate = false;
enum Axis_plane axis_displayed = XY_AXIS;
enum Axis_plane axis_displayed_old = XY_AXIS;
bool double_resolution = false;
bool double_resolution_old = false;
bool raster = false;
//int reflections = 0;
bool reflections_only = false;
bool reflections_only_old = false;
//int axis_displayed = 1;
enum Map_display display_mode = AMPLITUDE;
enum Map_display display_mode_old = AMPLITUDE;


Color heat_map_color(double value, double max_value, double fix_value, bool is_fix_value)
{
    double lerp = 0.0;
    if(is_fix_value && value <= fix_value) lerp = value/fix_value;
    else if(is_fix_value) lerp = 1.0 + (4.0*(value-fix_value) / (max_value-fix_value));
    else lerp = 5.0*value / max_value;
    int r = 0, g = 0, b = 0;
    if(lerp <= 1.0) b = (int)(lerp*255.0);
    else if(lerp <= 2.0) {b = 255; g = (int)((lerp-1.0)*255.0);}
    else if(lerp <= 3.0) {b = (int)((3.0-lerp)*255.0); g = 255;}
    else if(lerp <= 4.0) {g = 255; r = (int)((lerp-3.0)*255.0);}
    else {r = 255; g = (int)((5.0-lerp)*255.0);}
    return (Color){r,g,b,255};
}

int wave_determine_line( )
{
    max_amplitude = 0.0;
    double value = 0.0;
    double x_center = 0.0;
    double y_center = 0.0;
    double z_center = 0.0;
    if(wave_center_focus>=0)
    {
        x_center = wave_centers[wave_center_focus].pos.x;
        y_center = wave_centers[wave_center_focus].pos.y;
        z_center = wave_centers[wave_center_focus].pos.z;
    }
    double x = x_center;
    double y = z_center;
    double z = 0.0;
    double displaced_center = 0.5*(segments-1);
    for(int i = 0; i <segments; i++){for(int j = 0; j <segments; j++){
        if(axis_displayed == XY_AXIS || axis_displayed == XZ_AXIS) x = x_center + i * resolution - displaced_center*resolution;
        if(axis_displayed == XY_AXIS ) y = y_center + j * resolution - displaced_center*resolution;
        else if(axis_displayed == YZ_AXIS ) y = y_center + i * resolution - displaced_center*resolution;
        if(axis_displayed == XZ_AXIS || axis_displayed == YZ_AXIS) z = z_center + j * resolution - displaced_center*resolution;
        if(display_mode == AMPLITUDE)
            value = get_amplitude_abs((struct Vec3){.x=x,.y=y,.z=z}, gv_global_time, reflections_only, wave_center_count_max+1);
        else if(display_mode == ENERGY)
            value = get_energy_abs((struct Vec3){.x=x,.y=y,.z=z}, gv_global_time, reflections_only, wave_center_count_max+1);
        else if(display_mode == ENERGY_KINETIC)
            value = get_kinetic_energy_abs((struct Vec3){.x=x,.y=y,.z=z}, gv_global_time, reflections_only, wave_center_count_max+1);
        else if(display_mode == ENERGY_POTENTIAL)
            value = get_potential_energy_abs((struct Vec3){.x=x,.y=y,.z=z}, gv_global_time, reflections_only, wave_center_count_max+1);
        data[i][j] = value;
        if(value > max_amplitude)max_amplitude = value;
        
    }}
   // printf("max_amplitude %f \n",max_amplitude);
    for(int i = 0; i <segments; i++){for(int j = 0; j <segments; j++){
        data_color[i][j] = heat_map_color(data[i][j], max_amplitude, gv_amplitude_longitudinal, false);//(reflections==0)
    }}
    heat_map_uptodate = true;
    return 0;
}

int wave_update()
{
    // if(particle_focus<0) return 0;
    // wave_determine_line(particles[particle_focus].psi, X_AXIS);
    return 0;
}
int wave_init()
{   
    pos_x = GetScreenWidth()-wave_width-8;
    pos_y = 38;
    wave_update();
    return 0;
}

int wave_draw_amplitudes()
{
    for(int i = 0; i <segments; i++){for(int j = 0; j <segments; j++){
        //DrawPixel(pos_x+4*i, pos_y+4*j, data_color[i][j]);
        DrawRectangle(pos_x+pixel_resolution*i, pos_y+wave_height-pixel_resolution-pixel_resolution*j, pixel_resolution, pixel_resolution, data_color[i][j]); 
    }}
    return 0;
}
int wave_draw_center()
{
    int x = 0;
    int y = 0;
    for(int i = 0; i < wave_center_count; i++)
    {
        if(axis_displayed == XY_AXIS || axis_displayed == XZ_AXIS)
            x = (int)((wave_centers[i].pos.x - wave_centers[wave_center_focus].pos.x ) /resolution);
        else
            x = (int)((wave_centers[i].pos.y - wave_centers[wave_center_focus].pos.y ) /resolution);
        if(fabs(x)>0.5*segments-0.5) continue;
        if(axis_displayed == XY_AXIS )
            y = (int)((wave_centers[i].pos.y - wave_centers[wave_center_focus].pos.y ) /resolution);
        else
            y = (int)((wave_centers[i].pos.z - wave_centers[wave_center_focus].pos.z ) /resolution);
        if(fabs(y)>0.5*segments-0.5) continue;
        DrawRectangle(pos_x+192+x*pixel_resolution-pixel_resolution/2, pos_y+192-y*pixel_resolution-pixel_resolution/2, 
        pixel_resolution, pixel_resolution, (Color){255,0,255,255}); 
    }
    return 0;
}
int wave_draw_color_scheme()
{
    for(int i = 0; i < 40; i++)
    {
        DrawRectangle(pos_x + i*5, pos_y+wave_height+8, 5,30, heat_map_color( (double)i,39.0,39.0,false ));
    }
    char buffer[16];
    sprintf(buffer,"%.2e",max_amplitude);
    DrawText(buffer, pos_x+212, pos_y+wave_height+13, 20, DARKGRAY);
    return 0;
}
int change_resolution(int direction)
{
    if(direction == 0) return 0;
    if(direction<0) resolution *= 0.75;
    else resolution *= 1.25;
    heat_map_uptodate = false;
    return 0;
}
int change_segments()
{
    if(double_resolution)
    {
        segments = 128;
        pixel_resolution = 3;
        resolution *= 0.5;
    }
    else
    {
        segments = 64;
        pixel_resolution = 6;
        resolution *= 2.0;
    }
    return 0;
}
int wave_draw()
{
    screen_width = GetScreenWidth();
    pos_x = screen_width-wave_width-8;
    GuiToggle((Rectangle){ screen_width-124, 4, 120, 30 }, "[h]eat map",&show_wave_heat_map);
    if(!show_wave_heat_map) return 0;
    
    
    DrawRectangle(pos_x, pos_y, wave_width, wave_height, wave_bg_color); 
    DrawRectangleLines(pos_x, pos_y, wave_width, wave_height, wave_edge_color);
    //DrawLine(pos_x,pos_y+wave_height/2,pos_x+wave_width,pos_y+wave_height/2,wave_zeroline_color); 
    if(GuiButton((Rectangle){ pos_x, pos_y+wave_height+46, 30, 30 }, "+"))change_resolution(-1);
    if(GuiButton((Rectangle){ pos_x+34, pos_y+wave_height+46, 30, 30 }, "-"))change_resolution(1);
    GuiToggleGroup((Rectangle){ pos_x+72, pos_y+wave_height+46, 30, 30 }, "X-Y;X-Z;Y-Z",(int*)(&axis_displayed));
    GuiToggle((Rectangle){ pos_x+174, pos_y+wave_height+46, 30, 30 }, "2x",&double_resolution);
    GuiToggle((Rectangle){ pos_x+212, pos_y+wave_height+46, 60, 30 }, "refl. only",&reflections_only);
    //GuiToggle((Rectangle){ pos_x+280, pos_y+wave_height+4, 40, 30 }, "raster",&raster);
    GuiToggleGroup((Rectangle){ pos_x, pos_y+wave_height+86, 60, 30 }, "Ampl;Energy;kin. E;pot. E",(int*)(&display_mode));
    wave_draw_color_scheme();
    if(axis_displayed_old != axis_displayed)
    {
        axis_displayed_old = axis_displayed;
        heat_map_uptodate = false;
    }
    if(display_mode_old != display_mode)
    {
        display_mode_old = display_mode;
        heat_map_uptodate = false;
    }
    if(double_resolution_old != double_resolution)
    {
        double_resolution_old = double_resolution;
        change_segments();
        heat_map_uptodate = false;
    }
    if(reflections_only_old != reflections_only)
    {
        reflections_only_old = reflections_only;
        heat_map_uptodate = false;
    }
    if(wave_center_focus_old != wave_center_focus)
    {
        wave_center_focus_old = wave_center_focus;
        heat_map_uptodate = false;
    }
    if(!heat_map_uptodate) wave_determine_line( );
    wave_draw_amplitudes();
    wave_draw_center();
    //scale
    DrawLine(pos_x+20,pos_y+40,pos_x+120,pos_y+40,WHITE);
    DrawLine(pos_x+20,pos_y+35,pos_x+20,pos_y+45,WHITE);
    DrawLine(pos_x+120,pos_y+35,pos_x+120,pos_y+45,WHITE);
    char buffer[16];
    sprintf(buffer,"%.2e",100.0*resolution/pixel_resolution);
    DrawText(buffer, pos_x+34, pos_y+12, 20, WHITE);
    // if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message")) printf("Button!\n");
    
    return 0;
}

int heat_map_new_focus()
{
    heat_map_uptodate = false;
    return 0;
}
int heat_map_update_time()
{
    heat_map_uptodate = false;
    return 0;
}

