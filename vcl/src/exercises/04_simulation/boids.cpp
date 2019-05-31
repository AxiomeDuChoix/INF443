
#include "boids.hpp"

#include <random>

#ifdef INF443_BOIDS

using namespace vcl;

// Generator for uniform random number
std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0,1.0);


static void set_gui(timer_event& timer);


void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& gui)
{
    // Default camera settings
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 5.0f;
    scene.camera.apply_rotation(0,0, -2.0f,1.2f);

    // Default gui display
    gui.show_frame_worldspace = true;
    gui.show_frame_camera = false;

    // Create mesh for particles represented as spheres
    const float r = 0.05f; // radius of spheres
    sphere = mesh_primitive_sphere(r);
    sphere.uniform_parameter.color = {0.6f, 0.6f, 1.0f};
    spherepredateur = mesh_primitive_sphere(3*r);
    spherepredateur.uniform_parameter.color = {1, 0, 0};

    // Create mesh for the ground displayed as a disc
    ground = mesh_primitive_disc(10.0f, {0,0,-r}, {0,0,1}, 80);
    ground.uniform_parameter.color = {1,1,1};

    // Delay between emission of a new particles
    timer.periodic_event_time_step = 0.2f;

    // Initialize boids' positions and speeds
    bool boolean;
    vec3 test;
    for (int i = 0; i<10;i++)
    {
        particle_structure new_particle;
        boolean = true;
        while (boolean){
            test = {5*distrib(generator),5*distrib(generator),0.0f};
            for (auto it = particles.begin(); it!=particles.end(); ++it)
            {
                if (norm(test- it->p)>3.0f){
                    boolean = false;
                }
            }
            boolean = !boolean;
        }
        std::cout<<"success"<<std::endl;
        const vec3 p0 = test;
        // Initial speed is random. (x,z) components are uniformly distributed along a circle.
        const float theta     = 2*3.14f*distrib(generator);
        const vec3 v0 = vec3( std::cos(theta), std::sin(theta), 0.0f);

        particles.push_back({p0,v0});
    }
    const float radius = 10.0f*distrib(generator);
    const float phi = 2*3.14f*distrib(generator);
    leader.p = vec3(radius*std::cos(phi),radius*std::sin(phi),0.0f);
    const float theta = 2*3.14f*distrib(generator);
    leader.v = vec3( std::cos(theta), std::sin(theta), 0.0f);

    const float radius2 = 10.0f*distrib(generator);
    const float phi2 = 2*3.14f*distrib(generator);
    predateur.p = vec3(radius2*std::cos(phi2),radius2*std::sin(phi2),0.0f);
    const float theta2 = 2*3.14f*distrib(generator);
    predateur.v = vec3( std::cos(theta2), std::sin(theta2), 0.0f);
    // leader = const vec3 p0 = {5*distrib(generator),5*distrib(generator),0};
}


void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    const float dt = timer.update(); // dt: Elapsed time between last frame
    set_gui(timer);

    const float m = 0.01f; // particle mass
    // Evolve position of particles
    // const vec3 g = {0.0f,0.0f,-9.81f};
    vec3 F_leader = vec3(0,0,0), F_predateur = vec3(0,0,0);
    for(particle_structure& particle : particles)
    {
        float alpha; // alignment factor

        vec3& p = particle.p;
        vec3& v = particle.v;

        //computing the force exerted on the current particle:
        vec3 F = vec3(0,0,0);

        for(particle_structure& particle2 : particles)
        {
            if (particle.distance(particle2)>0.01f && particle.distance(particle2)<20.0f){
                if (particle.distance(particle2)<2.0){
                    std::cout<<"cas 1"<<std::endl;
                    F+= 0.05f*(1/(particle.distance(particle2)*particle.distance(particle2)))*(particle.p-particle2.p);
                }
                else if (particle.distance(particle2)>2.5f){
                    std::cout<<"cas 3"<<std::endl;
                    F+=0.1f*particle.distance(particle2)*(particle2.p-particle.p);
                }
                else{
                    std::cout<<"cas 2"<<std::endl;
                    alpha = std::exp(-particle.distance(particle2));
                    F+=0.5f*alpha*(particle2.v-particle.v);
                }
            }
        }
        if (particle.distance(leader)>0.01f && particle.distance(leader)<20.0f){
            if (particle.distance(leader)<=2.0f){
                 const float radius = 10.0f*distrib(generator);
                 const float phi = 2*3.14f*distrib(generator);
                 leader.p = vec3(radius*std::cos(phi),radius*std::sin(phi),0.0f);
                 const float theta = 2*3.14f*distrib(generator);
                 leader.v = vec3( std::cos(theta), std::sin(theta), 0.0f);
            }
            if (particle.distance(leader)>2.0f){
                F+=0.01f*particle.distance(leader)*particle.distance(leader)*(leader.p-particle.p);
                F_leader -= 0.0001f*particle.distance(leader)*(leader.p-particle.p);
            }
        }
        F_predateur+=(p-predateur.p);
        // F -= std::exp(norm(v))*0.00001f*v;
        // std::cout<<F<<std::endl;
        // Numerical integration
        v = v+dt*F/m;
        if (norm(v)>50.0f) v = 0.5f*v;
        v = v-p/20;
        // v = std::exp(-norm(v)/1000)*v;
        // v =
        p = p + dt*v;
    }
    leader.v = leader.v+dt*F_leader/m;
    leader.p = leader.p + dt*leader.v;

    //calculer ici la vitesse et la position du prédateur.
    // Bounce particles too far away;
    float theta, cordrad, cordtan; vec3 rad, tan;
    for(auto it = particles.begin(); it!=particles.end(); ++it)
        if ((it->p.x)*(it->p.x)+(it->p.y)*(it->p.y)>25.0f){
            theta = std::atan(it->p.y/(it->p.x+0.0001f));
            rad = vec3(std::cos(theta),std::sin(theta),0.0f);
            tan = vec3(std::sin(theta),-std::cos(theta),0.0f);
            cordrad = dot(it->v,rad); cordtan = dot(it->v,tan);
            it->v = (-cordrad)*rad+(cordtan)*tan;
        }
    
    // Display particles
    for(particle_structure& particle : particles)
    {
        sphere.uniform_parameter.translation = particle.p;
        sphere.draw(shaders["mesh"], scene.camera);
    }
    // spherepredateur.uniform_parameter.translation = predateur.p;
    // spherepredateur.draw(shaders["mesh"], scene.camera);
    // sphere.uniform_parameter.translation = leader.p;
    // sphere.uniform_parameter.color = {1,0,0};
    // sphere.draw(shaders["mesh"], scene.camera);
    // Display ground
    ground.draw(shaders["mesh"],scene.camera);
        
}

float particle_structure::distance(particle_structure& particle){
    return std::sqrt((p.x-particle.p.x)*(p.x-particle.p.x)+(p.y-particle.p.y)*(p.y-particle.p.y)+(p.z-particle.p.z)*(p.z-particle.p.z));
}

static void set_gui(timer_event& timer)
{
    // Can set the speed of the animation
    float scale_min = 0.05f;
    float scale_max = 1.0f;
    ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer.scale, &scale_min, &scale_max, "%.2f s");

    // Start and stop animation
    if (ImGui::Button("Stop"))
        timer.stop();
    if (ImGui::Button("Start"))
        timer.start();
}


#endif
