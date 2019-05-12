
#include "01_modelisation.hpp"
#include <random>

#ifdef INF443_01_MODELISATION

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;




float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
mesh create_terrain();
mesh create_cylinder(float radius, float height);
mesh create_tree_foliage(float radius, float height, float z_offset);


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{

     // Create a quad with (u,v)-texture coordinates (billboards)
    mesh surface_cpu;
    surface_cpu.position     = {{0,-0.2f,0}, { 0,0.2f,0}, { 0, 0.2f,0.4f}, {0,-0.2f,0.4f}};
    surface_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
    surface_cpu.connectivity = {{0,1,2}, {0,2,3}};

    surface = surface_cpu;
    surface.uniform_parameter.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture

    // Create visual terrain surface
    terrain = create_terrain();
    terrain.uniform_parameter.color = {0.6f,0.85f,0.5f};
    terrain.uniform_parameter.shading.specular = 0.0f; // non-specular terrain material
    // Load a texture image on GPU and stores its ID
    texture_id = texture_gpu( image_load_png("data/grass.png") );
    texture_billboard = texture_gpu( image_load_png("data/billboard_grass.png") );
    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    // Setup the tree positions:

    update_tree_position();

}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe


    //Modifs
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // Display terrain
    glPolygonOffset( 1.0, 1.0 );
    terrain.draw(shaders["mesh"], scene.camera);
    // After the surface is displayed it is safe to set the texture id to a white image
    //  Avoids to use the previous texture for another object
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        terrain.draw(shaders["wireframe"], scene.camera);
    }


    // display trees

    mesh_drawable cylinder;
    mesh_drawable foliage;
    for (int i = 0;i<15;i++){
        cylinder = mesh_drawable(create_cylinder(0.3f,2.0f));
        cylinder.uniform_parameter.translation = scene_exercise::tree_position[i]+vec3(0.0f,0.0f,-0.2f);
        cylinder.uniform_parameter.color = {0.3f, 0.0f, 0.0f};
        cylinder.draw(shaders["mesh"], scene.camera);



        foliage = mesh_drawable(create_tree_foliage(1.0f, 1.0f, 0.8f));
        foliage.uniform_parameter.translation = scene_exercise::tree_position[i]+vec3(0.0f,0.0f,1.8);
        foliage.uniform_parameter.color = {0.0f, 1.0f, 0.0f};   
        foliage.draw(shaders["mesh"], scene.camera);
    }



    const mat3 Identity = mat3::identity();
    const mat3 R = rotation_from_axis_angle_mat3({0,0,1}, 3.14f/2.0f); // orthogonal rotation

    // Enable use of alpha component as color blending for transparent elements
    //  new color = previous color + (1-alpha) current color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);


    glBindTexture(GL_TEXTURE_2D, texture_billboard);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts

    for (int i = 0;i<15;i++){
    // Display two orthogonal billboards with static orientation
    // ********************************************************** //

        surface.uniform_parameter.translation = scene_exercise::tree_position[15+i];
        surface.uniform_parameter.rotation = Identity;
        surface.draw(shaders["mesh"], scene.camera);
        if(gui_scene.wireframe)
            surface.draw(shaders["wireframe"], scene.camera);

        surface.uniform_parameter.rotation = R;
        surface.draw(shaders["mesh"], scene.camera);
        if(gui_scene.wireframe)
            surface.draw(shaders["wireframe"], scene.camera);


    }
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);

}



// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
{
    const vec2 u1 = {0.0f, 0.0f};
    const float h1 = 3.0f;
    const float sigma1 = 0.5f;
    const float d1 = norm(vec2(u,v)-u1)/sigma1;

    const vec2 u2 = {0.5f, 0.5f};
    const float h2 = -1.5f;
    const float sigma2 = 0.15f;
    const float d2 = norm(vec2(u,v)-u2)/sigma2;

    const vec2 u3 = {0.2f, 0.7f};
    const float h3 = 1.0f;
    const float sigma3 = 0.2f;
    const float d3 = norm(vec2(u,v)-u3)/sigma3;

    const vec2 u4 = {0.8f, 0.7f};
    const float h4 = 2.0f;
    const float sigma4 = 0.2f;
    const float d4 = norm(vec2(u,v)-u4)/sigma4;
    const float z = h1*std::exp(-d1*d1)+h2*std::exp(-d2*d2)+h3*std::exp(-d3*d3)+h4*std::exp(-d4*d4);

    return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
    float height = 0.6f;
    float scaling = 3.0f;
    int octave = 7;
    float persistency = 0.4f;
    // Evaluate Perlin noise
    const float noise = perlin(scaling*u, scaling*v, octave, persistency);
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);
    const float z = evaluate_terrain_z(u,v)*noise;

    return {x,y,z};
}

// Generate terrain mesh
mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.texture_uv.resize(N*N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku)
    {
        for(size_t kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute coordinates
            terrain.position[kv+N*ku] = evaluate_terrain(u,v);
            terrain.texture_uv[kv+N*ku] = {u,v};
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


mesh create_cylinder(float radius, float height){

    mesh m;
    //number of samples
    const size_t N = 20;

    // Geometry
    for (size_t k=0;k<N;k++){
        const float u = k/float(N);
        const vec3 p = {radius*std::cos(2*3.141592f*u),radius*std::sin(2*3.141592f*u),0.0f};
        m.position.push_back(p);
        m.position.push_back(p+vec3(0,0,height));
    }
    // Connectivity
    for (size_t k = 0; k<N;k++){
        const unsigned int u0 = 2*k;
        const unsigned int u1 = (2*k+1)%(2*N);
        const unsigned int u2 = (2*k+2)%(2*N);
        const unsigned int u3 = (2*k+3)%(2*N);       
        const index3 triangle1= {u0,u1,u2};
        const index3 triangle2 = {u1,u2,u3};
        m.connectivity.push_back(triangle1);
        m.connectivity.push_back(triangle2);
    }
    return m;
}
// mesh create_cone(float radius, float height, float z_offset){
//      mesh m;
//     //number of samples
//     const size_t N = 20;
//     const vec3 zero = {0.0f,0.0f,0.0f};
//     const vec3 top = {0.0f,0.0f,height};
//     // Geometry
//     m.position.push_back(zero+vec3(0,0,z_offset));
//     m.position.push_back(top+vec3(0,0,z_offset));
//     for (size_t k=0;k<N+1;k++){
//         const float u = k/float(N);
//         const vec3 p = {radius*std::cos(2*3.141592f*u),radius*std::sin(2*3.141592f*u),0.0f};
//         m.position.push_back(p+vec3(0,0,z_offset));
//     }

//     // Connectivity
//     for (size_t k = 0; k<N+1;k++){
//         const unsigned int u0 = k+2;
//         const unsigned int u1 = (k+1>=N)? (k+1) : ((k+1+2)%(N+1));
//         const index3 triangle1= {0,u0,u1};
//         const index3 triangle2 = {1,u0,u1};
//         m.connectivity.push_back(triangle1);
//         m.connectivity.push_back(triangle2);
//     }
//     return m;
// }
mesh create_cone(float radius, float height, float z_offset)
{
    mesh m;
    // conical structure
    // *************************** //
    unsigned int temp;
    const size_t N = 20;
    // geometry
    for(size_t k=0; k<N; ++k)
    {
        const float u = k/float(N);
        const vec3 p = {radius*std::cos(2*3.14f*u), radius*std::sin(2*3.14f*u), 0.0f};
        m.position.push_back( p+vec3{0,0,z_offset} );
    }
    // apex
    m.position.push_back({0,0,height+z_offset});
    // connectivity
    const unsigned int Ns = N;
    for(unsigned int k=0; k<Ns; ++k) {
        temp = (k+1)%N;
        m.connectivity.push_back( {k , temp, Ns} );
    }
    // close the part in the bottom of the cone
    // *************************** //
    // Geometry
    for(size_t k=0; k<N; ++k)
    {
        const float u = k/float(N);
        const vec3 p = {radius*std::cos(2*3.14f*u), radius*std::sin(2*3.14f*u), 0.0f};
        m.position.push_back( p+vec3{0,0,z_offset} );
    }
    // central position
    m.position.push_back( {0,0,z_offset} );

    // connectivity
    for(unsigned int k=0; k<Ns; ++k){
        temp = (k+1)%Ns+N+1;
        m.connectivity.push_back( {k+Ns+1, temp, 2*Ns+1} );
    }

    return m;
}
mesh create_tree_foliage(float radius, float height, float z_offset)
{
    mesh m = create_cone(radius, height, 0);
    m.push_back(create_cone(radius, height, z_offset) );
    m.push_back(create_cone(radius, height, 2*z_offset) );

    return m;
}


void scene_exercise::update_tree_position(){
    // Uniform distribution in [0,1]
    float x,y;
    vec3 p;
    tree_position.clear();
    std::uniform_real_distribution<float> distrib(0.0,1.0);
    std::default_random_engine generator;
    int i = 0;
    bool boolean = true;
    while (i<30){
        x = distrib(generator);
        y = distrib(generator);
        p = evaluate_terrain(x,y);
        boolean = true;
        for (int j = 0; j<i;j++){
            if (norm(tree_position[j]-p)<2.5f){
                boolean = false;
                break;
            }
        }
        if (boolean){
            tree_position.push_back(p);
            i++;
        }
    }

}




void scene_exercise::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}



#endif



