#include <stdbool.h>
#include <math.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>
#include "global_var.h"
#include "wave_math.h"
#include "wave_center_setup.h"
#include "radiation.h"
#include "wave_center.h"
#include "wave_map.h"
#include "wave_3d.h"
#include "time_control.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 1200;
    int screenHeight = 600;
    
    
    bool move_free = false;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); 
    InitWindow(screenWidth, screenHeight, "EWT");

    Camera camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
    //DisableCursor();  

    SetTargetFPS(30);             
    
    //create_wave_center((struct Vec3) {.x=0.0,.y=0.0,.z=0.0}, (struct Vec3) {.x=0.0,.y=0.0,.z=0.0});
    //create_cube(2, 1, 1, 0.3);//64
    //wave_center_focus = 0;
    if(show_wave_heat_map) wave_determine_line( X_AXIS);
    
    
    wave_init();
    init_global_var();
    //testing
    //light_cone_tester();
    //---------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (move_free)UpdateCamera(&camera, CAMERA_FREE);

        time_control_update();
        if(IsKeyPressed(KEY_SPACE))
        {
            move_free = !move_free;
            if(move_free)DisableCursor();
            else EnableCursor();
        }
        if((IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) && !move_free)
        {
            int index_found = -1;
            float distance_to_camera = 0.0f;
            Ray mouse_ray = GetMouseRay(GetMousePosition(), camera); 
            RayCollision collision; 
            for(int i = 0; i < wave_center_count; i++)
            {
                collision = GetRayCollisionSphere(mouse_ray, 
                (Vector3){wave_centers[i].pos.x,wave_centers[i].pos.y,wave_centers[i].pos.z}, wave_center_radius); 
                if(collision.hit)
                {
                    if(index_found < 0 || collision.distance < distance_to_camera)
                    {
                        index_found = i;
                        distance_to_camera = collision.distance;
                    }
                }    
            }
            if(index_found >= 0 && index_found != wave_center_focus )
            {
                wave_center_focus = index_found;
                heat_map_new_focus();
            }
            
        }
        if(IsKeyPressed(KEY_H))
        {
            
            show_wave_heat_map = !show_wave_heat_map;
            //if(show_wave_heat_map)wave_determine_line(X_AXIS);
        }
        if(IsKeyPressed(KEY_T))
        {
            time_step();
            // gv_global_time += gv_time_step;
            // gv_time_steps ++;
            // update_time();
            // heat_map_update_time();
            
            //wave_update();
        }
        if(IsKeyPressed(KEY_Z))
        {
            for(int i = 0; i <100; i++)
            {
            gv_global_time += gv_time_step;
            gv_time_steps ++;
            update_time();
            }
            if(show_wave_heat_map)wave_determine_line(X_AXIS);
            
            //wave_update();
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            BeginMode3D(camera);
            wave_draw3d();
            EndMode3D();

            DrawText("[SPACE] to toggle camera mode.", 400, 10, 20, DARKGRAY);
            wave_draw();
            draw_wave_center_setup();
            global_var_draw();
            draw_time_control();
           // 
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}