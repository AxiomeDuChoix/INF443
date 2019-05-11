
#include "terrain_exemple.hpp"
#include <random>
#include <time.h>

#ifdef PROJECT_TERRAIN_EXEMPLE

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;



float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v, float noise);
mesh create_terrain(const gui_scene_structure& gui_scene);
mesh create_cylinder(float radius, float height);
mesh create_cone(float radius, float height, float z_offset);
mesh create_tree_foliage(float radius, float height, float z_offset, float ecart);



void scene_project::update_tree_position(){
    std::uniform_real_distribution<float> distrib(0.0f,1.0f);
    std::default_random_engine generator(time(NULL));
    tree_position.clear();
    for(int i=0;i<nb_tree;i++){
        float u=distrib(generator);
        float v=distrib(generator);
        tree_position.push_back(evaluate_terrain(u,v,0.0f));
    }
}
void scene_project::update_grass_position(){
    std::uniform_real_distribution<float> distrib(0.0f,1.0f);
    std::default_random_engine generator(time(NULL)+1);
    grass_position.clear();
    for(int i=0;i<nb_grass;i++){
        float u=distrib(generator);
        float v=distrib(generator);
        grass_position.push_back(evaluate_terrain(u,v,0.0f));
    }
}

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_project::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    // Create visual terrain surface
    terrain = create_terrain(gui_scene);
    terrain.uniform_parameter.color = {0.6f,0.85f,0.5f};
    terrain.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material
    terrain_texture_id = texture_gpu( image_load_png("data/grass.png") );
    update_tree_position();
    //Create quads with (u,v)-texture coordinates
    mesh grass_surface_cpu;
    grass_surface_cpu.position     = {{-0.2f,0,0}, { 0.2f,0,0}, { 0.2f,0,0.4f}, {-0.2f,0,0.4f}};
    grass_surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
    grass_surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

    grass_surface = grass_surface_cpu;
    grass_surface.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture

    grass_texture_id = texture_gpu( image_load_png("data/billboard_grass.png") );
    update_grass_position();

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
    glBindTexture(GL_TEXTURE_2D, terrain_texture_id);
    terrain.draw(shaders["mesh"], scene.camera);
    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        terrain.draw(shaders["wireframe"], scene.camera);
    }
    glBindTexture(GL_TEXTURE_2D,scene.texture_white);
//    terrain.draw(shaders["wireframe"], scene.camera);
    mesh_drawable cylindre=create_cylinder(r_tronc,h_tronc);
    cylindre.uniform_parameter.color={0.88f,0.41f,0};
    mesh_drawable tree_foliage=create_tree_foliage(r_feuille,h_feuille,h_tronc,0.5*h_feuille);
    tree_foliage.uniform_parameter.color={0.10f,1,0.2f};
    for(int i=0,l=tree_position.size();i<l;i++){
        cylindre.uniform_parameter.translation=tree_position[i];
        cylindre.uniform_parameter.translation+={0,0,0};
        tree_foliage.uniform_parameter.translation=tree_position[i];
        tree_foliage.uniform_parameter.translation+={0,0,0};
        cylindre.draw(shaders["mesh"],scene.camera);
        tree_foliage.draw(shaders["mesh"],scene.camera);
    }

    //Draw grass
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);

    glBindTexture(GL_TEXTURE_2D, grass_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    const mat3 Identity = mat3::identity();
    const mat3 R = rotation_from_axis_angle_mat3({0,0,1}, 3.14f/2.0f); // orthogonal rotation

    for(int i=0,l=grass_position.size();i<l;i++){
        grass_surface.uniform_parameter.translation = grass_position[i];
        grass_surface.uniform_parameter.translation+={0,0,0.3f};
        grass_surface.uniform_parameter.rotation = Identity;
        grass_surface.draw(shaders["mesh"], scene.camera);
        if(gui_scene.wireframe)
            grass_surface.draw(shaders["wireframe"], scene.camera);

        grass_surface.uniform_parameter.rotation = R;
        grass_surface.draw(shaders["mesh"], scene.camera);
        if(gui_scene.wireframe)
            grass_surface.draw(shaders["wireframe"], scene.camera);
    }
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);



}



// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
{
    const int n=5;
    const vec2 pi[n] = {{0.1f, 0.3f},{0.4f,0.5f},{0.7f,0.3f},{0.1f, 0.9f},{0.3f, 0.8f}};
    const float hi[n] = {2.0f,3.0f,2.0f,1.5f,1.0f};
    const float sigmai[n] = {0.15f,0.1f,0.2f,0.1f,0.3f};

    float zincr = 0;
    for(int i=0;i<n;i++){

        const float d = norm(vec2(u,v)-pi[i])/sigmai[i];
        zincr += hi[i]*std::exp(-d*d);
    }
    const float z=zincr;
    return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v, float noise)
{


    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);
    const float z = evaluate_terrain_z(u,v)+noise;

    return {x,y,z};
}

// Generate terrain mesh
mesh create_terrain(const gui_scene_structure& gui_scene)
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);


            // get gui parameters
            const float scaling = gui_scene.scaling;
            const int octave = gui_scene.octave;
            const float persistency = gui_scene.persistency;
            const float height = gui_scene.height;

            // Evaluate Perlin noise
            const float noise = perlin(scaling*u, scaling*v, octave, persistency);

            // Compute coordinates
            terrain.position[kv+N*ku] = evaluate_terrain(u,v,noise*height);
            terrain.texture_uv.push_back({ku%2,kv%2});
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for(unsigned int ku=0; ku<Ns-1; ++ku)
    {
        for(unsigned int kv=0; kv<Ns-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const index3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const index3 triangle_2 = {idx, idx+Ns, idx+1+Ns};
            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }

    }

    return terrain;
}

void scene_project::update_terrain()
{
    // Clear memory in case of pre-existing terrain
    terrain.data_gpu.clear();

    // Create visual terrain surface
    terrain = create_terrain(gui_scene);
    terrain.uniform_parameter.color = {1.0f, 1.0f, 1.0f};
    terrain.uniform_parameter.shading.specular = 0.0f;
}

mesh create_cylinder(float radius, float height){
    const size_t N=50;
    mesh cylindre;
    cylindre.position.resize(2*N);
    for(size_t ku=0; ku<N; ++ku)
    {
        const float u=radius*std::cos(2*3.1416*ku/(N));
        const float v=radius*std::sin(2*3.1416*ku/(N));
        cylindre.position[2*ku]={u,v,0};
        cylindre.position[2*ku+1]={u,v,height};
    }
    for(size_t ku=0; ku<N; ++ku)
    {
        const unsigned int idx = 2*ku; // current vertex offset
        const index3 triangle_1 = {idx, (idx+1), (idx+2)%(2*N)};
        const index3 triangle_2 = {(idx+1), (idx+2)%(2*N), (idx+3)%(2*N)};
        cylindre.connectivity.push_back(triangle_1);
        cylindre.connectivity.push_back(triangle_2);
    }
    return cylindre;
}



mesh create_cone(float radius, float height, float z_offset){
    const int N=50;
    mesh cone;
    cone.position.resize(N+2);
    for(int i=0;i<N;i++){
        const float u=radius*std::cos(2*3.1416*i/(N));
        const float v=radius*std::sin(2*3.1416*i/(N));
        cone.position[i]={u,v,z_offset};
    }
    cone.position[N]={0,0,z_offset};
    cone.position[N+1]={0,0,height+z_offset};
    for(int i=0;i<N;i++){
        const index3 triangle_1 = {i, (i+1)%N, N};
        const index3 triangle_2 = {i, (i+1)%N, N+1};
        cone.connectivity.push_back(triangle_1);
        cone.connectivity.push_back(triangle_2);
    }
    return cone;
}


mesh create_tree_foliage(float radius, float height, float z_offset,float ecart)
{
    mesh m = create_cone(radius, height, z_offset);
    m.push_back( create_cone(radius, height, z_offset+ecart) );
    m.push_back( create_cone(radius, height, z_offset+2*ecart) );

    return m;
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

