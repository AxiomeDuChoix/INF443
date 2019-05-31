
#include "terrain.hpp"
#include <random>
#include <time.h>
#include <algorithm>

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;
float pi=std::acos(-1);
//USEFUL FONCTIONS
vec2 to2(vec3 v){
    return vec2(v[0],v[1]);
}
float theta(vec2 v){
    if(norm(v)<1e-6)return 0;
    return std::acos(v[0]/norm(v))+pi*(v[1]<0);
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
    init_collines();
    init_cote_fractale();
    init_terrain();
}

void Terrain::init_collines(){
    collines.resize(n_collines);
    std::default_random_engine generator;
    generator.seed(time(NULL));
    std::uniform_real_distribution<float> d_angle(0.0f,2*pi);
    std::uniform_real_distribution<float> d_h(-alea_h_colline/2,alea_h_colline/2);
    std::uniform_real_distribution<float> d_l(-alea_l_colline/2,alea_l_colline/2);
    for(int i=0;i<n_collines;i++){
        float r,alpha,h,l;
        alpha=d_angle(generator);
        std::uniform_real_distribution<float> d_rayon(l_montagne,l_ile/2*(1+std::abs(std::cos((alpha-pi/2)*3.0f/2)))-l_bord-2*l_plage-l_colline);
        r=d_rayon(generator);
        h=d_h(generator);
        l=d_l(generator);
        collines[i]={r*std::cos(alpha),r*std::sin(alpha),h_colline+h,l_colline+l};
    }
}

void Terrain::init_cote_fractale(){
    cote_fractale.resize(maillage_angle);
    float pas=1.0f/(maillage_angle-1);
    float perlin_m=0.0f,mult=persistencyc;
    for(int i=0;i<octavec;i++){
        perlin_m+=mult;
        mult*=persistencyc;
    }
    for(int i=0;i<maillage_angle;i++){
        cote_fractale[i]=heightc*(perlin(scalingc*i*pas,octavec,persistencyc,4.0f)-perlin_m/2);
    }
}

void Terrain::init_terrain(){
    terrain.connectivity.clear();
    terrain.position.resize(nb_points*(nb_points+1)/2+12);
    terrain.color.resize(nb_points*(nb_points+1)/2+12);
    srand(time(NULL));
    std::default_random_engine generator;
    generator.seed(time(NULL));
    std::uniform_real_distribution<float> distribution(0.0,alea_montagne);
    float facteur=1.0f/(nb_points-1);
    float m_perlint=0.0f,mult=persistencyt;
    for(int i=0;i<octavet;i++){m_perlint+=mult;mult*=persistencyt;}
    for(size_t x=0;x<nb_points;x++){
        for(size_t y=0;y<=x;y++){
            vec3 pos=tr3-x*pas13*facteur+y*pas12*facteur;
            terrain.color[x*(x+1)/2+y]=colorxy(pos[0],pos[1]);
            terrain.position[x*(x+1)/2+y]=pos;
            terrain.position[x*(x+1)/2+y]+={0,0,evaluate_z(pos[0],pos[1])+heightt*(perlin(scalingt*pos[0],scalingt*pos[1],octavet,persistencyt)-m_perlint)};
            float dmin=std::min(norm(to2(pos)-centre1),std::min(norm(to2(pos)-centre2),norm(to2(pos)-centre3)));
            dmin-=distribution(generator);
            float di=dist_bord(pos[0],pos[1]);
            if(dmin<l_montagne){
                float zplus=perlin(scalingm*pos[0],scalingm*pos[1],octavem,persistencym);
                terrain.position[x*(x+1)/2+y]+={0,0,((l_montagne-dmin)/l_montagne)*((l_montagne-dmin)/l_montagne)*zplus*heightm};
                terrain.color[x*(x+1)/2+y]=color_montagne.rdcolor();
            }
            if(di<l_bord+2*l_plage){
                float angle=theta(to2(pos));
                int k=(int)(maillage_angle*angle/(2*pi));
                terrain.position[x*(x+1)/2+y]+=pos*((l_bord+2*l_plage-di)/(l_bord+2*l_plage)*cote_fractale[k]*(std::abs(std::sin((angle-pi/2)*3.0f/2.0f))+0.1f));
            }

        }
    }
    for(size_t x=0;x<nb_points-1;x++){
        for(size_t y=0;y<=x;y++){
            size_t rang=x*(x+1)/2;
            const index3 triangle_1={rang+y,rang+x+1+y,rang+x+2+y};
            terrain.connectivity.push_back(triangle_1);
            if(x!=y){
                const index3 triangle_2={rang+y,rang+x+2+y,rang+1+y};
                terrain.connectivity.push_back(triangle_2);
            }
        }
    }
    int l=terrain.position.size()-12;
    terrain.position[l]={-distance_eau,distance_eau,hauteur_eau};
    terrain.position[l+1]={distance_eau,distance_eau,hauteur_eau};
    terrain.position[l+2]={distance_eau,-distance_eau,hauteur_eau};
    terrain.position[l+3]={-distance_eau,-distance_eau,hauteur_eau};
    terrain.position[l+4]={0,10*distance_eau,hauteur_eau};
    terrain.position[l+5]={10*distance_eau,0,hauteur_eau};
    terrain.position[l+6]={0,-10*distance_eau,hauteur_eau};
    terrain.position[l+7]={-10*distance_eau,0,hauteur_eau};
    terrain.position[l+8]={-10*distance_eau,10*distance_eau,hauteur_eau};
    terrain.position[l+9]={10*distance_eau,10*distance_eau,hauteur_eau};
    terrain.position[l+10]={10*distance_eau,-10*distance_eau,hauteur_eau};
    terrain.position[l+11]={-10*distance_eau,-10*distance_eau,hauteur_eau};
    terrain.connectivity.push_back({l+0,l+1,l+4});
    terrain.connectivity.push_back({l+1,l+2,l+5});
    terrain.connectivity.push_back({l+2,l+3,l+6});
    terrain.connectivity.push_back({l+3,l+0,l+7});
    terrain.connectivity.push_back({l+0,l+7,l+8});
    terrain.connectivity.push_back({l+0,l+8,l+4});
    terrain.connectivity.push_back({l+1,l+4,l+9});
    terrain.connectivity.push_back({l+1,l+9,l+5});
    terrain.connectivity.push_back({l+2,l+5,l+10});
    terrain.connectivity.push_back({l+2,l+10,l+6});
    terrain.connectivity.push_back({l+3,l+6,l+11});
    terrain.connectivity.push_back({l+3,l+11,l+7});
    terrain.color[l]=couleur_eau;
    terrain.color[l+1]=couleur_eau;
    terrain.color[l+2]=couleur_eau;
    terrain.color[l+3]=couleur_eau;
    terrain.color[l+4]=couleur_eau;
    terrain.color[l+5]=couleur_eau;
    terrain.color[l+6]=couleur_eau;
    terrain.color[l+7]=couleur_eau;
    terrain.color[l+8]=couleur_eau;
    terrain.color[l+9]=couleur_eau;
    terrain.color[l+10]=couleur_eau;
    terrain.color[l+11]=couleur_eau;
}
void Terrain::init_drawable(){
    terrain_drawable=terrain;
    terrain_drawable.uniform_parameter.shading.specular=specular;
    terrain_drawable.uniform_parameter.shading.ambiant=ambiant;
    terrain_drawable.uniform_parameter.shading.diffuse=diffuse;
}
void Terrain::setPerlinMontagne(float height_,float scaling_,int octave_,float persistency_){
    heightm=height_;
    scalingm=scaling_;
    octavem=octave_;
    persistencym=persistency_;
}
void Terrain::setPerlinTerrain(float height_,float scaling_,int octave_,float persistency_){
    heightt=height_;
    scalingt=scaling_;
    octavet=octave_;
    persistencyt=persistency_;
}
void Terrain::setPerlinCote(float height_,float scaling_,int octave_,float persistency_){
    heightc=height_;
    scalingc=scaling_;
    octavec=octave_;
    persistencyc=persistency_;
}
void Terrain::reload(){
    terrain_drawable.data_gpu.clear();
    init_cote_fractale();
    init_collines();
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
    for(int i=0;i<n_collines;i++){
        float d=norm(v-vec2(collines[i][0],collines[i][1]))/collines[i][3];
        z+=std::exp(-d*d)*collines[i][2]*(d<2.0f);
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
