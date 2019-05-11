#pragma once

#include "../base_project/base_project.hpp"
#include "terrain.hpp"
#ifdef PROJECT_TERRAIN_TEST

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe;

    float height = 0.2f;
    float scaling = 3.0f;
    int octave = 9;
    float persistency = 0.7f;
};

struct scene_project : base_scene_project
{

    /** A part must define two functions that are called from the main function:
     * setup_data: called once to setup data before starting the animation loop
     * frame_draw: called at every displayed frame within the animation loop
     *
     * These two functions receive the following parameters
     * - shaders: A set of shaders.
     * - scene: Contains general common object to define the 3D scene. Contains in particular the camera.
     * - data: The part-specific data structure defined previously
     * - gui: The GUI structure allowing to create/display buttons to interact with the scene.
    */
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void set_gui();

    // visual representation of a surface
    Terrain terrain;
    vcl::mesh_drawable terrain_drawable;
    gui_scene_structure gui_scene;
    void update_terrain();
};

#endif


