enum Axis {
    X_AXIS,
    Y_AXIS,
    Z_AXIS,
};
enum Axis_plane {
    XY_AXIS,
    XZ_AXIS,
    YZ_AXIS,
};
struct Vec3
{
    double x;
    double y;
    double z;
};
struct Event
{
    struct Vec3 pos;
    double time;
};
struct Wave_center
{
    struct Vec3 pos;
    struct Vec3 velocity;
    struct Event events[2048];
    int steps;
};
struct Path
{
    struct Event to;
    struct Event from;
    double r[8];
};

struct Vec3 v_add(struct Vec3 v1, struct Vec3 v2)
{
    return (struct Vec3){.x = v1.x+v2.x, .y = v1.y+v2.y, .z = v1.z+v2.z};
}
struct Vec3 v_scalar(struct Vec3 v, double scalar)
{
    return (struct Vec3){.x = scalar*v.x, .y = scalar*v.y, .z = scalar*v.z};
}
struct Vec3 v_square(struct Vec3 v)
{
    return (struct Vec3){.x = v.x*v.x, .y = v.y*v.y, .z = v.z*v.z};
}
struct Event event_plus_time(struct Event e, double t)
{
    return (struct Event){.pos = e.pos, .time = e.time + t};
}
double distance(struct Vec3 v1, struct Vec3 v2)
{
    return sqrt( (v1.x-v2.x)*(v1.x-v2.x) + (v1.y-v2.y)*(v1.y-v2.y) +(v1.z-v2.z)*(v1.z-v2.z));
}
double magnitude(struct Vec3 v)
{
    return sqrt( v.x*v.x + v.y*v.y + v.z*v.z);
}
double v_sum(struct Vec3 v)
{
    return (v.x + v.y + v.z);
}
double distance_events(struct Event e1, struct Event e2)
{
    return distance(e1.pos, e2.pos);
}

struct Vec3 normalized_direction(struct Vec3 from, struct Vec3 to)
{
    double amplitude_inv = 1.0/distance(from,to);
    return (struct Vec3){.x = amplitude_inv*(to.x-from.x), .y = amplitude_inv*(to.y-from.y), .z = amplitude_inv*(to.z-from.z)};
}
struct Vec3 normalized_direction_events(struct Event from, struct Event to)
{
    return normalized_direction(from.pos, to.pos);
}
//speed assuming cause -> effect. When speed = c then effect exact on light cone of cause. 
//For some impossibilities, outside ligtht cone, 2c returned to avoid singularity and negative values.
double speed(struct Event cause,struct Event effect)
{
    double distance = distance_events(cause, effect);
    double time = (effect.time - cause.time);
    if(distance == 0.0 && time == 0.0) return gv_c;
    if(time <= 0.0) return 2.0 * gv_c;
    if(distance/(2.0*gv_c) > time) return 2.0 * gv_c;
    return distance / time;
}
//find the cause event of wave_center to the effect within light cone
struct Event find_light_cone_event(struct Event effect, struct Wave_center* wave_center)
{
    struct Event none = {.time = -1.0};//time -1 for event means none found. To be checked after a call.
    int max_steps_index= wave_center->steps-1;
    if( max_steps_index == 0) return none; //no trajectory
    if(speed(wave_center->events[0],  effect) > gv_c) return none; //too far away to begin with
    //first guess go to index of same time, then calculate distance. Then go back again by time=distance/c. 
    int time_steps_diff = (int)((wave_center->events[max_steps_index].time - effect.time)/gv_time_step);
    int index_estimated_time = max_steps_index - time_steps_diff;//estimation of index same time
    if(index_estimated_time<0)index_estimated_time = 0;
    if(index_estimated_time>max_steps_index)index_estimated_time = max_steps_index;
    index_estimated_time -= 
    (int)((distance_events (wave_center->events[index_estimated_time], effect)/gv_c)/gv_time_step);
    if(index_estimated_time<0)index_estimated_time = 0;
    //if(index_estimated_time>max_steps_index)index_estimated_time = max_steps_index; //shouldnt be possible
    //first guess end, start algorithm
    int late_estimation_index = index_estimated_time;
    int early_estimation_index = index_estimated_time;
    while(late_estimation_index >= 0 && speed(wave_center->events[late_estimation_index],effect) > gv_c)
    {late_estimation_index--;}
    while(late_estimation_index < max_steps_index &&
    speed(wave_center->events[late_estimation_index],effect) < gv_c)
    {late_estimation_index++;}
     
    if(late_estimation_index == 0) return wave_center->events[0];
    early_estimation_index = late_estimation_index -1;
    
    struct Event e_early = wave_center->events[early_estimation_index];
    double distance_early = distance_events(e_early, effect);
    double time_diff_early = effect.time - e_early.time ;
    struct Event e_late = wave_center->events[late_estimation_index];
    double distance_late = distance_events(e_late, effect);
    double time_diff_late = effect.time - e_late.time ;
    double first_event_diff_time = (distance_early*time_diff_late - distance_late*time_diff_early)/
    (-gv_c*time_diff_early + gv_c*time_diff_late + distance_early - distance_late);
    double first_event_time = effect.time - first_event_diff_time;
    //lerp
    if(first_event_time < e_early.time || first_event_time > e_late.time)return none; //shouldnt happen
    double lerp = (first_event_time - e_early.time) / (e_late.time - e_early.time);
    struct Event result;
    result.time = first_event_time;
    result.pos.x = e_early.pos.x + lerp*(e_late.pos.x - e_early.pos.x);
    result.pos.y = e_early.pos.y + lerp*(e_late.pos.y - e_early.pos.y);
    result.pos.z = e_early.pos.z + lerp*(e_late.pos.z - e_early.pos.z);
    return result;
}

