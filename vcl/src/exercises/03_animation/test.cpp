#include "test.hpp"


#ifdef INF443_TEST


using namespace vcl;

mesh create_cylinder(float radius, float height);
mesh create_pot(float radius1, float radius2, float height);
mesh patte(float r, float unit_size);
mesh antenne(float r, float unit_size);


void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
}


void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    float f = 0.05, translation_factor = 0.0f, l = 0.1;
    for(int i = 0; i<5;i++){
        mesh_drawable patti = patte(f,l);
        patti.uniform_parameter.translation = {0.0f,0.0f,translation_factor};
        patti.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
        patti.draw(shaders["mesh"], scene.camera);
        f+=0.05/10; translation_factor+=0.23f*l;
    }
        for(int i = 0; i<5;i++){
        mesh_drawable patti = patte(f,l);
        patti.uniform_parameter.translation = {0.0f,0.0f,translation_factor};
        patti.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
        patti.draw(shaders["mesh"], scene.camera);
        f-=0.05/10; translation_factor+=0.23f*l;
    }

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


void scene_exercise::set_gui()
{
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);

    const float time_scale_min = 0.1f;
    const float time_scale_max = 3.0f;
    ImGui::SliderFloat("Time scale", &timer.scale, time_scale_min, time_scale_max);
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);

}



#endif