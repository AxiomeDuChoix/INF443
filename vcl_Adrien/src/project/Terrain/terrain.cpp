
#include "terrain.hpp"
#include <random>
#include <time.h>

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;
Terrain::Terrain()
{
    tr1={-0.5f*1.732f*l_ile,-0.5f*l_ile,0};
    tr2={0.5f*1.732f*l_ile,-0.5f*l_ile,0};
    tr3={0.0f,l_ile,0.0f};
    pas12=tr2-tr1;
    pas13=tr3-tr1;
    pas23=tr3-tr2;
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
    init_terrain();

}
void Terrain::init_terrain(){
    terrain.connectivity.clear();
    terrain.position.resize(nb_points*(nb_points+1)/2);
    float facteur=1.0f/(nb_points-1);
    for(size_t x=0;x<nb_points;x++){
        for(size_t y=0;y<=x;y++){
            vec3 pos=tr3-x*pas13*facteur+y*pas12*facteur;
            terrain.position[x*(x+1)/2+y]=pos;
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



