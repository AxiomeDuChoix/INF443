#include "centipede.hpp"
#include <string.h>


#ifdef INF443_CENTIPEDE


using namespace vcl;


mesh create_cylinder(float radius, float height);
mesh create_pot(float radius1, float radius2, float height);
mesh create_abdomen_elementaire(float r);
mesh patte(float r, float l);
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
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
    // mesh arm_top_left = mesh_primitive_cylinder(r_cylinder, {0,0,0}, {-l_arm,0,0});
    // mesh arm_bottom_left = mesh_primitive_cylinder(r_cylinder, {0,0,0}, {-l_arm/1.5f,-l_arm/1.0f,0});
    // mesh arm_top_right = mesh_primitive_cylinder(r_cylinder, {0,0,0}, {l_arm,0,0});
    // mesh arm_bottom_right = mesh_primitive_cylinder(r_cylinder, {0,0,0}, {l_arm/1.5f,-l_arm/1.0f,0});

    // mesh shoulder = mesh_primitive_sphere(0.055f);



    // hierarchy.add_element(arm_top_left, "arm_top_left", "head",{-r_head+0.05f,0,0});
    // hierarchy.add_element(arm_bottom_left, "arm_bottom_left", "arm_top_left",{-l_arm,0,0});

    // hierarchy.add_element(arm_top_right, "arm_top_right", "head",{r_head-0.05f,0,0});
    // hierarchy.add_element(arm_bottom_right, "arm_bottom_right", "arm_top_right",{l_arm,0,0});

    // hierarchy.add_element(shoulder, "shoulder_left", "arm_bottom_left");
    // hierarchy.add_element(shoulder, "shoulder_right", "arm_bottom_right");

    timer.scale = 0.5f;
}




void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    set_gui();

    const float t = timer.t;
    std::string abdo = "abdomen", string_patte = "patte";

    // hierarchy.translation("head") = {0,0,0.2f*(1+std::sin(2*3.14f*t))};
    hierarchy.draw(shaders["mesh"], scene.camera);
    if(gui_scene.wireframe)
        hierarchy.draw(shaders["wireframe"], scene.camera);
    for(int i = 0; i<10;i++){
        hierarchy.rotation(abdo+std::to_string(i+1))= rotation_from_axis_angle_mat3({0,1,0},std::sin(2*3.1415f*(t-0.4f)+i*3.1415/4)/7);
    }
    for (int k = 0; k<10;k++){
        hierarchy.rotation(string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(1))= rotation_from_axis_angle_mat3({0,1,0},-std::sin(20*3.1415f*(t-0.4f)))*rotation_from_axis_angle_mat3({0,0,1},3.1415f/3)*rotation_from_axis_angle_mat3({0,1,0},3.1415f/2);
        hierarchy.rotation(string_patte+"droite"+std::to_string(k+1)+","+std::to_string(1))= rotation_from_axis_angle_mat3({0,1,0},-std::sin(20*3.1415f*(t-0.4f)+3.1415f/2))*rotation_from_axis_angle_mat3({0,0,1},-3.1415f/3)*rotation_from_axis_angle_mat3({0,1,0},3.1415f/2);

    }
    for (int k = 0; k<10;k++){
        for(int i = 1; i<10;i++){
            hierarchy.rotation(string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(i+1))= rotation_from_axis_angle_mat3({0,1,0},3.1415f/40);
            hierarchy.rotation(string_patte+"droite"+std::to_string(k+1)+","+std::to_string(i+1))= rotation_from_axis_angle_mat3({0,1,0},-3.1415f/40);

        }
    }

    // hierarchy.rotation("arm_top_left") = rotation_from_axis_angle_mat3({0,1,0}, std::sin(2*3.14f*(t-0.4f)) );
    // hierarchy.rotation("arm_bottom_left") = rotation_from_axis_angle_mat3({0,1,0}, std::sin(2*3.14f*(t-0.6f)) );

    // hierarchy.rotation("arm_top_right") = rotation_from_axis_angle_mat3({0,-1,0}, std::sin(2*3.14f*(t-0.4f)) );
    // hierarchy.rotation("arm_bottom_right") = rotation_from_axis_angle_mat3({0,-1,0}, std::sin(2*3.14f*(t-0.6f)) );

    // hierarchy.translation("body") = {0,0,0};

    // /*{0,0,0.2f*(1+std::sin(2*3.14f*t))};*/

    // hierarchy.rotation("arm_top_left") = rotation_from_axis_angle_mat3({0,1,0}, std::sin(2*3.14f*(t-0.4f)) );
    // hierarchy.rotation("arm_bottom_left") = rotation_from_axis_angle_mat3({0,1,0}, std::sin(2*3.14f*(t-0.6f)) );

    // hierarchy.rotation("arm_top_right") = rotation_from_axis_angle_mat3({0,-1,0}, std::sin(2*3.14f*(t-0.4f)) );
    // hierarchy.rotation("arm_bottom_right") = rotation_from_axis_angle_mat3({0,-1,0}, std::sin(2*3.14f*(t-0.6f)) );

    // hierarchy.draw(shaders["mesh"], scene.camera);
    // if(gui_scene.wireframe)
    //     hierarchy.draw(shaders["wireframe"], scene.camera);

    // display trees


// Yeux de caméléon:
    // mesh_drawable pot1, pot2, pot3, pot4, pot5, pot6, pot7;
    // pot1 = mesh_drawable(create_pot(0.3f,0.2f,0.1f));
    // pot2 = mesh_drawable(create_pot(0.2f,0.15f,0.1f));
    // pot3 = mesh_drawable(create_pot(0.15f,0.1f,0.1f));
    // pot4 = mesh_drawable(create_pot(0.1f,0.05f,0.1f));

    // pot1.uniform_parameter.translation = {0.0f,0.0f,0.0f};
    // pot2.uniform_parameter.translation = {0.0f,0.0f,0.1f};
    // pot3.uniform_parameter.translation = {0.0f,0.0f,0.2f};
    // pot4.uniform_parameter.translation = {0.0f,0.0f,0.3f};
    // /*cylinder.uniform_parameter.color = {0.0f, 0.0f, 0.0f};*/
    // pot1.draw(shaders["mesh"], scene.camera);
    // pot2.draw(shaders["mesh"], scene.camera);
    // pot3.draw(shaders["mesh"], scene.camera);
    // pot4.draw(shaders["mesh"], scene.camera);
    // float f = 0.5, translation_factor = 0.0f;
    // for(int i = 0; i<5;i++){
    //     mesh_drawable abdomen = create_abdomen_elementaire(f);
    //     abdomen.uniform_parameter.translation = {0.0f,0.0f,translation_factor};
    //     abdomen.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
    //     abdomen.draw(shaders["mesh"], scene.camera);
    //     f+=0.05; translation_factor+=0.23f;
    // }
    //     for(int i = 0; i<5;i++){
    //     mesh_drawable abdomen = create_abdomen_elementaire(f);
    //     abdomen.uniform_parameter.translation = {0.0f,0.0f,translation_factor};
    //     abdomen.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
    //     abdomen.draw(shaders["mesh"], scene.camera);
    //     f-=0.05; translation_factor+=0.23f;
    // }
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
