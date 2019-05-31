#include "scene_centipede2.hpp"

#include <random>

#ifdef INF443_SCENE_CENTIPEDE2


using namespace vcl;

// Generator for uniform random number
std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0,1.0);

vec3 gradient_du_terrain(float u, float v);
float evaluate_terrain_z_sans_bruit(float u, float v);
float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
mesh create_terrain();
mesh create_pot(float radius1, float radius2, float height, float offset);
mesh create_abdomen_elementaire(float r);
mesh create_abdomen_elementaire(int nb_pots,float r, float l_central);
mesh patte(float r, float l);
mesh create_cylinder(float radius, float height);

mesh_drawable_hierarchy create_centipede();

size_t index_at_value(float t, const std::vector<float>& vt);
vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3);
vec3 cardinal_spline_interpolation(const trajectory_structure& trajectory, float t);
vec3 cardinal_spline_derivative_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3);
vec3 cardinal_spline_derivative_interpolation(const trajectory_structure& trajectory, float t);

vcl::mesh create_skybox();


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{



    // setup initial position of the camera
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    terrain_mesh = create_terrain();
    update_trajectory();
    timer.t = trajectory.time[1];
    timer.t_min = trajectory.time[1];
    timer.t_max = trajectory.time[trajectory.time.size()-2];
    sphere_trajectory = mesh_primitive_sphere();

    centipede = create_centipede();
    testhierarchy = new Centipede();

    segment_drawer.init();

    drawable_trajectory = curve_dynamic_drawable(100); // number of steps stroed in the trajectory
    drawable_trajectory.uniform_parameter.color = {0,0,1};
}

/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.scale = gui_scene.time_scale;
    timer.update();

    set_gui();
    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe

    display_centipede(&centipede, shaders, scene);
    display_trajectory(shaders,scene);
    // mesh_drawable test= create_abdomen_elementaire(9,1,0.2);
    // mesh_drawable test= create_abdomen_elementaire(9,0.8,0.5);

    // test.draw(shaders["mesh"], scene.camera);

    // testhierarchy->centipede.draw(shaders["mesh"], scene.camera);
}
void scene_exercise::display_centipede(mesh_drawable_hierarchy* hiera, std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    const float t = timer.t;
    const vec3 p = cardinal_spline_interpolation(trajectory, t);
    if (gui_scene.trajectory){
        drawable_trajectory.add_point(p);
        drawable_trajectory.draw(shaders["curve"],scene.camera);
    }
    const vec3 d = normalize(cardinal_spline_derivative_interpolation(trajectory, t));
    vec3 dprev = d;


    mat3 R = rotation_between_vector_mat3({1,0,0},d);
    mat3 inv;

    // up vector
    const vec3 up = {0,0,1};
    const vec3 up_proj = up-dot(up,d)*d;
    const vec3 new_up = R*vec3{0,0,1};

    const mat3 twist = rotation_between_vector_mat3(new_up,up_proj);
    R = twist*R;

    std::string abdo = "abdomen", string_patte = "patte";

    hiera->draw(shaders["mesh"], scene.camera);
    // hiera->update_hierarchy();
    if(gui_scene.wireframe)
        hiera->draw(shaders["wireframe"], scene.camera);
    for (int k = 0; k<10;k++){
        hiera->rotation(string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(1))= rotation_from_axis_angle_mat3({0,1,0},-std::sin(20*3.1415f*(t-0.4f)))*rotation_from_axis_angle_mat3({0,0,1},3.1415f/3)*rotation_from_axis_angle_mat3({0,1,0},3.1415f/2);
        hiera->rotation(string_patte+"droite"+std::to_string(k+1)+","+std::to_string(1))= rotation_from_axis_angle_mat3({0,1,0},-std::sin(20*3.1415f*(t-0.4f)+3.1415f/2))*rotation_from_axis_angle_mat3({0,0,1},-3.1415f/3)*rotation_from_axis_angle_mat3({0,1,0},3.1415f/2);
    }
    for (int k = 0; k<10;k++){
        for(int i = 1; i<10;i++){
            hiera->rotation(string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(i+1))= rotation_from_axis_angle_mat3({0,1,0},3.1415f/40);
            hiera->rotation(string_patte+"droite"+std::to_string(k+1)+","+std::to_string(i+1))= rotation_from_axis_angle_mat3({0,1,0},-3.1415f/40);
        }
    }
    vec3 grad_prev, grad, translation_global, d2, newpoint;
    hiera->translation("head") = p;
    grad_prev = gradient_du_terrain((p.x)/20+0.5f,(p.y)/20+0.5f);
    hiera->rotation("head") = rotation_between_vector_mat3(up,grad_prev)*rotation_from_axis_angle_mat3(cross(d,up_proj),-3.1415f/2)*rotation_from_axis_angle_mat3({0,0,1},3.1415f/2)*R;
    
    // vec3 normal, local_up = vec3(0,1,0);
    // cylindre de débogage
    // mesh_drawable cylinder;
    // float time = t-0.2*0.23f/norm(cardinal_spline_derivative_interpolation(trajectory, t));
    // if (time<timer.t_min){
    //     time+= timer.t_max-timer.t_min;
    // }
    // newpoint = cardinal_spline_interpolation(trajectory, time);
    // d2 = normalize(cardinal_spline_derivative_interpolation(trajectory, time));
    // float signe = 1;
    // if( norm(d-d2)<1e-4f )
    //     supermatrice= mat3::identity();
    // else{
    //     const float prod = dot(d,d2);
    //     const float det = dot(cross(d,d2),up);
    //     signe = (det<0)? -1 : 1;
    //     const float angle = std::acos(prod);
    //     supermatrice = rotation_from_axis_angle_mat3(vec3(0,1,0),signe*angle/10);
    // }

    float time = t;
    // mesh_drawable cylinder, cylinder2; 
    // vec3 v3;
    for (int i = 0; i<10; i++){
        translation_global = hiera->get_translation_global(abdo+std::to_string(i+1));
        grad = gradient_du_terrain((translation_global.x)/20+0.5f,(translation_global.y)/20+0.5f);
        // normale au (i+1-ième abdomen) normal = hiera->get_rotation_global(abdo+std::to_string(i+1))*local_up;
        // (i+1)-ième cylindre de débogage: 
        // cylinder = create_cylinder(0.01,norm(d2));
        // cylinder.uniform_parameter.translation=newpoint;
        // cylinder.uniform_parameter.rotation = rotation_between_vector_mat3(up,d2); //rotation_between_vector_mat3(normal,grad)*
        // cylinder.uniform_parameter.color = vec3(0,1,0);
        // cylinder.draw(shaders["mesh"],scene.camera);
        if (det(hiera->get_rotation_global(abdo+std::to_string(i+1)))>1e-5){
            inv = inverse(hiera->get_rotation_global(abdo+std::to_string(i+1)));
        }
        time = time-0.2*0.23f/norm(cardinal_spline_derivative_interpolation(trajectory, time));
        d2 = cardinal_spline_derivative_interpolation(trajectory, time);
        if (time<timer.t_min){
            time+= timer.t_max-timer.t_min;
        }
        // éventuellement utile pour la démo: 

        // cylinder = create_cylinder(0.01,norm(d2));
        // cylinder.uniform_parameter.translation=translation_global;
        // cylinder.uniform_parameter.rotation = rotation_between_vector_mat3(up,inv*d2); //rotation_between_vector_mat3(normal,grad)*
        // cylinder.uniform_parameter.color = vec3(0,1,0);
        // cylinder.draw(shaders["mesh"],scene.camera);

        // cylinder2 = create_cylinder(0.01,norm(d2));
        // cylinder2.uniform_parameter.translation=translation_global;
        // v3 = hiera->get_rotation_global(abdo+std::to_string(i+1))*up;
        // cylinder2.uniform_parameter.rotation = rotation_between_vector_mat3(up,v3); //rotation_between_vector_mat3(normal,grad)*
        // cylinder2.uniform_parameter.color = vec3(0,0,1);
        // cylinder2.draw(shaders["mesh"],scene.camera);
        hiera->rotation(abdo+std::to_string(i+1))= rotation_between_vector_mat3(inv*dprev,inv*d2)*rotation_between_vector_mat3(inv*grad_prev,inv*grad);
        // std::cout<<rotation_between_vector_mat3(v3,d2)<<std::endl;
        // hiera->rotation(abdo+std::to_string(i+1))=/*rotation_from_axis_angle_mat3({0,1,0},-signe*std::sin(3.1415f*(t-0.4f)+i*3.1415/4)/10)*/supermatrice*rotation_between_vector_mat3(inv*grad_prev,inv*grad);
        // hiera->rotation(abdo+std::to_string(i+1))=rotation_from_axis_angle_mat3({0,1,0},std::sin(2*3.1415f*(t-0.4f)+i*3.1415/4)/7)*rotation_between_vector_mat3(inv*grad_prev,inv*grad);
        grad_prev = grad;
        dprev = d2;
    }
}
void scene_exercise::display_centipede(Centipede* centi, std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    const float t = timer.t;
    const vec3 p = cardinal_spline_interpolation(trajectory, t);
    if (gui_scene.trajectory){
        drawable_trajectory.add_point(p);
        drawable_trajectory.draw(shaders["curve"],scene.camera);
    }
    const vec3 d = normalize(cardinal_spline_derivative_interpolation(trajectory, t));
    vec3 dprev = d;


    mat3 R = rotation_between_vector_mat3({1,0,0},d);
    mat3 inv;

    // up vector
    const vec3 up = {0,0,1};
    const vec3 up_proj = up-dot(up,d)*d;
    const vec3 new_up = R*vec3{0,0,1};

    const mat3 twist = rotation_between_vector_mat3(new_up,up_proj);
    R = twist*R;

    std::string abdo = "abdomen", string_patte = "patte";

    centi->centipede.draw(shaders["mesh"], scene.camera);
    // centi->centipede.update_hierarchy();
    if(gui_scene.wireframe)
        centi->centipede.draw(shaders["wireframe"], scene.camera);
    for (int k = 0; k<centi->n_abdomen;k++){
        centi->centipede.rotation(string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(1))= rotation_from_axis_angle_mat3({0,1,0},-std::sin(20*3.1415f*(t-0.4f)))*rotation_from_axis_angle_mat3({0,0,1},3.1415f/3)*rotation_from_axis_angle_mat3({0,1,0},3.1415f/2);
        centi->centipede.rotation(string_patte+"droite"+std::to_string(k+1)+","+std::to_string(1))= rotation_from_axis_angle_mat3({0,1,0},-std::sin(20*3.1415f*(t-0.4f)+3.1415f/2))*rotation_from_axis_angle_mat3({0,0,1},-3.1415f/3)*rotation_from_axis_angle_mat3({0,1,0},3.1415f/2);
    }
    for (int k = 0; k<centi->n_abdomen;k++){
        for(int i = 1; i<centi->n_sous_pattes;i++){
            centi->centipede.rotation(string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(i+1))= rotation_from_axis_angle_mat3({0,1,0},3.1415f/40);
            centi->centipede.rotation(string_patte+"droite"+std::to_string(k+1)+","+std::to_string(i+1))= rotation_from_axis_angle_mat3({0,1,0},-3.1415f/40);
        }
    }
    vec3 grad_prev, grad, translation_global, d2, newpoint;
    centi->centipede.translation("head") = p;
    grad_prev = gradient_du_terrain((p.x)/20+0.5f,(p.y)/20+0.5f);
    centi->centipede.rotation("head") = rotation_between_vector_mat3(up,grad_prev)*rotation_from_axis_angle_mat3(cross(d,up_proj),-3.1415f/2)*rotation_from_axis_angle_mat3({0,0,1},3.1415f/2)*R;

    float time = t;
    // mesh_drawable cylinder, cylinder2; 
    // vec3 v3;
    for (int i = 0; i<centi->n_abdomen; i++){
        translation_global = centi->centipede.get_translation_global(abdo+std::to_string(i+1));
        grad = gradient_du_terrain((translation_global.x)/20+0.5f,(translation_global.y)/20+0.5f);
        // normale au (i+1-ième abdomen) normal = centi->centipede.get_rotation_global(abdo+std::to_string(i+1))*local_up;
        // (i+1)-ième cylindre de débogage: 
        // cylinder = create_cylinder(0.01,norm(d2));
        // cylinder.uniform_parameter.translation=newpoint;
        // cylinder.uniform_parameter.rotation = rotation_between_vector_mat3(up,d2); //rotation_between_vector_mat3(normal,grad)*
        // cylinder.uniform_parameter.color = vec3(0,1,0);
        // cylinder.draw(shaders["mesh"],scene.camera);
        if (det(centi->centipede.get_rotation_global(abdo+std::to_string(i+1)))>1e-5){
            inv = inverse(centi->centipede.get_rotation_global(abdo+std::to_string(i+1)));
        }
        time = time-centi->scaling*centi->l_tot_abdomen/norm(cardinal_spline_derivative_interpolation(trajectory, time));
        d2 = cardinal_spline_derivative_interpolation(trajectory, time);
        if (time<timer.t_min){
            time+= timer.t_max-timer.t_min;
        }
        // éventuellement utile pour la démo: 

        // cylinder = create_cylinder(0.01,norm(d2));
        // cylinder.uniform_parameter.translation=translation_global;
        // cylinder.uniform_parameter.rotation = rotation_between_vector_mat3(up,inv*d2); //rotation_between_vector_mat3(normal,grad)*
        // cylinder.uniform_parameter.color = vec3(0,1,0);
        // cylinder.draw(shaders["mesh"],scene.camera);

        // cylinder2 = create_cylinder(0.01,norm(d2));
        // cylinder2.uniform_parameter.translation=translation_global;
        // v3 = centi->centipede.get_rotation_global(abdo+std::to_string(i+1))*up;
        // cylinder2.uniform_parameter.rotation = rotation_between_vector_mat3(up,v3); //rotation_between_vector_mat3(normal,grad)*
        // cylinder2.uniform_parameter.color = vec3(0,0,1);
        // cylinder2.draw(shaders["mesh"],scene.camera);
        centi->centipede.rotation(abdo+std::to_string(i+1))= rotation_between_vector_mat3(inv*dprev,inv*d2)*rotation_between_vector_mat3(inv*grad_prev,inv*grad);
        // std::cout<<rotation_between_vector_mat3(v3,d2)<<std::endl;
        // centi->centipede.rotation(abdo+std::to_string(i+1))=/*rotation_from_axis_angle_mat3({0,1,0},-signe*std::sin(3.1415f*(t-0.4f)+i*3.1415/4)/10)*/supermatrice*rotation_between_vector_mat3(inv*grad_prev,inv*grad);
        // centi->centipede.rotation(abdo+std::to_string(i+1))=rotation_from_axis_angle_mat3({0,1,0},std::sin(2*3.1415f*(t-0.4f)+i*3.1415/4)/7)*rotation_between_vector_mat3(inv*grad_prev,inv*grad);
        grad_prev = grad;
        dprev = d2;
    }
}
void scene_exercise::display_trajectory(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    if(gui_scene.trajectory)
    {
        const size_t N = trajectory.time.size();
        segment_drawer.uniform_parameter.color = vec3(0,1,0);
        for(size_t k=0; k<N-1; ++k)
        {
            segment_drawer.uniform_parameter.p1 = trajectory.position[k];
            segment_drawer.uniform_parameter.p2 = trajectory.position[k+1];
            segment_drawer.draw(shaders["segment_im"], scene.camera);
        }

        for(size_t k=0; k<N; ++k)
        {
            sphere_trajectory.uniform_parameter.translation = trajectory.position[k];
            sphere_trajectory.uniform_parameter.scaling = 0.05f;
            if( picked_object==int(k) )
                sphere_trajectory.uniform_parameter.color = {1,0,0};
            else
                sphere_trajectory.uniform_parameter.color = {1,1,1};
            sphere_trajectory.draw(shaders["mesh"], scene.camera);
        }

    }
}
vec3 gradient_du_terrain(float u, float v){
    float h = 0.01f;
    float fuv = evaluate_terrain_z_sans_bruit(u,v);
    float dfdx = (evaluate_terrain_z_sans_bruit(u+h,v)-fuv)/h;
    float dfdy = (evaluate_terrain_z_sans_bruit(u,v+h)-fuv)/h;

    vec3 dir1 = {1.0f,0.0f,dfdx/20};
    vec3 dir2 = {0.0f,1.0f,dfdy/20};
    return (20*cross(dir1,dir2));
}
float evaluate_terrain_z_sans_bruit(float u, float v){
    const std::vector<vec2> pi = {{0,0}, {0.5f,0.5f}, {0.2f,0.7f}, {0.8f,0.7f}};
    const std::vector<float> hi = {3.0f, 1.5f, 1.0f, 2.0f};
    const std::vector<float> sigma_i = {0.5f, 0.15f, 0.2f, 0.2f};
    const size_t N = pi.size();
    float z = 0.0f;
    for(size_t k=0; k<N; ++k)
    {
        const float u0 = pi[k].x;
        const float v0 = pi[k].y;
        const float d2 = (u-u0)*(u-u0)+(v-v0)*(v-v0);
        z += hi[k] * std::exp( - d2/sigma_i[k]/sigma_i[k] );
    }

    return z;
}
float evaluate_terrain_z(float u, float v)
{
    const std::vector<vec2> pi = {{0,0}, {0.5f,0.5f}, {0.2f,0.7f}, {0.8f,0.7f}};
    const std::vector<float> hi = {3.0f, -1.5f, 1.0f, 2.0f};
    const std::vector<float> sigma_i = {0.5f, 0.15f, 0.2f, 0.2f};

    const size_t N = pi.size();
    float z = 0.0f;
    for(size_t k=0; k<N; ++k)
    {
        const float u0 = pi[k].x;
        const float v0 = pi[k].y;
        const float d2 = (u-u0)*(u-u0)+(v-v0)*(v-v0);
        z += hi[k] * std::exp( - d2/sigma_i[k]/sigma_i[k] );

        z += 0.2f*perlin(3*u,3*v, 7, 0.4f);
    }

    return z;
}
vec3 evaluate_terrain(float u, float v)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);
    const float z = evaluate_terrain_z_sans_bruit(u,v);

    return {x,y,z};
}
mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const size_t N = 600;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.texture_uv.resize(N*N);
    terrain.color.resize(N*N);

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
            terrain.texture_uv[kv+N*ku] = {5*u, 5*v};

            const float c = 0.5f+0.5f*std::max(std::min(terrain.position[kv+N*ku].z/2.0f,1.0f),0.0f);
            terrain.color[kv+N*ku] = {c,c,c,1.0f};
        }
    }


    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    assert(Ns >= 2);
    for(unsigned int ku=0; ku<Ns-1; ++ku)
    {
        for(unsigned int kv=0; kv<Ns-1; ++kv)
        {
            const unsigned int idx = kv + Ns*ku; // current vertex offset

            const index3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const index3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    return terrain;
}
mesh create_cylinder(float radius, float height)
{
    mesh m;
    const size_t N = 20;
    for(size_t k=0; k<N; ++k)
    {
        const float u = k/float(N);
        const vec3 p = {radius*std::cos(2*3.14f*u), radius*std::sin(2*3.14f*u), 0.0f};
        m.position.push_back( p );
        m.position.push_back( p+vec3(0,0,height) );
    }

    const unsigned int Ns = N;
    for(unsigned int k=0; k<Ns; ++k)
    {
        const unsigned int u00 = 2*k;
        const unsigned int u01 = (2*k+1)%(2*N);
        const unsigned int u10 = (2*(k+1))%(2*N);
        const unsigned int u11 = (2*(k+1)+1) % (2*N);

        const index3 t1 = {u00, u10, u11};
        const index3 t2 = {u00, u11, u01};
        m.connectivity.push_back(t1);
        m.connectivity.push_back(t2);
    }


    return m;
}
mesh create_pot(float radius1, float radius2, float height, float offset){

    mesh m;
    //number of samples
    const size_t N = 20;

    // Geometry
    for (size_t k=0;k<N;k++){
        const float u = k/float(N);
        const vec3 p = {radius1*std::cos(2*3.141592f*u),radius1*std::sin(2*3.141592f*u),offset};
        const vec3 q = {radius2*std::cos(2*3.141592f*u),radius2*std::sin(2*3.141592f*u),height+offset};
        m.position.push_back(p);
        m.position.push_back(q);
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
mesh create_abdomen_elementaire(float r){

    mesh pot1, pot2, pot3, pot4, pot5, pot6, pot7, pot8, pot9;
    pot1 = create_pot(0.2f*r,0.175f*r,0.1f,0.0f);
    pot2 = create_pot(0.175f*r,0.15f*r,0.05f,0.1f);
    pot3 = create_pot(0.15f*r,0.125f*r,0.01f,0.15f);
    pot4 = create_pot(0.125f*r,0.1f*r,0.005f,0.16f);
    pot5 = create_pot(0.19f*r,0.2f*r,0.025f,-0.025f);
    pot6 = create_pot(0.18f*r,0.19f*r,0.025f,-0.05f);
    pot7 = create_pot(0.15f*r,0.18f*r,0.025f,-0.075f);
    pot8 = create_pot(0.125f*r,0.15f*r,0.025f,-0.1f);
    pot9 = create_pot(0.05f*r,0.125f*r,0.025f,-0.125f);

    mesh m = pot1;
    m.push_back(pot2);
    m.push_back(pot3);
    m.push_back(pot4);
    m.push_back(pot5);
    m.push_back(pot6);
    m.push_back(pot7);
    m.push_back(pot8);
    m.push_back(pot9);
    return m;
}
mesh create_abdomen_elementaire(int nb_pots,float r, float l){
    mesh m;
    int n = nb_pots/2;
    float fr1, fr2, fl = 1, offset = 0, backoffset = -l;
    for (int i = 0; i<n;i++){
        fr1 = r-i*(r/2)/n;
        fr2 = r-(i+1)*(r/2)/n;
        mesh potfront = create_pot(fr1*r,fr2*r,fl*l,offset);
        mesh potback = create_pot(fr2*r,fr1*r,fl*l,backoffset);
        m.push_back(potfront);
        m.push_back(potback);
        offset+=fl*l;
        backoffset-=fl*l/2;
        fl/=2;
    }
    return m;
}
mesh patte(float r, float l){

    mesh pot1, pot2, pot3, pot4, pot5, pot6, pot7, pot8, pot9;
    pot1 = create_pot(0.2f*r,0.175f*r,0.1f*l,0.0f*l);
    pot2 = create_pot(0.175f*r,0.15f*r,0.05f*l,0.1f*l);
    pot3 = create_pot(0.15f*r,0.125f*r,0.01f*l,0.15f*l);
    pot4 = create_pot(0.125f*r,0.1f*r,0.005f*l,0.16f*l);
    pot5 = create_pot(0.19f*r,0.2f*r,0.025f*l,-0.025f*l);
    pot6 = create_pot(0.18f*r,0.19f*r,0.025f*l,-0.05f*l);
    pot7 = create_pot(0.15f*r,0.18f*r,0.025f*l,-0.075f*l);
    pot8 = create_pot(0.125f*r,0.15f*r,0.025f*l,-0.1f*l);
    pot9 = create_pot(0.05f*r,0.125f*r,0.025f*l,-0.125f*l);

    mesh m = pot1;
    m.push_back(pot2);
    m.push_back(pot3);
    m.push_back(pot4);
    m.push_back(pot5);
    m.push_back(pot6);
    m.push_back(pot7);
    m.push_back(pot8);
    m.push_back(pot9);
    return m;
}
int nearest_integer(float x){
    if (std::abs(std::floor(x)-x)<std::abs(std::ceil(x)-x)){
        return (int) std::floor(x);
    }
    return (int) std::ceil(x);
}
void scene_exercise::update_trajectory()
{
    float x, y, u, v;
    int ku, kv;
    const size_t Nterrain = 600;
    const size_t N = 12;
    const float r = 1.0f;
    vec3 offset = {0.0f,0.0f,0.05f};
    for(size_t k=0; k<N; ++k)
    {
        const float theta = k%(N-3)/(N-3.0f);
        x=r*std::cos(2*3.14f*theta), y = r*std::sin(2*3.14f*theta);
        u = x/20 +0.5f;
        v = y/20 + 0.5f;
        ku = nearest_integer(u*(Nterrain-1.0f));
        kv = nearest_integer(v*(Nterrain-1.0f));
        const vec3 p = terrain_mesh.position[ku*Nterrain+kv]+offset;
        trajectory.position.push_back(p);
    }

    update_time_trajectory();

}

void scene_exercise::update_time_trajectory()
{
    const float max_time = 10.0f;

    const size_t N = trajectory.position.size();
    trajectory.time.resize(N);
    float length = 0.0f;
    for(size_t k=0; k<N-1; ++k)
    {
        const vec3& p0 = trajectory.position[k];
        const vec3& p1 = trajectory.position[k+1];
        const float L = norm(p1-p0);
        length += L;
    }

    trajectory.time[0] = 0.0f;
    for(size_t k=1; k<N-1; ++k)
    {
        const vec3& p0 = trajectory.position[k-1];
        const vec3& p1 = trajectory.position[k];
        const float L = norm(p1-p0);

        trajectory.time[k] = trajectory.time[k-1]+L/length * max_time;
    }
    trajectory.time[N-1]=max_time;

    timer.t_min = trajectory.time[1];
    timer.t_max = trajectory.time[trajectory.time.size()-2];
}

mesh_drawable_hierarchy create_centipede()
{
    mesh_drawable_hierarchy hierarchy;
    hierarchy.scaling = 0.2f;

    const float r_head = 0.1f;
    const float l = 0.1f;
    mesh_drawable head = mesh_primitive_sphere(r_head,{0,0,0},40,40);
    head.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
    hierarchy.add_element(head, "head", "root");
    mesh_drawable eye = mesh_primitive_sphere(0.05f,{0,0,0},20,20);
    eye.uniform_parameter.color = {0,0,0};
    hierarchy.add_element(eye, "eye_left", "head",{r_head/3,r_head/2,r_head/1.5f});
    hierarchy.add_element(eye, "eye_right", "head",{-r_head/3,r_head/2,r_head/1.5f});
    float f = 0.5;
    std::string abdo = "abdomen", parent = "head";
    for(int i = 0; i<5;i++){
        mesh_drawable abdomen = create_abdomen_elementaire(f);
        abdomen.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
        hierarchy.add_element(abdomen,abdo+std::to_string(i+1),parent,{0.0f,0.0f,-0.23f});
        parent = abdo+std::to_string(i+1);
        f+=0.05;
    }
    for(int i = 0; i<5;i++){
        mesh_drawable abdomen = create_abdomen_elementaire(f);
        abdomen.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
        hierarchy.add_element(abdomen,abdo+std::to_string(5+i+1),parent,{0.0f,0.0f,-0.23f});
        parent = abdo+std::to_string(5+i+1);
        f-=0.05;
    }
    std::string string_patte = "patte";
    for(int k = 0; k<10;k++){
        parent = abdo+std::to_string(k+1);
        f = 0.05;
        mesh_drawable patte_temp = patte(f,l);
        patte_temp.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
        hierarchy.add_element(patte_temp,string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(1),parent,{-0.08f,-0.07f,0.0f});
        f+=0.05/10; parent = string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(1);
        for(int i = 1; i<5;i++){
            mesh_drawable patte_temp = patte(f,l);
            patte_temp.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
            hierarchy.add_element(patte_temp,string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(i+1),parent,{0.0f,0.0f,-0.23f*l});
            f+=0.05/10; parent = string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(i+1);
        }
        for(int i = 0; i<5;i++){
            mesh_drawable patte_temp = patte(f,l);
            patte_temp.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
            hierarchy.add_element(patte_temp,string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(5+i+1),parent,{0.0f,0.0f,-0.23f*l});
            f-=0.05/10;parent = string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(5+i+1);
        }
        parent = abdo+std::to_string(k+1);
        f = 0.05;
        patte_temp.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
        hierarchy.add_element(patte_temp,string_patte+"droite"+std::to_string(k+1)+","+std::to_string(1),parent,{0.08f,-0.07f,0.0f});
        f+=0.05/10; parent = string_patte+"droite"+std::to_string(k+1)+","+std::to_string(1);
        for(int i = 1; i<5;i++){
            mesh_drawable patte_temp = patte(f,l);
            patte_temp.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
            hierarchy.add_element(patte_temp,string_patte+"droite"+std::to_string(k+1)+","+std::to_string(i+1),parent,{0.0f,0.0f,0.23f*l});
            f+=0.05/10; parent = string_patte+"droite"+std::to_string(k+1)+","+std::to_string(i+1);
        }
        for(int i = 0; i<5;i++){
            mesh_drawable patte_temp = patte(f,l);
            patte_temp.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
            hierarchy.add_element(patte_temp,string_patte+"droite"+std::to_string(k+1)+","+std::to_string(5+i+1),parent,{0.0f,0.0f,0.23f*l});
            f-=0.05/10;parent = string_patte+"droite"+std::to_string(k+1)+","+std::to_string(5+i+1);
        }

    }
    return hierarchy;
}

size_t index_at_value(float t, const std::vector<float>& vt)
{
    const size_t N = vt.size();
    assert(vt.size()>=2);
    assert(t>=vt[0]);
    assert(t<vt[N-1]);

    size_t k=0;
    while( vt[k+1]<t )
        ++k;
    return k;
}
vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3)
{
    const float sigma = t2-t1;

    const vec3 d1 = (p2-p0)/(t2-t0) * sigma;
    const vec3 d2 = (p3-p1)/(t3-t1) * sigma;

    const float s = (t-t1)/sigma;
    const float s2 = s*s;
    const float s3 = s2*s;

    const vec3 p = (2*s3-3*s2+1)*p1 + (s3-2*s2+s)*d1 + (-2*s3+3*s2)*p2 + (s3-s2)*d2;

    return p;
}
vec3 cardinal_spline_derivative_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3)
{
    const float sigma = t2-t1;

    const vec3 d1 = (p2-p0)/(t2-t0) * sigma;
    const vec3 d2 = (p3-p1)/(t3-t1) * sigma;

    const float s = (t-t1)/sigma;
    const float s2 = s*s;

    const vec3 p = (6*s2-6*s)*p1 + (3*s2-4*s+1)*d1 + (-6*s2+6*s)*p2 + (3*s2-2*s)*d2;

    return p;
}
vec3 cardinal_spline_interpolation(const trajectory_structure& trajectory, float t)
{
    const size_t idx = index_at_value(t, trajectory.time);

    const float t0 = trajectory.time[idx-1];
    const float t1 = trajectory.time[idx];
    const float t2 = trajectory.time[idx+1];
    const float t3 = trajectory.time[idx+2];

    const vec3& p0 = trajectory.position[idx-1];
    const vec3& p1 = trajectory.position[idx];
    const vec3& p2 = trajectory.position[idx+1];
    const vec3& p3 = trajectory.position[idx+2];

    //const vec3 p = linear_interpolation(t,t1,t2,p1,p2);
    const vec3 p = cardinal_spline_interpolation(t,t0,t1,t2,t3,p0,p1,p2,p3);
    return p;
}
vec3 cardinal_spline_derivative_interpolation(const trajectory_structure& trajectory, float t)
{
    const size_t idx = index_at_value(t, trajectory.time);

    const float t0 = trajectory.time[idx-1];
    const float t1 = trajectory.time[idx];
    const float t2 = trajectory.time[idx+1];
    const float t3 = trajectory.time[idx+2];

    const vec3& p0 = trajectory.position[idx-1];
    const vec3& p1 = trajectory.position[idx];
    const vec3& p2 = trajectory.position[idx+1];
    const vec3& p3 = trajectory.position[idx+2];

    const vec3 p = cardinal_spline_derivative_interpolation(t,t0,t1,t2,t3,p0,p1,p2,p3);
    return p;
}


void scene_exercise::mouse_click(scene_structure& scene, GLFWwindow* window, int , int action, int )
{
    // Mouse click is used to select a position of the control polygon
    // ******************************************************************** //

    // Window size
    int w=0;
    int h=0;
    glfwGetWindowSize(window, &w, &h);

    // Current cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Convert pixel coordinates to relative screen coordinates between [-1,1]
    const float x = 2*float(xpos)/float(w)-1;
    const float y = 1-2*float(ypos)/float(h);

    // Check if shift key is pressed
    const bool key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));

    if(action==GLFW_PRESS && key_shift)
    {
        // Create the 3D ray passing by the selected point on the screen
        const ray r = picking_ray(scene.camera, x,y);

        // Check if this ray intersects a position (represented by a sphere)
        //  Loop over all positions and get the intersected position (the closest one in case of multiple intersection)
        const size_t N = trajectory.position.size();
        picked_object = -1;
        float distance_min = 0.0f;
        for(size_t k=0; k<N; ++k)
        {
            const vec3 c = trajectory.position[k];
            const picking_info info = ray_intersect_sphere(r, c, 0.1f);

            if( info.picking_valid ) // the ray intersects a sphere
            {
                const float distance = norm(info.intersection-r.p); // get the closest intersection
                if( picked_object==-1 || distance<distance_min ){
                    picked_object = k;
                }
            }
        }
    }

}

void scene_exercise::mouse_move(scene_structure& scene, GLFWwindow* window)
{
    // Mouse move is used to translate a position once selected
    // ******************************************************************** //

    // Window size
    int w=0;
    int h=0;
    glfwGetWindowSize(window, &w, &h);

    // Current cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Convert pixel coordinates to relative screen coordinates between [-1,1]
    const float x = 2*float(xpos)/float(w)-1;
    const float y = 1-2*float(ypos)/float(h);

    // Check that the mouse is clicked (drag and drop)
    const bool mouse_click_left  = (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT )==GLFW_PRESS);
    const bool key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));

    const size_t N = trajectory.position.size();
    if(mouse_click_left && key_shift && picked_object!=-1)
    {
        // Translate the selected object to the new pointed mouse position within the camera plane
        // ************************************************************************************** //

        // Get vector orthogonal to camera orientation
        const mat4 M = scene.camera.camera_matrix();
        const vec3 n = {M(0,2),M(1,2),M(2,2)};

        // Compute intersection between current ray and the plane orthogonal to the view direction and passing by the selected object
        const ray r = picking_ray(scene.camera, x,y);
        vec3& p0 = trajectory.position[picked_object];
        const picking_info info = ray_intersect_plane(r,n,p0);

        // translate the position
        p0 = info.intersection;

        // Make sure that duplicated positions are moved together
        int Ns = N;
        if(picked_object==0 || picked_object==Ns-3){
            trajectory.position[0] = info.intersection;
            trajectory.position[N-3] = info.intersection;
        }
        if(picked_object==1 || picked_object==Ns-2){
            trajectory.position[1] = info.intersection;
            trajectory.position[N-2] = info.intersection;
        }
        if(picked_object==2 || picked_object==Ns-1){
            trajectory.position[2] = info.intersection;
            trajectory.position[N-1] = info.intersection;
        }

        update_time_trajectory();

    }
}
void scene_exercise::set_gui()
{


    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
    ImGui::Checkbox("Texture", &gui_scene.texture);
    ImGui::Checkbox("Trajectory", &gui_scene.trajectory);

    ImGui::SliderFloat("Time scale", &gui_scene.time_scale,0.01f,3.0f);
}

#endif