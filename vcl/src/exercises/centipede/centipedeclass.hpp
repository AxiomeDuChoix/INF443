#pragma once

#include "../../exercises/base_exercise/base_exercise.hpp"

struct Centipede{ 
    // général: 
    vcl::mesh_drawable_hierarchy centipede;
    float scaling;
    // tete: 
    float r_head;
    // yeux:
    float r_eyes;
    // abdomen:
    int n_abdomen;
    int n_sections_abdomen;
    float r_abdomen; 
    float l_abdo;
    // pattes:
    int n_sous_pattes;
    int n_sections_pattes;
    float r_patte;
    float l_patte;
    float angle_pattes;

    Centipede();
    Centipede(float _scaling, float _r_head, float _r_eyes, 
              int _n_abdomen, int _n_sections_abdomen, float _r_abdomen, float _l_abdo,
              int _n_sous_pattes, int _n_sections_pattes, float _r_patte, float _l_patte, float angle_pattes);
    vcl::mesh_drawable_hierarchy create_centipede();
    void display_centipede(std::map<std::string,GLuint>& shaders, scene_structure& scene);

private:
    vcl::mesh section_cone(float radius1, float radius2, float height, float offset);
    vcl::mesh create_abdomen_elementaire(int nb_pots, float r, float l, float& l_tot);
    vcl::mesh patte(int nb_pots, float r, float l, float& l_tot);
};
