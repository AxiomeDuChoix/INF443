#include "centipedeclass.hpp"
#include <cmath>
#include <random>
#include <time.h>

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;
Centipede::Centipede()
{
    scaling = 0.2f;
    r_head = 0.2f;
    r_eyes = 0.05f;
    n_abdomen = 10;
    n_sections_abdomen = 10;
    r_abdomen = 0.5f;
    l_abdo = 0.1f;
    n_sous_pattes = 10;
    n_sections_pattes = 10;
    r_patte = 0.15f;
    l_patte = 0.01f;
    angle_pattes = 3.1415f/3;
    centipede = create_centipede();
}
Centipede::Centipede(float _scaling, float _r_head, float _r_eyes, 
              int _n_abdomen, int _n_sections_abdomen, float _r_abdomen, float _l_abdo,
              int _n_sous_pattes, int _n_sections_pattes, float _r_patte, float _l_patte, float _angle_pattes)
{
    scaling = _scaling;
    r_head = _r_head;
    r_eyes = _r_eyes;
    n_abdomen = _n_abdomen;
    n_sections_abdomen = _n_sections_abdomen;
    r_abdomen = _r_abdomen;
    l_abdo = _l_abdo;
    n_sous_pattes = _n_sous_pattes;
    n_sections_pattes = _n_sections_pattes;
    r_patte = _r_patte;
    l_patte = _l_patte;
    angle_pattes = _angle_pattes;
    centipede = create_centipede();
}

mesh Centipede::section_cone(float radius1, float radius2, float height, float offset){
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
mesh Centipede::create_abdomen_elementaire(int nb_pots, float r, float l, float& l_tot){
    mesh m;
    int n = nb_pots/2;
    float fr1, fr2, fl = 1, offset = 0, backoffset = -l;
    for (int i = 0; i<n;i++){
        fr1 = r-i*(r/2)/n;
        fr2 = r-(i+1)*(r/2)/n;
        l_tot+=2*fl*l;
        mesh potfront = section_cone(fr1*r,fr2*r,fl*l,offset);
        mesh potback = section_cone(fr2*r,fr1*r,fl*l,backoffset);
        m.push_back(potfront);
        m.push_back(potback);
        offset+=fl*l;
        backoffset-=fl*l/2;
        fl/=2;
    }
    return m;
}
mesh Centipede::patte(int nb_pots, float r, float l, float& l_tot){
    mesh m;
    int n = nb_pots/2;
    float fr1, fr2, fl = 1, offset = 0, backoffset = -l;
    for (int i = 0; i<n;i++){
        fr1 = r-i*(r/2)/n;
        fr2 = r-(i+1)*(r/2)/n;
        l_tot+=2*fl*l;
        mesh potfront = section_cone(fr1*r,fr2*r,fl*l,offset);
        mesh potback = section_cone(fr2*r,fr1*r,fl*l,backoffset);
        m.push_back(potfront);
        m.push_back(potback);
        offset+=fl*l;
        backoffset-=fl*l/2;
        fl/=2;
    }
    return m;
}
mesh_drawable_hierarchy Centipede::create_centipede(){
    mesh_drawable_hierarchy hierarchy;
    hierarchy.scaling = scaling;
    // head
    mesh_drawable head = mesh_primitive_sphere(r_head,{0,0,0},40,40);
    head.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
    hierarchy.add_element(head, "head", "root");
    // eye
    mesh_drawable eye = mesh_primitive_sphere(r_eyes,{0,0,0},20,20);
    eye.uniform_parameter.color = {0,0,0};
    hierarchy.add_element(eye, "eye_left", "head",{r_head/3,r_head/2,r_head/1.5f});
    hierarchy.add_element(eye, "eye_right", "head",{-r_head/3,r_head/2,r_head/1.5f});
    // corps: 
    float rayon, l_tot;
    std::string abdo = "abdomen", parent = "head";
    for(int i = 0; i<n_abdomen/2;i++){
        l_tot = 0;
        rayon = 3*r_abdomen/4+i*(r_abdomen/2)/n_abdomen;
        mesh_drawable abdomen = create_abdomen_elementaire(n_sections_abdomen, rayon, l_abdo, l_tot);
        abdomen.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
        hierarchy.add_element(abdomen,abdo+std::to_string(i+1),parent,{0.0f,0.0f,-l_tot});
        parent = abdo+std::to_string(i+1);
    }
    for(int i = 0; i<n_abdomen/2;i++){
        l_tot = 0;
        rayon = r_abdomen-i*(r_abdomen/2)/n_abdomen;
        mesh_drawable abdomen = create_abdomen_elementaire(n_sections_abdomen, rayon, l_abdo, l_tot);
        abdomen.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
        hierarchy.add_element(abdomen,abdo+std::to_string(n_abdomen/2+i+1),parent,{0.0f,0.0f,-l_tot});
        parent = abdo+std::to_string(n_abdomen/2+i+1);
    }
    // pattes à gauche:
    std::string string_patte = "patte";
    float rayon_abdomen_local;
    for(int k = 0; k<2*(n_abdomen/2);k++){
        if (k<n_abdomen/2){
            rayon_abdomen_local = 3*r_abdomen/4+k*(r_abdomen/2)/n_abdomen;
        }
        else rayon_abdomen_local = r_abdomen-k*(r_abdomen/2)/n_abdomen;
        parent = abdo+std::to_string(k+1);
        rayon = 3*r_patte/4;
        l_tot = 0;
        mesh_drawable patte_temp0 = patte(n_sections_pattes,rayon,l_patte,l_tot);
        patte_temp0.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
        hierarchy.add_element(patte_temp0,string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(1),parent,{-0.4f*std::cos(angle_pattes)*rayon_abdomen_local,-0.4f*std::sin(angle_pattes)*rayon_abdomen_local,0.0f});
        
        parent = string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(1);
        for(int i = 1; i<n_sous_pattes/2;i++){
            l_tot = 0;
            rayon = 3*r_patte/4+i*(r_patte/2)/n_sous_pattes;
            mesh_drawable patte_temp = patte(n_sections_pattes,rayon,l_patte,l_tot);
            patte_temp.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
            hierarchy.add_element(patte_temp,string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(i+1),parent,{0.0f,0.0f,-l_tot});
            parent = string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(i+1);
        }
        for(int i = 0; i<n_sous_pattes/2;i++){
            l_tot = 0;
            rayon = r_patte-i*(r_patte/2)/n_sous_pattes;
            mesh_drawable patte_temp = patte(n_sections_pattes,rayon,l_patte,l_tot);
            patte_temp.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
            hierarchy.add_element(patte_temp,string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(n_sous_pattes/2+i+1),parent,{0.0f,0.0f,-l_tot});
            parent = string_patte+"gauche"+std::to_string(k+1)+","+std::to_string(n_sous_pattes/2+i+1);
        }
        // pattes à droite
        parent = abdo+std::to_string(k+1);
        patte_temp0.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
        hierarchy.add_element(patte_temp0,string_patte+"droite"+std::to_string(k+1)+","+std::to_string(1),parent,{0.4f*std::cos(angle_pattes)*rayon_abdomen_local,-0.4f*std::sin(angle_pattes)*rayon_abdomen_local,0.0f});
        parent = string_patte+"droite"+std::to_string(k+1)+","+std::to_string(1);
        for(int i = 1; i<n_sous_pattes/2;i++){
            l_tot = 0;
            rayon = 3*r_patte/4+i*(r_patte/2)/n_sous_pattes;
            mesh_drawable patte_temp = patte(n_sections_pattes,rayon,l_patte,l_tot);
            patte_temp.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
            hierarchy.add_element(patte_temp,string_patte+"droite"+std::to_string(k+1)+","+std::to_string(i+1),parent,{0.0f,0.0f,l_tot});
            parent = string_patte+"droite"+std::to_string(k+1)+","+std::to_string(i+1);
        }
        for(int i = 0; i<n_sous_pattes/2;i++){
            l_tot = 0;
            rayon = r_patte-i*(r_patte/2)/n_sous_pattes;
            mesh_drawable patte_temp = patte(n_sections_pattes,rayon,l_patte,l_tot);
            patte_temp.uniform_parameter.color = {102.0f/100,75.0f/100,38.0f/100};
            hierarchy.add_element(patte_temp,string_patte+"droite"+std::to_string(k+1)+","+std::to_string(n_sous_pattes/2+i+1),parent,{0.0f,0.0f,l_tot});
            parent = string_patte+"droite"+std::to_string(k+1)+","+std::to_string(n_sous_pattes/2+i+1);
        }

    }
    return hierarchy;
}