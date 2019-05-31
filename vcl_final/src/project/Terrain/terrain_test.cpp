
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
    terrain.init_drawable();
    // Create visual terrain surface
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
    glBindTexture(GL_TEXTURE_2D,scene.texture_white);
    terrain.terrain_drawable.draw(shaders["mesh"], scene.camera);
    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        terrain.terrain_drawable.draw(shaders["wireframe"], scene.camera);
    }
    glBindTexture(GL_TEXTURE_2D,scene.texture_white);
    glDepthMask(true);
}

void scene_project::update_terrain()
{
    // Clear memory in case of pre-existing terrain

    terrain.setPerlinMontagne(gui_scene.heightm,gui_scene.scalingm,gui_scene.octavem,gui_scene.persistencym);
    terrain.setPerlinTerrain(gui_scene.heightt,gui_scene.scalingt,gui_scene.octavet,gui_scene.persistencyt);
    terrain.setPerlinCote(gui_scene.heightc,gui_scene.scalingc,gui_scene.octavec,gui_scene.persistencyc);
    terrain.reload();
}
void scene_project::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
    ImGui::Separator();
    ImGui::Text("Perlin parameters");
    //Montagne
    float height_minm = 0.1f;
    float height_maxm = 2.0f;
    if( ImGui::SliderScalar("Height Montagne", ImGuiDataType_Float, &gui_scene.heightm, &height_minm, &height_maxm) )
        update_terrain();

    float scaling_minm = 0.1f;
    float scaling_maxm = 10.0f;
    if( ImGui::SliderScalar("(u,v) Scaling Montagne", ImGuiDataType_Float, &gui_scene.scalingm, &scaling_minm, &scaling_maxm) )
        update_terrain();

    int octave_minm = 1;
    int octave_maxm = 15;
    if( ImGui::SliderScalar("Octave Montagne", ImGuiDataType_S32, &gui_scene.octavem, &octave_minm, &octave_maxm) )
        update_terrain();

    float persistency_minm = 0.1f;
    float persistency_maxm = 0.9f;
    if( ImGui::SliderScalar("Persistency Montagne", ImGuiDataType_Float, &gui_scene.persistencym, &persistency_minm, &persistency_maxm) )
        update_terrain();
    //Terrain
    float height_mint = 0.05f;
    float height_maxt = 1.0f;
    if( ImGui::SliderScalar("Height Terrain", ImGuiDataType_Float, &gui_scene.heightt, &height_mint, &height_maxt) )
        update_terrain();

    float scaling_mint = 0.1f;
    float scaling_maxt = 10.0f;
    if( ImGui::SliderScalar("(u,v) Scaling Terrain", ImGuiDataType_Float, &gui_scene.scalingt, &scaling_mint, &scaling_maxt) )
        update_terrain();

    int octave_mint = 1;
    int octave_maxt = 15;
    if( ImGui::SliderScalar("Octave Terrain", ImGuiDataType_S32, &gui_scene.octavet, &octave_mint, &octave_maxt) )
        update_terrain();

    float persistency_mint = 0.1f;
    float persistency_maxt = 0.9f;
    if( ImGui::SliderScalar("Persistency Terrain", ImGuiDataType_Float, &gui_scene.persistencyt, &persistency_mint, &persistency_maxt) )
        update_terrain();

    //Cote
    float height_minc = 0.05f;
    float height_maxc = 1.0f;
    if( ImGui::SliderScalar("Height Cote", ImGuiDataType_Float, &gui_scene.heightc, &height_minc, &height_maxc) )
        update_terrain();

    float scaling_minc = 0.1f;
    float scaling_maxc = 10.0f;
    if( ImGui::SliderScalar("(u,v) Scaling Cote", ImGuiDataType_Float, &gui_scene.scalingc, &scaling_minc, &scaling_maxc) )
        update_terrain();

    int octave_minc = 1;
    int octave_maxc = 15;
    if( ImGui::SliderScalar("Octave Cote", ImGuiDataType_S32, &gui_scene.octavec, &octave_minc, &octave_maxc) )
        update_terrain();

    float persistency_minc = 0.1f;
    float persistency_maxc = 0.9f;
    if( ImGui::SliderScalar("Persistency Cote", ImGuiDataType_Float, &gui_scene.persistencyc, &persistency_minc, &persistency_maxc) )
        update_terrain();
}



#endif

