enum Creation_mode{
    GRID,
    TETRA,
};

struct Wave_center wave_centers[64]; // change to wave_centers* to store only existing items
int wave_center_count_max = 64;
int wave_center_count = 0;
int wave_center_focus = -1;
bool show_wave_center_control = false;
bool show_wave_center_control_old = false;
bool show_global_var_control_old = false;
enum Creation_mode creation_mode = GRID;
int grid_x = 1, grid_y = 1, grid_z = 1;
char wave_center_distance[128] = "26.0";
bool focus_distance = false;
int tetra_level = 0;



int create_wave_center(struct Vec3 position, struct Vec3 velocity)
{
    if(wave_center_count >= wave_center_count_max) return -1;
    
    wave_centers[wave_center_count].pos = position;
    wave_centers[wave_center_count].events[0].pos = position;
    wave_centers[wave_center_count].events[0].time = gv_global_time;
    wave_centers[wave_center_count].velocity = velocity;
    wave_centers[wave_center_count].steps = 1;

    wave_center_count++;
    return 0;
} 

int create_cube(int row, int column, int depth, double distance)
{
    for(int i = 0; i < row; i++){for(int j = 0; j < column; j++){for(int k = 0; k < depth; k++){
        create_wave_center((struct Vec3){.x = i * distance - 0.5*(row-1)*distance,
        .y = j * distance - 0.5*(column-1)*distance, .z = k * distance - 0.5*(depth-1)*distance}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    }}}
    return 0;
}

double get_wave_center_spec(enum Axis axis, bool velocity)
{
    if(wave_center_focus < 0) return 0.0;
    if(velocity)
    {
        if(axis == X_AXIS) return wave_centers[wave_center_focus].velocity.x;
        if(axis == Y_AXIS) return wave_centers[wave_center_focus].velocity.y;
        if(axis == Z_AXIS) return wave_centers[wave_center_focus].velocity.z;
    }
    else
    {
        if(axis == X_AXIS) return wave_centers[wave_center_focus].pos.x;
        if(axis == Y_AXIS) return wave_centers[wave_center_focus].pos.y;
        if(axis == Z_AXIS) return wave_centers[wave_center_focus].pos.z;       
    }
    return 0.0;
}
double average_distance()
{
    double result = 0.0;
    int count = 0;
    for(int i = 0; i < wave_center_count; i++)for(int j = 0; j < wave_center_count; j++){
    {
        if(i == j) continue;
        result+=distance(wave_centers[i].pos,wave_centers[j].pos);
        count++;
    }}
    if(count == 0) return result;
    return result/(double)count;
}

int next_focus(bool backwards)
{
    if(backwards)
    {
        if(wave_center_focus <= 0) wave_center_focus = wave_center_count-1;
        else wave_center_focus--;
    }
    else
    {
        if(wave_center_focus >= wave_center_count-1) wave_center_focus = 0;
        else wave_center_focus++;
    }
    return 0;
}

int delete_all()
{
    wave_center_focus = -1;
    wave_center_count = 0;
    heat_map_uptodate = false;
    return 0;
}

int delete_wave_center()
{
    if(wave_center_focus < 0) return 0;
    for (int i = wave_center_focus+1; i < wave_center_count; i++)
    {
        wave_centers[i-1] = wave_centers[i];
    }
    wave_center_count--;
    wave_center_focus = -1;
    heat_map_uptodate = false;
    return 0;
}


int create_grid()
{
    double a = text_to_double(wave_center_distance);
    if(a<= 0.0) return 0;
    create_cube(grid_x, grid_y, grid_z, a);
    return 0;
}
int create_tetra()
{
    double a = text_to_double(wave_center_distance);
    if(a<= 0.0) return 0;
    double hd = 0.86602540378;
    double hp = 0.81649658092;
    create_wave_center((struct Vec3){.x = 0.5*a, .y = 0.0, .z = 0.0}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    create_wave_center((struct Vec3){.x = -0.5*a, .y = 0.0, .z = 0.0}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    create_wave_center((struct Vec3){.x = 0.0, .y = hd*a, .z = 0.0}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    create_wave_center((struct Vec3){.x = 0.0, .y = (1.0/3.0)* hd*a, .z = hp*a}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    if(tetra_level == 0) return 0;
    if(tetra_level == 1)
    {
    create_wave_center((struct Vec3){.x = 0.0, .y = (1.0/3.0)* hd*a, .z = -hp*a}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    return 0;
    }
    create_wave_center((struct Vec3){.x = a, .y = -(1.0/3.0)* hd*a, .z = -hp*a}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    create_wave_center((struct Vec3){.x = 0.0, .y = -(1.0/3.0)* hd*a, .z = -hp*a}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    create_wave_center((struct Vec3){.x = -a, .y = -(1.0/3.0)* hd*a, .z = -hp*a}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    
    create_wave_center((struct Vec3){.x = -0.5*a, .y = (2.0/3.0)* hd*a, .z = -hp*a}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    create_wave_center((struct Vec3){.x = 0.5*a, .y = (2.0/3.0)* hd*a, .z = -hp*a}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    
    create_wave_center((struct Vec3){.x = 0.0, .y = (5.0/3.0)* hd*a, .z = -hp*a}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    return 0;
}

int draw_wave_center_setup()
{
    GuiToggle((Rectangle){ 4, 4, 120, 30 }, "wave center",&show_wave_center_control);
    if(show_wave_center_control_old != show_wave_center_control)
    {
        show_wave_center_control_old = show_wave_center_control;
        if(show_wave_center_control && show_global_var_control)
        {
            show_global_var_control = false;
            show_global_var_control_old = false;
        }
    }
    if(show_global_var_control_old != show_global_var_control)
    {
        show_global_var_control_old = show_global_var_control;
        if(show_wave_center_control && show_global_var_control)
        {
            show_wave_center_control = false;
            show_wave_center_control_old = false;
        }
    }
    if(!show_wave_center_control) return 0;
    DrawText("position", 4, 42, 17, DARKGRAY);
    //GuiLabel((Rectangle){ 4, 4, 120, 30 },"position");
    char buffer[16];
    sprintf(buffer,"%.3e",get_wave_center_spec(X_AXIS, false));
    GuiTextBox((Rectangle){ 4, 60, 70, 30 },buffer,16,false);
    sprintf(buffer,"%.3e",get_wave_center_spec(Y_AXIS, false));
    GuiTextBox((Rectangle){ 78, 60, 70, 30 },buffer,16,false);
    sprintf(buffer,"%.3e",get_wave_center_spec(Z_AXIS, false));
    GuiTextBox((Rectangle){ 152, 60, 70, 30 },buffer,16,false);
    DrawText("velocity", 4, 100, 17, DARKGRAY);
    sprintf(buffer,"%.3e",get_wave_center_spec(X_AXIS, true));
    GuiTextBox((Rectangle){ 4, 120, 70, 30 },buffer,16,false);
    sprintf(buffer,"%.3e",get_wave_center_spec(Y_AXIS, true));
    GuiTextBox((Rectangle){ 78, 120, 70, 30 },buffer,16,false);
    sprintf(buffer,"%.3e",get_wave_center_spec(Z_AXIS, true));
    GuiTextBox((Rectangle){ 152, 120, 70, 30 },buffer,16,false);
    DrawText("average distance", 4, 158, 17, DARKGRAY);
    sprintf(buffer,"%.3e",average_distance());
    GuiTextBox((Rectangle){ 4, 178, 70, 30 },buffer,16,false);
    sprintf(buffer,"%d",wave_center_count);
    GuiTextBox((Rectangle){ 86, 178, 30, 30 },buffer,16,false);
    
    if(GuiButton((Rectangle){ 4, 160+58, 30, 30 }, "#118#"))next_focus(true);
    if(GuiButton((Rectangle){ 38, 160+58, 30, 30 }, "#119#"))next_focus(false);
    if(GuiButton((Rectangle){ 4, 200+58, 50, 30 }, "delete"))delete_wave_center();
    if(GuiButton((Rectangle){ 60, 200+58, 100, 30 }, "delete all"))delete_all();
    DrawText("creation", 4, 240+58, 17, DARKGRAY);
    GuiToggleGroup((Rectangle){ 4, 260+58, 60, 30 }, "grid;tetra",(int*)(&creation_mode));
    if(creation_mode == GRID)
    {
        GuiSpinner((Rectangle){ 22, 300+58, 80, 30 },"#x ",&grid_x,1,12,false);
        GuiSpinner((Rectangle){ 126, 300+58, 80, 30 },"#y ",&grid_y,1,12,false);
        GuiSpinner((Rectangle){ 230, 300+58, 80, 30 },"#z ",&grid_z,1,12,false);
        GuiLabel((Rectangle){ 4, 334+58, 70, 30 },"distance: ");
        if(GuiTextBox((Rectangle){ 64, 334+58, 70, 30 },wave_center_distance,16,focus_distance))focus_distance = !focus_distance;
        if(GuiButton((Rectangle){ 4, 380+58, 80, 30 }, "create")){create_grid();};
    }
    else if(creation_mode == TETRA)
    {
        GuiSpinner((Rectangle){ 36, 300+58, 80, 30 },"shape ",&tetra_level,0,2,false);
        GuiLabel((Rectangle){ 4, 334+58, 70, 30 },"distance: ");
        if(GuiTextBox((Rectangle){ 64, 334+58, 70, 30 },wave_center_distance,16,focus_distance))focus_distance = !focus_distance;
        
        if(GuiButton((Rectangle){ 4, 380+58, 80, 30 }, "create")){create_tetra();};
    }
    return 0;
}