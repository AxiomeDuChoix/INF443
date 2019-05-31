#pragma once

#include "../base_project/base_project.hpp"
#include "color.hpp"
struct Terrain{
    const size_t nb_points=400; //Précision du tracé
    float ambiant=1.0f,diffuse=0,specular=0;
    float l_ile=10.0f; //Taille caractéristique de l'île
    float l_bord=1.0f; //Taille caractéristique de décroissance des bords.
    float profondeur_bord=3.0f; //Profondeur des bords
    int courbure_bord=3;
    float l_plage=1.0f;
    vcl::vec3 tr1,tr2,tr3; //Les 3 points du triangle qui représente l'île
    vcl::vec3 pas12,pas13,pas23;
    float h_montagne=2.0f; //Hauteur de la montagne centrale
    float l_montagne=0.50f;
    float alea_montagne=0.50f;
    vcl::vec2 centre1, centre2, centre3;
    std::vector<vcl::vec4> collines; //x,y,h,l
    int n_collines=30;
    float h_colline=0.20f;
    float l_colline=0.2f;
    float alea_h_colline=0.02f;
    float alea_l_colline=0.05f;
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
    std::vector<int> poids_montagne={10,10,10,1};
//    std::vector<vcl::vec4> colors_montagne={{255,255,255,255}};
//    std::vector<int> poids_montagne={1};
    Color color_montagne;
    std::vector<vcl::vec4> colors_marais={{59,63,74,0},{33,45,25,0},{133,145,127,0}};
    std::vector<int> poids_marais={4,1,1};
    Color color_marais;
    std::vector<vcl::vec4> colors_plage={{239,209,165,0}};
    std::vector<int> poids_plage={1};
    Color color_plage;
    int randcolor=5;
    int l_rand=10;
    vcl::vec4 colorxy(float x, float y);
    GLuint foret_texture_id=0;
    GLuint plaine_texture_id=0;
    GLuint marais_texture_id=0;
    GLuint montagne_texture_id=0;
    std::vector<vcl::vec3> tree_position;
    //PERLIN PARAMETRES
    //Montagnes
    float heightm = 0.2f;
    float scalingm = 0.50f;
    int octavem = 9;
    float persistencym = 0.4f;
    //Terrain
    float heightt = 0.07f;
    float scalingt = 0.40f;
    int octavet = 9;
    float persistencyt = 0.55f;
    //Cote
    int maillage_angle=100;
    std::vector<float> cote_fractale;
    float heightc = 0.8f;
    float scalingc = 7.0f;
    int octavec = 9;
    float persistencyc = 0.55f;
    //CONSTRUCTEURS METHODES
    Terrain();
    void setPerlinMontagne(float height_,float scaling_,int octave_,float persistency_);
    void setPerlinTerrain(float height_,float scaling_,int octave_,float persistency_);
    void setPerlinCote(float height_,float scaling_,int octave_,float persistency_);
    void init_drawable();
    void reload();
    float evaluate_z(float x,float y);
    float dist_bord(float x,float y);
    int nearest(vcl::vec2 v);
    void init_terrain();
    void init_cote_fractale();
    void init_collines();
};




