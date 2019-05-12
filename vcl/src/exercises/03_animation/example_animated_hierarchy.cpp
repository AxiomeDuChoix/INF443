
#include "example_animated_hierarchy.hpp"


#ifdef INF443_EXAMPLE_ANIMATED_HIERARCHY


using namespace vcl;

/** Function returning the index i such that t \in [vt[i],vt[i+1]] */
static int counter = 0;
static size_t index_at_value(float t, const std::vector<float>& vt);

static vec3 linear_interpolation(float t, float t1, float t2, const vec3& p1, const vec3& p2);
static vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3);
static vec3 cardinal_spline_speed(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3);
static float angle(vec3 v,int n);

void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
    const float r_body = 0.25f;
    const float r_cylinder = 0.05f;
    const float l_arm = 0.2f;
    mesh body = mesh_primitive_sphere(r_body,{0,0,0},40,40);


    mesh_drawable eye = mesh_primitive_sphere(0.05f,{0,0,0},20,20);
    mesh_drawable bec= mesh_primitive_cone(0.07f,  {0,0,0}, {0,0,0.2}, 20, 20);
    eye.uniform_parameter.color = {0,0,0};
    bec.uniform_parameter.color = {1.0,0.5,0};
    mesh arm_top_left = mesh_primitive_quad({0,0,-l_arm/2},{-l_arm,0,-l_arm/3}, {-l_arm,0,l_arm/3}, {0,0,l_arm/2});
    mesh arm_bottom_left = mesh_primitive_quad({0,0,-l_arm/3},{-l_arm/3,0,-l_arm/4}, {-l_arm/3,0,l_arm/4}, {0,0,l_arm/3});
    mesh arm_top_right = mesh_primitive_quad({0,0,-l_arm/2},{0,0,l_arm/2}, {l_arm,0,l_arm/3}, {l_arm,0,-l_arm/3});
    mesh arm_bottom_right = mesh_primitive_quad({0,0,-l_arm/3},{0,0,l_arm/3}, {l_arm/3,0,l_arm/4}, {l_arm/3,0,-l_arm/4});
        

    mesh shoulder = mesh_primitive_sphere(0.055f);

    hierarchy.add_element(body, "body", "root");
    hierarchy.add_element(eye, "eye_left", "body",{r_body/3,r_body/2,r_body/1.5f});
    hierarchy.add_element(eye, "eye_right", "body",{-r_body/3,r_body/2,r_body/1.5f});
    hierarchy.add_element(bec,"bec","body",{0,0,r_body/1.1f});

    hierarchy.add_element(arm_top_left, "arm_top_left", "body",{-r_body+0.05f,0,0});
    hierarchy.add_element(arm_bottom_left, "arm_bottom_left", "arm_top_left",{-l_arm,0,0});

    hierarchy.add_element(arm_top_right, "arm_top_right", "body",{r_body-0.05f,0,0});
    hierarchy.add_element(arm_bottom_right, "arm_bottom_right", "arm_top_right",{l_arm,0,0});

    // hierarchy.add_element(shoulder, "shoulder_left", "arm_bottom_left");
    // hierarchy.add_element(shoulder, "shoulder_right", "arm_bottom_right");

    timer.scale = 0.5f;

        // Initial Keyframe data
    keyframe_position = {{0,0,0}, {1,0,0}, {1,1,0}, {2,1,0}, {3,1,0}, {3,0,0}, {4,0,0}, {4,-1,0}, {0,-1,0}, {0,0,0}, {1,0,0}, {1,1,0}};
    keyframe_time = {0,1,2,3,4,5,6,7,8,9,10,11};



    // Set timer bounds
    //  To ease spline interpolation of a closed curve time \in [t_1,t_{N-2}]
    timer.t_min = keyframe_time[1];
    timer.t_max = keyframe_time[keyframe_time.size()-2];
    timer.t = timer.t_min;



    sphere = mesh_primitive_sphere();
    sphere.uniform_parameter.color = {1,1,1};
    sphere.uniform_parameter.scaling = 0.05f;

    segment_drawer.init();

    trajectory = curve_dynamic_drawable(100); // number of steps stroed in the trajectory
    trajectory.uniform_parameter.color = {0,0,1};

    picked_object=-1;
}




void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    set_gui();

    const float t = timer.t;

 // ********************************************* //
    // Compute interpolated position at time t
    // ********************************************* //
    const size_t idx = index_at_value(t, keyframe_time);

    // Assume a closed curve trajectory
    const size_t N = keyframe_time.size();



    // // Linear interpolation
    // const float t1 = keyframe_time[idx];
    // const float t2 = keyframe_time[idx+1];

    // const vec3& p1 = keyframe_position[idx];
    // const vec3& p2 = keyframe_position[idx+1];

    //modif:

    const float t0 = keyframe_time[idx-1];
    const float t1 = keyframe_time[idx];
    const float t2 = keyframe_time[idx+1];
    const float t3 = keyframe_time[idx+2];

    const vec3& p0 = keyframe_position[idx-1];
    const vec3& p1 = keyframe_position[idx];
    const vec3& p2 = keyframe_position[idx+1];
    const vec3& p3 = keyframe_position[idx+2];

    // const vec3 p = linear_interpolation(t,t1,t2,p1,p2);
    const vec3 p = cardinal_spline_interpolation(t,t0,t1,t2,t3,p0,p1,p2,p3);
    const vec3 v = cardinal_spline_speed(t,t0,t1,t2,t3,p0,p1,p2,p3);
    trajectory.add_point(p);


    // Draw current position

    hierarchy.translation("body") = p;
/*    hierarchy.rotation("body") =  rotation_from_axis_angle_mat3(v,std::acos(dot(-cross(v,{0,0,1}),{0,0,1})/norm(cross(v,{0,0,1}))))*rotation_from_axis_angle_mat3(-cross(v,{0,0,1}),3.1415/2);*/
    //rotation_between_vector_mat3({0,0,1},v);
    /*rotation_from_axis_angle_mat3({0,0,1}, std::acos(dot({1,0,0},v)/norm(v)));*/
    hierarchy.rotation("body")=rotation_from_axis_angle_mat3(v,angle(v,counter))*rotation_between_vector_mat3({0,0,1},v);
    hierarchy.rotation("arm_top_left") = rotation_from_axis_angle_mat3({0,0,1}, std::sin(2*3.14f*(t-0.4f)) );
    hierarchy.rotation("arm_bottom_left") = rotation_from_axis_angle_mat3({0,0,1}, 0.5*std::sin(2*3.14f*(t-0.6f)) );

    hierarchy.rotation("arm_top_right") = rotation_from_axis_angle_mat3({0,0,-1}, std::sin(2*3.14f*(t-0.4f)) );
    hierarchy.rotation("arm_bottom_right") = rotation_from_axis_angle_mat3({0,0,-1}, std::sin(2*3.14f*(t-0.6f)) );

    hierarchy.draw(shaders["mesh"], scene.camera);
    if(gui_scene.wireframe)
        hierarchy.draw(shaders["wireframe"], scene.camera);



    // Draw sphere at each keyframe position
    for(size_t k=0; k<N; ++k)
    {
        const vec3& p_keyframe = keyframe_position[k];
        sphere.uniform_parameter.translation = p_keyframe;
        sphere.draw(shaders["mesh"],scene.camera);
    }


    // Draw segments between each keyframe
    for(size_t k=0; k<keyframe_position.size()-1; ++k)
    {
        const vec3& pa = keyframe_position[k];
        const vec3& pb = keyframe_position[k+1];

        segment_drawer.uniform_parameter.p1 = pa;
        segment_drawer.uniform_parameter.p2 = pb;
        segment_drawer.draw(shaders["segment_im"], scene.camera);
    }

    // Draw moving point trajectory
    trajectory.draw(shaders["curve"], scene.camera);

    // Draw selected sphere in red
    if( picked_object!=-1 )
    {
        const vec3& p_keyframe = keyframe_position[picked_object];
        sphere.uniform_parameter.color = vec3(1,0,0);
        sphere.uniform_parameter.scaling = 0.06f;
        sphere.uniform_parameter.translation = p_keyframe;
        sphere.draw(shaders["mesh"],scene.camera);
        sphere.uniform_parameter.color = vec3(1,1,1);
        sphere.uniform_parameter.scaling = 0.05f;
    }
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
        const size_t N = keyframe_position.size();
        picked_object = -1;
        float distance_min = 0.0f;
        for(size_t k=0; k<N; ++k)
        {
            const vec3 c = keyframe_position[k];
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

    const size_t N = keyframe_position.size();
    if(mouse_click_left && key_shift && picked_object!=-1)
    {
        // Translate the selected object to the new pointed mouse position within the camera plane
        // ************************************************************************************** //

        // Get vector orthogonal to camera orientation
        const mat4 M = scene.camera.camera_matrix();
        const vec3 n = {M(0,2),M(1,2),M(2,2)};

        // Compute intersection between current ray and the plane orthogonal to the view direction and passing by the selected object
        const ray r = picking_ray(scene.camera, x,y);
        vec3& p0 = keyframe_position[picked_object];
        const picking_info info = ray_intersect_plane(r,n,p0);

        // translate the position
        p0 = info.intersection;

        // Make sure that duplicated positions are moved together
        int Ns = N;
        if(picked_object==0 || picked_object==Ns-3){
            keyframe_position[0] = info.intersection;
            keyframe_position[N-3] = info.intersection;
        }
        if(picked_object==1 || picked_object==Ns-2){
            keyframe_position[1] = info.intersection;
            keyframe_position[N-2] = info.intersection;
        }
        if(picked_object==2 || picked_object==Ns-1){
            keyframe_position[2] = info.intersection;
            keyframe_position[N-1] = info.intersection;
        }
    }
}

static size_t index_at_value(float t, const std::vector<float>& vt)
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


static vec3 linear_interpolation(float t, float t1, float t2, const vec3& p1, const vec3& p2)
{
    const float alpha = (t-t1)/(t2-t1);
    const vec3 p = (1-alpha)*p1 + alpha*p2;

    return p;
}

static vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3){
    const float mu = 0.5f;
    const float s = (t-t1)/(t2-t1);
    const vec3 d1 = mu*(t2-t1)/(t2-t0)*(p2-p0);
    const vec3 d2 = mu*(t2-t1)/(t3-t1)*(p3-p1);
    const vec3 p = (2*s*s*s-3*s*s+1)*p1+(s*s*s-2*s*s+s)*d1+(-2*s*s*s+3*s*s)*p2+(s*s*s-s*s)*d2;
    return p;
}

static vec3 cardinal_spline_speed(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3){
    const float mu = 0.5f;
    const float s = (t-t1)/(t2-t1);
    const vec3 d1 = mu*(t2-t1)/(t2-t0)*(p2-p0);
    const vec3 d2 = mu*(t2-t1)/(t3-t1)*(p3-p1);
    const vec3 v = ((6*s*s-6*s)*p1+(3*s*s-4*s+1)*d1+(-6*s*s+6*s)*p2+(3*s*s-2*s)*d2)/(t2-t1);
    return v;
}
static float angle(vec3 v,int n){
    v = rotation_from_axis_angle_mat3({0,0,1},3.14/2)*v;
    float v1 = v.x;
    float v2 = v.y;
    if (std::abs(v1)<0.1f) counter++;
    float resu = std::atan(v2/v1);
    if (n%2==0) resu+= 3.1415f;
    return resu;
}


void scene_exercise::set_gui()
{
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);

    const float time_scale_min = 0.1f;
    const float time_scale_max = 3.0f;
    ImGui::SliderFloat("Time scale", &timer.scale, time_scale_min, time_scale_max);
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);

    // ajout

    if( ImGui::Button("Print Keyframe") )
    {
        std::cout<<"keyframe_position={";
        for(size_t k=0; k<keyframe_position.size(); ++k)
        {
            const vec3& p = keyframe_position[k];
            std::cout<< "{"<<p.x<<"f,"<<p.y<<"f,"<<p.z<<"f}";
            if(k<keyframe_position.size()-1)
                std::cout<<", ";
        }
        std::cout<<"}"<<std::endl;
    }

}



#endif

