#pragma once

#include "../../base_project/base_project.hpp"

#ifdef PROJECT_CENTIPEDE


struct gui_scene_structure
{
    bool wireframe   = false;
};

struct scene_project : base_scene_project
{

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    void set_gui();

    vcl::mesh_drawable_hierarchy hierarchy;

    vcl::mesh_drawable ground;

    gui_scene_structure gui_scene;
    vcl::timer_interval timer;
};

#endif


