
#include "eau_test.hpp"


#ifdef PROJECT_EAU

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_project::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
    eau.init_draw();
    // Create a reference ground as a horizontal quadrangle
    ground = mesh_primitive_quad({-1,-1,0},{-1,1,0},{1,1,0},{1,-1,0});
}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_project::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();

    // Display reference ground
//    ground.draw(shaders["mesh"], scene.camera);


    // Enable use of alpha component as color blending for transparent elements
    //  new color = previous color + (1-alpha) current color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);


    glBindTexture(GL_TEXTURE_2D, eau.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    eau.surface.draw(shaders["mesh"], scene.camera);
    if(gui_scene.wireframe)
        eau.surface.draw(shaders["wireframe"], scene.camera);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);

}




void scene_project::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}



#endif

