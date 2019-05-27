
#include "terrain.hpp"
#include <random>
#include <time.h>
#include <algorithm>

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;
//USEFUL FONCTIONS
vec2 to2(vec3 v){
    return vec2(v[0],v[1]);
}

vec2 projete_ortho(vec2 v, vec2 a){
    a/=norm(a);
    return v-a*dot(v,a);
}
vec2 projete_ortho(vec2 v, vec3 a){
    vec2 a2(a[0],a[1]);
    a2/=norm(a2);
    return v-a2*dot(v,a2);
}

//
Terrain::Terrain()
{
    tr1={-0.5f*1.732f*l_ile,-0.5f*l_ile,0};
    tr2={0.5f*1.732f*l_ile,-0.5f*l_ile,0};
    tr3={0.0f,l_ile,0.0f};
    pas12=tr2-tr1;
    pas13=tr3-tr1;
    pas23=tr3-tr2;
    centre1=to2(tr1)*l_montagne/l_ile;
    centre2=to2(tr2)*l_montagne/l_ile;
    centre3=to2(tr3)*l_montagne/l_ile;
    std::cout<<"OK"<<colors_foret.size()<<'\n';
    color_foret=Color(colors_foret,poids_foret,randcolor);
    color_montagne=Color(colors_montagne,poids_montagne,randcolor);
    color_plaine=Color(colors_plaine,poids_plaine,randcolor);
    color_marais=Color(colors_marais,poids_marais,randcolor);
    color_plage=Color(colors_plage,poids_plage,randcolor);
    init_terrain();
}

Terrain::Terrain(float _l,float _l_ile,float _h_montagne,int _n_collines,float _h_colline,int _n_geysers,int _n_crevasses,int _n_mares)
{
    l=_l;
    l_ile=_l_ile;
    h_montagne=_h_montagne;
    n_collines=_n_collines;
    h_colline=_h_colline;
    n_geysers=_n_geysers;
    n_crevasses=_n_crevasses;
    n_mares=_n_mares;
    tr1={-0.5f*1.732f*l_ile,-0.5f*l_ile,0};
    tr2={0.5f*1.732f*l_ile,-0.5f*l_ile,0};
    tr3={0.0f,l_ile,0.0f};
    pas12=tr2-tr1;
    pas13=tr3-tr1;
    pas23=tr3-tr2;
    centre1=to2(tr1)*l_montagne/l_ile;
    centre2=to2(tr2)*l_montagne/l_ile;
    centre3=to2(tr3)*l_montagne/l_ile;
    color_foret=Color(colors_foret,poids_foret,randcolor);
    color_montagne=Color(colors_montagne,poids_montagne,randcolor);
    color_plaine=Color(colors_plaine,poids_plaine,randcolor);
    color_marais=Color(colors_marais,poids_marais,randcolor);
    color_plage=Color(colors_plage,poids_plage,randcolor);
    init_terrain();

}
void Terrain::init_terrain(){
    terrain.connectivity.clear();
    terrain.position.resize(nb_points*(nb_points+1)/2);
    terrain.color.resize(nb_points*(nb_points+1)/2);
    srand(time(NULL));
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0,alea_montagne);
    float facteur=1.0f/(nb_points-1);
    for(size_t x=0;x<nb_points;x++){
        for(size_t y=0;y<=x;y++){
            vec3 pos=tr3-x*pas13*facteur+y*pas12*facteur;
            terrain.color[x*(x+1)/2+y]=colorxy(pos[0],pos[1]);
            terrain.position[x*(x+1)/2+y]=pos;
            terrain.position[x*(x+1)/2+y]+={0,0,evaluate_z(pos[0],pos[1])};
            float dmin=std::min(norm(to2(pos)-centre1),std::min(norm(to2(pos)-centre2),norm(to2(pos)-centre3)));
            dmin-=distribution(generator);
            float di=dist_bord(pos[0],pos[1]);
            if(dmin<l_montagne){
                float zplus=0;
                for(int i=0;i<perlin_it;i++){
                    zplus+=(1/(1<<i))*perlin(l_montagne*pos[0]*(1<<i),l_montagne*pos[1]*(1<<i),octave,persistency);
                }
                terrain.position[x*(x+1)/2+y]+={0,0,(l_montagne-dmin)/l_montagne*zplus};
                terrain.color[x*(x+1)/2+y]=color_montagne.rdcolor();
            }
//            else if (di>0.8f*l_bord&&di<l_bord*1.2f){
//                float fac=(di)/l_bord*l_ile/nb_points;
//                terrain.position[x*(x+1)/2+y]+={fac*(2*(rand()%2)-1)*perlin(scaling*pos[0],octave,persistency),fac*(2*(rand()%2)-1)*perlin(scaling*pos[1],octave,persistency),0};
//            }

        }
    }
    for(size_t x=0;x<nb_points-1;x++){
        for(size_t y=0;y<=x;y++){
            size_t rang=x*(x+1)/2;
            const index3 triangle_1={rang+y,rang+x+1+y,rang+x+2+y};
            terrain.connectivity.push_back(triangle_1);
            if(x!=y){
                const index3 triangle_2={rang+y,rang+1+y,rang+x+2+y};
                terrain.connectivity.push_back(triangle_2);
            }
        }
    }
}
void Terrain::init_drawable(){
    terrain_drawable=terrain;
//    terrain_drawable.uniform_parameter.shading.specular=0.0f;
//    terrain_drawable.uniform_parameter.shading.ambiant=0.0f;
//    terrain_drawable.uniform_parameter.shading.diffuse=0.0f;
}
void Terrain::setPerlin(float height_,float scaling_,int octave_,float persistency_,int sensibilite_noise_cote_){
    height=height_;
    scaling=scaling_;
    octave=octave_;
    persistency=persistency_;
    sensibilite_noise_cote=sensibilite_noise_cote_;
}
void Terrain::setPerlin(float height_,float scaling_,int octave_,float persistency_){
    height=height_;
    scaling=scaling_;
    octave=octave_;
    persistency=persistency_;
}
void Terrain::reload(){
    terrain_drawable.data_gpu.clear();
    init_terrain();
    init_drawable();
}
float Terrain::evaluate_z(float x, float y){
    vec2 v(x,y);
    float z=0.0f;
    float di=dist_bord(x,y);
    if(di<=l_bord){
        z-=profondeur_bord*(std::exp(-courbure_bord*di*di/(l_bord*l_bord))-std::exp(-courbure_bord));
    }

    const float d1=norm(v-centre1)/l_montagne;
    const float d2=norm(v-centre2)/l_montagne;
    const float d3=norm(v-centre3)/l_montagne;
    float dmin=std::min(std::min(d1,d2),d3);
//    float fac=l_montagne/l_ile;
    float a1=std::exp(-d1*d1*4);
    float a2=std::exp(-d2*d2*4);
    float a3=std::exp(-d3*d3*4);
    z+=h_montagne/2*a1*(dmin<1.0f);
    z+=h_montagne/2*a2*(dmin<1.0f);
    z+=h_montagne/2*a3*(dmin<1.0f);

//    const float d=norm(v)/l_montagne;
//    float a=std::exp(-d*d),b=std::exp(-d*d*4);
//    z+=2*h_montagne*(a*a-0.8f*b)*(d<1);


    return z;
}
float Terrain::dist_bord(float x, float y){
    vec2 v(x,y);
    float a=norm(projete_ortho(v,pas12)-to2((tr1+tr2)/2));
    float b=norm(projete_ortho(v,pas13)-to2((tr1+tr3)/2));
    float c=norm(projete_ortho(v,pas23)-to2((tr2+tr3)/2));
    return std::min(a,std::min(b,c));
}
int Terrain::nearest(vec2 v){
    if(std::min(norm(v-centre1),std::min(norm(v-centre2),norm(v-centre3)))<l_montagne)return 4;
    float di=dist_bord(v[0],v[1]);
    if(di<l_bord+l_plage)return 5;
    float d1=norm(to2(tr1)-v);
    float d2=norm(to2(tr2)-v);
    float d3=norm(to2(tr3)-v);
    float mini=std::min(d1,std::min(d2,d3));
    if(std::abs(mini-d1)<1e-6)return 1;
    else if(std::abs(mini-d2)<1e-6)return 2;
    else return 3;
}
vec4 Terrain::colorxy(float x, float y){
    vec2 v(x,y);
    float d_plage=dist_bord(v[0],v[1])*(100-l_rand+rand()%(2*l_rand+1))/100.0f;
    if (d_plage<l_bord+l_plage)return color_plage.rdcolor();
    float d_foret=norm(to2(tr1/2.0f)-v)*(100-l_rand+rand()%(2*l_rand+1));
    float d_plaine=norm(to2(tr2/2.0f)-v)*(100-l_rand+rand()%(2*l_rand+1));
    float d_marais=norm(to2(tr3/2.0f)-v)*(100-l_rand+rand()%(2*l_rand+1));
    float d=std::min(std::min(d_foret,d_plaine),d_marais);
    if(abs(d-d_foret)<1e-6)return color_foret.rdcolor();
    else if(abs(d-d_plaine)<1e-6)return color_plaine.rdcolor();
    else if(abs(d-d_marais)<1e-6)return color_marais.rdcolor();
}
