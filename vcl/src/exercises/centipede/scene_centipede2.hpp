#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"
#include "centipedeclass.hpp"

#ifdef INF443_SCENE_CENTIPEDE2

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe = false;
    bool texture   = false;

    bool trajectory = false;

    float time_scale = 1.0f;
};

struct trajectory_structure
{
    std::vector<vcl::vec3> position;
    std::vector<float> time;
};

struct scene_exercise : base_scene_exercise
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

    void mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods);
    void mouse_move(scene_structure& scene, GLFWwindow* window);

    void set_gui();

    vcl::mesh terrain_mesh;
    trajectory_structure trajectory;
    void update_trajectory();
    void update_time_trajectory();
    void display_trajectory(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    int picked_object;
    vcl::curve_dynamic_drawable drawable_trajectory;    

    // Draw the trajectory of the moving point as a curve
    vcl::segment_drawable_immediate_mode segment_drawer;
    vcl::mesh_drawable sphere_trajectory;

    vcl::mesh_drawable_hierarchy centipede;
    void display_centipede(vcl::mesh_drawable_hierarchy* hiera, std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void display_centipede(Centipede* centi, std::map<std::string,GLuint>& shaders, scene_structure& scene);

    gui_scene_structure gui_scene;


    vcl::timer_interval timer;
    Centipede* testhierarchy;

};

#endif