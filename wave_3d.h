
int wave_draw3d()
{
    DrawGrid(100, 10.0f);
    for(int i = 0; i < wave_center_count; i++)
    {
        if(i == wave_center_focus)
            DrawSphere((Vector3){wave_centers[i].pos.x,wave_centers[i].pos.y,wave_centers[i].pos.z}, wave_center_radius, RED);
        else
            DrawSphere((Vector3){wave_centers[i].pos.x,wave_centers[i].pos.y,wave_centers[i].pos.z}, wave_center_radius, BLACK); 
    }
    return 0;
}