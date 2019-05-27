#pragma once

#include "../base_project/base_project.hpp"
#include "color.hpp"
struct Terrain{
    const size_t nb_points=1000; //Précision du tracé

    float l=10.0f; //Taille terrain (carré)
    float l_ile=60.0f; //Taille caractéristique de l'île
    float l_bord=10.0f; //Taille caractéristique de décroissance des bords.
    float profondeur_bord=10.0f; //Profondeur des bords
    int courbure_bord=2;
    float l_plage=5.0f;
    vcl::vec3 tr1,tr2,tr3; //Les 3 points du triangle qui représente l'île
    vcl::vec3 pas12,pas13,pas23;
    float h_montagne=5.0f; //Hauteur de la montagne centrale
    float l_montagne=2.0f;
    float alea_montagne=0.20f;
    vcl::vec2 centre1, centre2, centre3;
    int n_collines=0;
    float h_colline=0.0f;
    int n_geysers=0;
    int n_crevasses=0;
    int n_mares=0;
    vcl::mesh terrain;
    vcl::mesh_drawable terrain_drawable;
//    vcl::vec4 color_foret=vcl::vec4(159,129,57,0)*(1/255.0f);
    std::vector<vcl::vec4> colors_foret={{156,174,61,0},{202,149,82,0},{224,193,81,0}};
    std::vector<int> poids_foret={3,1,1};
    Color color_foret;
    std::vector<vcl::vec4> colors_plaine={{188,163,44,0},{98,135,37,0},{202,193,48,0}};
    std::vector<int> poids_plaine={2,5,1};
    Color color_plaine;
    std::vector<vcl::vec4> colors_montagne={{160,110,70,0},{100,75,30,0},{200,130,60,0},{255,0,0,0}};
    std::vector<int> poids_montagne={1,1,1,1};
    Color color_montagne;
    std::vector<vcl::vec4> colors_marais={{59,63,74,0},{33,45,25,0},{133,145,127,0}};
    std::vector<int> poids_marais={4,1,1};
    Color color_marais;
    std::vector<vcl::vec4> colors_plage={{239,209,165,0}};
    std::vector<int> poids_plage={1};
    Color color_plage;
    int randcolor=20;
    int l_rand=10;
    vcl::vec4 colorxy(float x, float y);
    GLuint foret_texture_id=0;
    GLuint plaine_texture_id=0;
    GLuint marais_texture_id=0;
    GLuint montagne_texture_id=0;
    std::vector<vcl::vec3> tree_position;
    //PERLIN PARAMETRES
    int perlin_it=50;
    float height = 0.2f;
    float scaling = 0.01f;
    int octave = 9;
    float persistency = 0.5f;
    int sensibilite_noise_cote=2; //Petite valeur=>grand bruit
    //CONSTRUCTEURS METHODES
    Terrain(float _l,float _l_ile,float _h_montagne,int _n_collines,float _h_colline,int _n_geysers,int _n_crevasses,int _n_mares);
    Terrain();
    void setPerlin(float height_,float scaling_,int octave_,float persistency_,int sensibilite_noise_cote_);
    void setPerlin(float height_,float scaling_,int octave_,float persistency_);
    void init_drawable();
    void reload();
    float evaluate_z(float x,float y);
    float dist_bord(float x,float y);
    int nearest(vcl::vec2 v);

private:
    void init_terrain();

};




