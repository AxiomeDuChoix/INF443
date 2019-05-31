
#include "eau.hpp"



// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;

Eau::Eau()
{
    surface_cpu.position.resize(N*N);
    surface_cpu.connectivity.clear();
    surface_cpu.texture_uv.clear();
    surface_cpu.normal.clear();
    surface_cpu.color.clear();

    float pas=1.0f/(N-1);
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            surface_cpu.position[i*N+j]={-taille/2.0f+i*taille*pas,-taille/2.0f+j*pas*taille,0*std::cos(3.14159f*nperiodesx*pas*j)};
            if((i<N-1)&&(j<N-1)){
                surface_cpu.connectivity.push_back({i*N+j,(i+1)*N+j+1,i*N+j+1});
                surface_cpu.connectivity.push_back({i*N+j,(i+1)*N+j,(i+1)*N+j+1});
            }
            surface_cpu.texture_uv.push_back({pas*i,pas*j});
            surface_cpu.normal.push_back(normalize(vec3(0.0f,-std::sin(3.14159f*nperiodesx*pas*j),1.0f)));
//            surface_cpu.color.push_back({std::abs(std::sin(3.14159f*nperiodesx*pas*j)),std::abs(std::sin(3.14159f*nperiodesx*pas*j)),std::abs(std::sin(3.14159f*nperiodesx*pas*j)),1});
        }
    }
    surface_cpu.fill_empty_fields();
}
void Eau::init_draw(){
    texture_id = texture_gpu( image_load_png(cheminTexture) );
    surface = surface_cpu;
    surface.uniform_parameter.shading = {1,0,1}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture

}



