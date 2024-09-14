

bool wave_center_no_movement = false;
//double wave_center_radius = 1.0;

bool differential_reflections_only = false;



int wave_center_move(int wave_center_index)
{
    //question: exclude reflection off itself?
    //int reflection = (differential_reflections_only?1:0);
    //double delta_step = gv_delta_r;//*gv_c/gv_background_frequency;
    struct Vec3 position = wave_centers[wave_center_index].pos;
    struct Vec3 velocity = wave_centers[wave_center_index].velocity;
    //double amplitude = get_amplitude_abs(position);
    double amplitude_xp = get_amplitude_abs(v_add(position,(struct Vec3){.x = gv_delta_r, .y = 0.0, .z = 0.0}),
    gv_global_time - gv_time_step, differential_reflections_only,wave_center_count_max+1);//wave_center_index
    double amplitude_xn = get_amplitude_abs(v_add(position,(struct Vec3){.x = -gv_delta_r, .y = 0.0, .z = 0.0}),
    gv_global_time - gv_time_step, differential_reflections_only,wave_center_count_max+1);
    double amplitude_yp = get_amplitude_abs(v_add(position,(struct Vec3){.x = 0.0, .y = gv_delta_r, .z = 0.0}),
    gv_global_time - gv_time_step, differential_reflections_only,wave_center_count_max+1);
    double amplitude_yn = get_amplitude_abs(v_add(position,(struct Vec3){.x = 0.0, .y = -gv_delta_r, .z = 0.0}),
    gv_global_time - gv_time_step, differential_reflections_only,wave_center_count_max+1);
    double amplitude_zp = get_amplitude_abs(v_add(position,(struct Vec3){.x = 0.0, .y = 0.0, .z = gv_delta_r}),
    gv_global_time - gv_time_step, differential_reflections_only,wave_center_count_max+1);
    double amplitude_zn = get_amplitude_abs(v_add(position,(struct Vec3){.x = 0.0, .y = 0.0, .z = -gv_delta_r}),
    gv_global_time - gv_time_step, differential_reflections_only,wave_center_count_max+1);
    struct Vec3 diff_neg = (struct Vec3){.x = amplitude_xn*amplitude_xn-amplitude_xp*amplitude_xp, 
    .y = amplitude_yn*amplitude_yn-amplitude_yp*amplitude_yp, .z = amplitude_zn*amplitude_zn-amplitude_zp*amplitude_zp};
    //question: what is factor of diff? gv_amplitude_longitudinal to norm amplitude?
    diff_neg = v_scalar(diff_neg, gv_diff_factor*gv_time_step/gv_delta_r);
    //printf("diff %f %f %f \n",(amplitude_xn-amplitude_xp), (amplitude_yn-amplitude_yp), (amplitude_zn-amplitude_zp));
    //if(magnitude(diff_neg)>gv_c*gv_time_step)
    //    diff_neg = v_scalar(v_scalar(diff_neg, 1.0/magnitude(diff_neg)) , gv_c*gv_time_step  );
    velocity = v_scalar(velocity, gv_attenuation);
    wave_centers[wave_center_index].velocity = v_add(velocity, diff_neg);
    //wave_centers[wave_center_index].pos = v_add(position, diff_neg);
    wave_centers[wave_center_index].pos = v_add(position,v_scalar( wave_centers[wave_center_index].velocity,gv_time_step));
    
    return 0;
}

int wave_center_move_all()
{
    for(int i = 0; i < wave_center_count; i++)
    {
        wave_center_move(i);
    }
    return 0;
}

int reduce_events(int index)
{
    for (int i = 128; i <wave_centers[index].steps;i++)
    {
        wave_centers[index].events[i-128] = wave_centers[index].events[i];
    }
    wave_centers[index].steps -= 128;
    return 0;
}

int update_time()
{
    if(!wave_center_no_movement) wave_center_move_all();

    for (int i = 0; i < wave_center_count; i++)
    {
        if(wave_centers[i].steps >= 2048) reduce_events(i);
        wave_centers[i].events[wave_centers[i].steps].pos = wave_centers[i].pos;
        wave_centers[i].events[wave_centers[i].steps].time = gv_global_time;
        wave_centers[i].steps ++; 
    }
    return 0;
}


