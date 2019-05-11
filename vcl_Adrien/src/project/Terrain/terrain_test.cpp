
#include "terrain_test.hpp"
#include <random>
#include <time.h>

#ifdef PROJECT_TERRAIN_TEST

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_project::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    // Create visual terrain surface
    terrain_drawable=terrain.terrain;
    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_project::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe

    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
//    glBindTexture(GL_TEXTURE_2D, terrain_texture_id);
    terrain_drawable.draw(shaders["mesh"], scene.camera);
    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        terrain_drawable.draw(shaders["wireframe"], scene.camera);
    }
    glBindTexture(GL_TEXTURE_2D,scene.texture_white);
    glDepthMask(true);
}

void scene_project::update_terrain()
{
    // Clear memory in case of pre-existing terrain
    terrain_drawable.data_gpu.clear();

    // Create visual terrain surface
    terrain_drawable = terrain.terrain;
}
void scene_project::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
    ImGui::Separator();
    ImGui::Text("Perlin parameters");

    float height_min = 0.1f;
    float height_max = 2.0f;
    if( ImGui::SliderScalar("Height", ImGuiDataType_Float, &gui_scene.height, &height_min, &height_max) )
        update_terrain();

    float scaling_min = 0.1f;
    float scaling_max = 10.0f;
    if( ImGui::SliderScalar("(u,v) Scaling", ImGuiDataType_Float, &gui_scene.scaling, &scaling_min, &scaling_max) )
        update_terrain();

    int octave_min = 1;
    int octave_max = 10;
    if( ImGui::SliderScalar("Octave", ImGuiDataType_S32, &gui_scene.octave, &octave_min, &octave_max) )
        update_terrain();

    float persistency_min = 0.1f;
    float persistency_max = 0.9f;
    if( ImGui::SliderScalar("Persistency", ImGuiDataType_Float, &gui_scene.persistency, &persistency_min, &persistency_max) )
        update_terrain();
}



#endif

