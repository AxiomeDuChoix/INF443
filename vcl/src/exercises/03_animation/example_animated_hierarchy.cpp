
#include "example_animated_hierarchy.hpp"


#ifdef INF443_EXAMPLE_ANIMATED_HIERARCHY


using namespace vcl;

mesh create_aile(float l_aile,float L_aile);

void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
//    const float r_body = 0.25f;
//    const float r_cylinder = 0.05f;
//    const float l_arm = 0.2f;
//    mesh body = mesh_primitive_sphere(r_body,{0,0,0},40,40);
//    mesh_drawable eye = mesh_primitive_sphere(0.05f,{0,0,0},20,20);
//    eye.uniform_parameter.color = {0,0,0};

//    mesh arm_top_left = mesh_primitive_cylinder(r_cylinder, {0,0,0}, {-l_arm,0,0});
//    mesh arm_bottom_left = mesh_primitive_cylinder(r_cylinder, {0,0,0}, {-l_arm/1.5f,-l_arm/1.0f,0});
//    mesh arm_top_right = mesh_primitive_cylinder(r_cylinder, {0,0,0}, {l_arm,0,0});
//    mesh arm_bottom_right = mesh_primitive_cylinder(r_cylinder, {0,0,0}, {l_arm/1.5f,-l_arm/1.0f,0});

//    mesh shoulder = mesh_primitive_sphere(0.055f);

//    hierarchy.add_element(body, "body", "root");
//    hierarchy.add_element(eye, "eye_left", "body",{r_body/3,r_body/2,r_body/1.5f});
//    hierarchy.add_element(eye, "eye_right", "body",{-r_body/3,r_body/2,r_body/1.5f});

//    hierarchy.add_element(arm_top_left, "arm_top_left", "body",{-r_body+0.05f,0,0});
//    hierarchy.add_element(arm_bottom_left, "arm_bottom_left", "arm_top_left",{-l_arm,0,0});

//    hierarchy.add_element(arm_top_right, "arm_top_right", "body",{r_body-0.05f,0,0});
//    hierarchy.add_element(arm_bottom_right, "arm_bottom_right", "arm_top_right",{l_arm,0,0});

//    hierarchy.add_element(shoulder, "shoulder_left", "arm_bottom_left");
//    hierarchy.add_element(shoulder, "shoulder_right", "arm_bottom_right");


    const float L_body=0.75f;
    const float l_body=0.25f;
    const float r_head=0.25f;
    const float r_eye=0.02f;
    const float r_moustache=0.01f;
    const float l_moustache=0.10f;
    const float phi_eye=3.14/4;
    const float theta_eye=3.14/6;
    const float l_bec=0.30f;
    const float theta_bec=-3.14/6;
    const float l_aile=1.0f;
    const float L_aile=0.5f;

    mesh body=mesh_primitive_cylinder(l_body,{0,0,0},{L_body,0,0});
    body.push_back(mesh_primitive_sphere(l_body,{0,0,0}));
    body.push_back(mesh_primitive_sphere(l_body,{L_body,0,0}));
    hierarchy.add_element(body,"body","root",{0,0,0});

    mesh head=mesh_primitive_sphere(r_head);
    hierarchy.add_element(head,"head","body",{L_body+l_body/2+r_head,0,0});
    mesh_drawable eye = mesh_primitive_sphere(r_eye);
    eye.uniform_parameter.color={0,0,0};
    mesh_drawable moustache=mesh_primitive_cylinder(r_moustache,{0,0,0},{0,l_moustache,0});
    moustache.uniform_parameter.color={0,0,0};
    hierarchy.add_element(eye,"eye_left","head",{r_head*sin(phi_eye)*cos(theta_eye),r_head*sin(phi_eye)*sin(theta_eye),r_head*cos(phi_eye)});
    hierarchy.add_element(eye,"eye_right","head",{r_head*sin(phi_eye)*cos(theta_eye),-r_head*sin(phi_eye)*sin(theta_eye),r_head*cos(phi_eye)});
    hierarchy.add_element(moustache,"moustache_left","head",{0,r_head,0});
    hierarchy.add_element(moustache,"moustache_right","head",{0,-r_head-l_moustache,0});
    mesh_drawable bec=mesh_primitive_cone(r_head/4,{0,0,0},{l_bec*cos(theta_bec),0,l_bec*sin(theta_bec)});
    bec.uniform_parameter.color={1,1,0};
    hierarchy.add_element(bec,"bec","head",{0.9*r_head*cos(theta_bec),0,0.9*r_head*sin(theta_bec)});
    mesh aile= create_aile(l_aile,L_aile);
    hierarchy.add_element(aile,"aile_droite","body",{0,0,0});
    timer.scale = 0.5f;
}


mesh create_aile(float l_aile,float L_aile){
    const int N=30;
    mesh aile;
    aile.position.resize(2*N);
    aile.position[0]={L_aile/2,0,0};
    for(int i=0;i<N;i++){
        float facteur=i/(N-1);
        aile.position[i+1]={facteur*L_aile/2,facteur*l_aile+l_aile/2*cos(3.14159/2*facteur),0};
    }
    for(int i=N;i<2*N-1;i++){
        float facteur=1-(i-N+1)/(N-1);
        aile.position[i+1]={L_aile/2+facteur*L_aile/2,facteur*l_aile/2+l_aile/2*cos(3.14159/2*facteur),0};
    }
    for(int i=1;i<2*N-1;i++){
        const index3 triangle={0,i,i+1};
        aile.connectivity.push_back(triangle);
    }
    return aile;
}

void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    set_gui();

    const float t = timer.t;

//    hierarchy.translation("body") = {0,0,0.2f*(1+std::sin(2*3.14f*t))};

//    hierarchy.rotation("arm_top_left") = rotation_from_axis_angle_mat3({0,1,0}, std::sin(2*3.14f*(t-0.4f)) );
//    hierarchy.rotation("arm_bottom_left") = rotation_from_axis_angle_mat3({0,1,0}, std::sin(2*3.14f*(t-0.6f)) );

//    hierarchy.rotation("arm_top_right") = rotation_from_axis_angle_mat3({0,-1,0}, std::sin(2*3.14f*(t-0.4f)) );
//    hierarchy.rotation("arm_bottom_right") = rotation_from_axis_angle_mat3({0,-1,0}, std::sin(2*3.14f*(t-0.6f)) );

    hierarchy.draw(shaders["mesh"], scene.camera);
    if(gui_scene.wireframe)
        hierarchy.draw(shaders["wireframe"], scene.camera);

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

