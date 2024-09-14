
struct Vec3 background_radiation(struct Event e)
{
    double x = 0.0;
    if(gv_background_wavelength_x != 0.0)
        x = gv_amplitude_longitudinal * cos(2.0*(PI*gv_c/gv_background_wavelength_x)*(e.time-e.pos.x/gv_c));
    double y = 0.0;
    if(gv_background_wavelength_y != 0.0)
        y = gv_amplitude_longitudinal * cos(2.0*(PI*gv_c/gv_background_wavelength_y)*(e.time-e.pos.y/gv_c));
    double z = 0.0;
    if(gv_background_wavelength_z != 0.0)
        z = gv_amplitude_longitudinal * cos(2.0*(PI*gv_c/gv_background_wavelength_z)*(e.time-e.pos.z/gv_c));

    return (struct Vec3){.x = x, .y = y, .z = z};
}
double background_radiation_1D(struct Event e)
{
    return v_sum(background_radiation(e));
}


int move_path(struct Event e, int current_wave_center, struct Path* path, int path_step, struct Vec3* amplitude, int reflections, bool reflection_only)
{
    if(path_step == 0)
    {
        *amplitude = background_radiation(e);
        if(reflection_only)
            *amplitude = (struct Vec3) {.x = 0.0, .y = 0.0, .z = 0.0}; // to hide background radiation to see reflections only
        path->to = e;
    }
    else
    {
        if(path_step == 1)path->from = e;
        //printf("more than 1 step!\n");
        struct Vec3 amplitude_mod;
        double amplitude_factor = 1.0;
        double extra_distance = 0.0;
        for(int i = 0; i < path_step; i++)
        {
           //amplitude_factor *= gv_c*(gv_r0 / path->r[i]);
           amplitude_factor *= fmin(1*(gv_r0 / path->r[i]) ,1.0);
           extra_distance += path->r[i];
        }
        amplitude_mod = normalized_direction_events(path->from, path->to);
        amplitude_mod = v_scalar( amplitude_mod, amplitude_factor );
        //time delay "extra_distance" already in event e
        //amplitude_mod = v_scalar( amplitude_mod,background_radiation_1D(event_plus_time(e,-1.0* extra_distance/gv_c))) ;
        amplitude_mod = v_scalar( amplitude_mod,background_radiation_1D(e));
        //if(path_step>=reflections) // to hide lower orders of reflection for debugging
            *amplitude = v_add(*amplitude, amplitude_mod);
        //*amplitude = amplitude_mod;
        //if(extra_distance >= gv_r_cutoff){ return 0;}
    }
    if(path_step >= reflections){/*printf("more than 6 steps!\n");*/ return 0;}

   
    struct Event reflection_event;
    for(int i = 0; i <wave_center_count; i++)
    {
        if(i == current_wave_center) continue;
        reflection_event = find_light_cone_event(e,&wave_centers[i]);
        if(reflection_event.time < -0.5) continue;
        path->r[path_step] = distance_events(e,reflection_event);
        move_path(reflection_event, i, path, (path_step+1), amplitude, reflections,reflection_only);
    }
    return 0;
}

struct Vec3 get_amplitude(struct Vec3 pos, double time, bool reflection_only, int ignore_index)
{
    struct Event current_event = (struct Event){.pos = pos,.time = time};
    struct Vec3 amplitude = {.x = 0.0, .y = 0.0, .z = 0.0};
    struct Path path;
    move_path(current_event, ignore_index, &path, 0, &amplitude, gv_reflections,reflection_only );
    return amplitude;
}

double get_amplitude_abs(struct Vec3 pos, double time, bool reflection_only, int ignore_index)
{
    return magnitude(get_amplitude(pos, time, reflection_only,ignore_index));
}
struct Vec3 get_kinetic_energy(struct Vec3 pos, double time, bool reflection_only, int ignore_index)
{
    struct Vec3 amplitude_pre = get_amplitude(pos, time-gv_time_step, reflection_only,ignore_index);
    struct Vec3 amplitude = get_amplitude(pos, time, reflection_only,ignore_index);
    return v_add(amplitude , v_scalar( amplitude_pre ,-1.0 ));
}
double get_kinetic_energy_abs(struct Vec3 pos, double time, bool reflection_only, int ignore_index)
{
    return magnitude(get_kinetic_energy(pos, time, reflection_only,ignore_index));
}
struct Vec3 get_potential_energy(struct Vec3 pos, double time, bool reflection_only, int ignore_index)
{
    return v_square(get_amplitude(pos, time, reflection_only,ignore_index));
}
double get_potential_energy_abs(struct Vec3 pos, double time, bool reflection_only, int ignore_index)
{
    return magnitude(get_potential_energy(pos, time, reflection_only,ignore_index));
}
struct Vec3 get_energy(struct Vec3 pos, double time, bool reflection_only, int ignore_index)
{
    return v_add(get_potential_energy(pos, time, reflection_only,ignore_index),get_kinetic_energy(pos, time, reflection_only,ignore_index));
}
double get_energy_abs(struct Vec3 pos, double time, bool reflection_only, int ignore_index)
{
    return magnitude(get_energy(pos, time, reflection_only,ignore_index));
}