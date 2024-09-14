double gv_global_time = 0.0; //nano seconds e-9
double gv_time_step = 0.01;   //nano seconds e-9
int gv_time_steps = 0;

bool heat_map_uptodate = false;

double gv_c = 299.792458; //mikrometer per nano seconds e-9;
double gv_amplitude_longitudinal = 2.85409650;//1e-17;//e-15
double gv_background_wavelength_x = 30.0;//per nano seconds e-9; 300 ir light
double gv_background_wavelength_y = 30.0;
double gv_background_wavelength_z = 30.0;
double gv_r0 = 3.0; // 1/ (2*PI*gv_background_frequency)
double gv_r_cutoff = 3.00; // 1000*gv_r0, ignores amplitudes of one promille of gv_amplitude_longitudinal
double gv_delta_r = 3.0;
double gv_diff_factor = 10.0;
double gv_attenuation = 1.0;

char gv_wl_x[128] = "300.0";
char gv_wl_y[128] = "300.0";
char gv_wl_z[128] = "300.0";
char gv_f_x[128] = "300.0";
char gv_f_y[128] = "300.0";
char gv_f_z[128] = "300.0";
char gv_t_x[128] = "300.0";
char gv_t_y[128] = "300.0";
char gv_t_z[128] = "300.0";
char gv_r0_txt[128] = "300.0";
char gv_delta_r_txt[128] = "200.0";
char gv_delta_t_txt[128] = "300.0";
char gv_acc_txt[128] = "1.0";
char gv_radius_displayed_txt[128] = "0.2";
char gv_attenuation_txt[128] = "1.0";
char gv_c_txt[128] = "0.2";

double wave_center_radius = 0.2;
int gv_reflections = 2;
bool show_global_var_control = false;//synced with and logic in wave_center_setup; see show_global_var_control_old
bool focus_freq_x = false;
bool focus_freq_y = false;
bool focus_freq_z = false;
bool focus_r0 = false;
bool focus_delta_r = false;
bool focus_diff_factor = false;
bool focus_delta_t = false;
bool focus_display_radius = false;
bool focus_attenuation = false;
bool focus_c = false;

double text_to_double(char* input_text)
{
    //strtod for exceptions
    return atof(input_text);
}


int frequencies_update()
{
    sprintf(gv_wl_x, "%.3e", gv_background_wavelength_x);
    sprintf(gv_wl_y, "%.3e", gv_background_wavelength_y);
    sprintf(gv_wl_z, "%.3e", gv_background_wavelength_z);
    
    if(gv_background_wavelength_x != 0.0)
    {
        sprintf(gv_f_x, "%.3e", gv_c/gv_background_wavelength_x);
        sprintf(gv_t_x, "%.3e", gv_background_wavelength_x/gv_c);
    }
    else
    {
        sprintf(gv_f_x , "0.0");
        sprintf(gv_t_x , "0.0");
    }
    if(gv_background_wavelength_y != 0.0)
    {
        sprintf(gv_f_y, "%.3e", gv_c/gv_background_wavelength_y);
        sprintf(gv_t_y, "%.3e", gv_background_wavelength_y/gv_c);
    }
    else
    {
        sprintf(gv_f_y , "0.0");
        sprintf(gv_t_y , "0.0");
    }
    if(gv_background_wavelength_z != 0.0)
    {
        sprintf(gv_f_z, "%.3e", gv_c/gv_background_wavelength_z);
        sprintf(gv_t_z, "%.3e", gv_background_wavelength_z/gv_c);
    }
    else
    {
        sprintf(gv_f_z , "0.0");
        sprintf(gv_t_z , "0.0");
    }
    return 0;
}
int init_global_var()
{ 
    frequencies_update();
    sprintf(gv_c_txt , "%.3e",gv_c);
    sprintf(gv_r0_txt , "%.3e",gv_r0);
    sprintf(gv_delta_r_txt , "%.3e",gv_delta_r);
    sprintf(gv_delta_t_txt , "%.3e",gv_time_step);
    sprintf(gv_acc_txt , "%.3e",gv_diff_factor);
    sprintf(gv_radius_displayed_txt , "%.3e",wave_center_radius);

    //sprintf(gv_radius_displayed_txt, "%.3e", wave_center_radius);
    return 0;
}
int convert_bg_frequencies()
{
    //printf("parse freq\n");
    gv_background_wavelength_x = text_to_double(gv_wl_x);
    gv_background_wavelength_y = text_to_double(gv_wl_y);
    gv_background_wavelength_z = text_to_double(gv_wl_z);
    frequencies_update();
    return 0;
}
int global_var_draw()
{
    GuiToggle((Rectangle){ 132, 4, 120, 30 }, "variables",&show_global_var_control);
    if(!show_global_var_control) return 0;
    GuiSpinner((Rectangle){ 64, 42, 80, 30 },"reflections ",&gv_reflections,1,4,false);
    GuiLabel((Rectangle){ 4, 76, 20, 30 },"c");
    if(GuiTextBox((Rectangle){ 24, 76, 70, 30 },gv_c_txt,16,focus_c)){focus_c = !focus_c;gv_c = text_to_double(gv_c_txt);frequencies_update();}
    DrawText("background wavelength", 4, 112, 17, DARKGRAY);
    GuiLabel((Rectangle){ 4, 140, 20, 30 },"x");
    if(GuiTextBox((Rectangle){ 24, 140, 70, 30 },gv_wl_x,16,focus_freq_x)){focus_freq_x = !focus_freq_x;convert_bg_frequencies();}
    GuiLabel((Rectangle){ 98, 140, 20, 30 },"y");
    if(GuiTextBox((Rectangle){ 118, 140, 70, 30 },gv_wl_y,16,focus_freq_y)){focus_freq_y = !focus_freq_y;convert_bg_frequencies();}
    GuiLabel((Rectangle){ 192, 140, 20, 30 },"z");
    if(GuiTextBox((Rectangle){ 212, 140, 70, 30 },gv_wl_z,16,focus_freq_z)){focus_freq_z = !focus_freq_z;convert_bg_frequencies();}
    GuiLabel((Rectangle){ 4, 186, 20, 30 },"fx");
    if(GuiTextBox((Rectangle){ 24, 186, 70, 30 },gv_f_x,16,false));
    GuiLabel((Rectangle){ 98, 186, 20, 30 },"fy");
    if(GuiTextBox((Rectangle){ 118, 186, 70, 30 },gv_f_y,16,false));
    GuiLabel((Rectangle){ 192, 186, 20, 30 },"fz");
    if(GuiTextBox((Rectangle){ 212, 186, 70, 30 },gv_f_z,16,false));
    GuiLabel((Rectangle){ 4, 222, 20, 30 },"Tx");
    if(GuiTextBox((Rectangle){ 24, 222, 70, 30 },gv_t_x,16,false));
    GuiLabel((Rectangle){ 98, 222, 20, 30 },"Ty");
    if(GuiTextBox((Rectangle){ 118, 222, 70, 30 },gv_t_y,16,false));
    GuiLabel((Rectangle){ 192, 222, 20, 30 },"Tz");
    if(GuiTextBox((Rectangle){ 212, 222, 70, 30 },gv_t_z,16,false));
    GuiLabel((Rectangle){ 4, 248+32, 70, 30 },"r0");
    if(GuiTextBox((Rectangle){ 74, 248+32, 70, 30 },gv_r0_txt,16,focus_r0)){focus_r0 = !focus_r0;gv_r0=text_to_double(gv_r0_txt); }
    GuiLabel((Rectangle){ 4, 284+32, 70, 30 },"delta r");
    if(GuiTextBox((Rectangle){ 74, 284+32, 70, 30 },gv_delta_r_txt,16,focus_delta_r)){focus_delta_r = !focus_delta_r;gv_delta_r=text_to_double(gv_delta_r_txt);}
    GuiLabel((Rectangle){ 4, 320+32, 70, 30 },"delta t");
    if(GuiTextBox((Rectangle){ 74, 320+32, 70, 30 },gv_delta_t_txt,16,focus_delta_t)){focus_delta_t = !focus_delta_t;gv_time_step = text_to_double(gv_delta_t_txt);}
    GuiLabel((Rectangle){ 4, 356+32, 70, 30 },"acceleration");
    if(GuiTextBox((Rectangle){ 74, 356+32, 70, 30 },gv_acc_txt,16,focus_diff_factor)){focus_diff_factor = !focus_diff_factor;gv_diff_factor =text_to_double(gv_acc_txt);}
    GuiLabel((Rectangle){ 4, 392+32, 70, 30 },"radius displ");
    if(GuiTextBox((Rectangle){ 74, 392+32, 70, 30 },gv_radius_displayed_txt,16,focus_display_radius)){focus_display_radius = !focus_display_radius;wave_center_radius=text_to_double(gv_radius_displayed_txt);}
    GuiLabel((Rectangle){ 4, 428+32, 70, 30 },"attenuation");
    if(GuiTextBox((Rectangle){ 74, 428+32, 70, 30 },gv_attenuation_txt,16,focus_attenuation)){focus_attenuation = !focus_attenuation;gv_attenuation=text_to_double(gv_attenuation_txt);}
    
    return 0;
}