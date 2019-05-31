#pragma once

#include <bits/stdc++.h>
#include "../base_project/base_project.hpp"

struct Color{
    int N;//nb couleurs;
    int randcolor;
    std::vector<vcl::vec4> colors;
    std::vector<int> poids;
    std::vector<int> poids_sum;
    int poids_tot;
    Color();
    Color(std::vector<vcl::vec4>& colors_,int randcolor_);
    Color(std::vector<vcl::vec4>& colors_,std::vector<int>& poids_,int randcolor_);
    vcl::vec4 rdcolor();
};
