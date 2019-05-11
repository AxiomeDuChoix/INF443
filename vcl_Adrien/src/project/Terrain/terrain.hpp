#pragma once

#include "../base_project/base_project.hpp"

struct Terrain{
    const size_t nb_points=100; //Précision du tracé

    float l=10.0f; //Taille terrain (carré)
    float l_ile=5.0f; //Taille caractéristique de l'île
    vcl::vec3 tr1,tr2,tr3; //Les 3 points du triangle qui représente l'île
    vcl::vec3 pas12,pas13,pas23;
    float h_montagne=5.0f; //Hauteur de la montagne centrale
    int n_collines=0;
    float h_colline=0.0f;
    int n_geysers=0;
    int n_crevasses=0;
    int n_mares=0;
    vcl::mesh terrain;
//    vcl::mesh_drawable terrain_drawable;
    GLuint foret_texture_id=0;
    GLuint plaine_texture_id=0;
    GLuint marais_texture_id=0;
    GLuint montagne_texture_id=0;
    std::vector<vcl::vec3> tree_position;

    Terrain(float _l,float _l_ile,float _h_montagne,int _n_collines,float _h_colline,int _n_geysers,int _n_crevasses,int _n_mares);
    Terrain();

private:
    void init_terrain();

};




