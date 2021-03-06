
#include "example_texture.hpp"


#ifdef INF443_EXAMPLE_TEXTURE

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
    // Create a surface with (u,v)-texture coordinates
    mesh surface_cpu;
    surface_cpu.position     = {{-1,-1,0}, { 1,-1,0}, { 1, 1,0}, {-1, 1,0}};
    surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
    surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

    surface = surface_cpu;

    // Load a texture image on GPU and stores its ID
    texture_id = texture_gpu( image_load_png("data/squirrel.png") );
}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();


    // Before displaying a textured surface: bind the associated texture id
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    surface.draw(shaders["mesh"], scene.camera);

    // After the surface is displayed it is safe to set the texture id to a white image
    //  Avoids to use the previous texture for another object
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);


    if(gui_scene.wireframe)
        surface.draw(shaders["wireframe"], scene.camera);

}




void scene_exercise::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}



#endif

