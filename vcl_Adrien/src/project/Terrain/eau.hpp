#pragma once

#include "../../project/base_project/base_project.hpp"

#ifdef PROJECT_EAU

// Stores some parameters that can be set from the GUI
struct Eau{
    int N=40; //N*N= nbpoints
    std::string cheminTexture="data/eau_texture.png";
    float taille=10.0f;
    int nperiodesx=3;
    vcl::mesh surface_cpu;
    vcl::mesh_drawable surface;
    GLuint texture_id;
    Eau();
    void init_draw();
};

#endif


