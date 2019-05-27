#include "color.hpp"

using namespace std;
vcl::vec4 colorrd(vcl::vec4& color,int rdcolor){
    float c1=color[0]+((rand()%(rdcolor*2+1))-rdcolor)/255.0f;
    c1=min(c1,1.0f);
    c1=max(c1,0.0f);
    float c2=color[1]+((rand()%(rdcolor*2+1))-rdcolor)/255.0f;
    c2=min(c2,1.0f);
    c2=max(c2,0.0f);
    float c3=color[2]+((rand()%(rdcolor*2+1))-rdcolor)/255.0f;
    c3=min(c3,1.0f);
    c3=max(c3,0.0f);
    float c4=color[3]+((rand()%(rdcolor*2+1))-rdcolor)/255.0f;
    c4=min(c4,1.0f);
    c4=max(c4,0.0f);
    return vcl::vec4(c1,c2,c3,c4);
}
Color::Color()
{

}

Color::Color(std::vector<vcl::vec4>& colors_,int randcolor_)
{
    srand(time(NULL));
    randcolor=randcolor_;
    N=colors_.size();
    for(int i=0;i<N;i++)colors.push_back(colors_[i]/255.0f);
    for(int i=0;i<N;i++){poids.push_back(1);poids_sum.push_back(i);}
    poids_tot=N;
}
Color::Color(std::vector<vcl::vec4>& colors_, std::vector<int>& poids_,int randcolor_)
{
    srand(time(NULL));
    randcolor=randcolor_;
    assert(colors_.size()==poids_.size());
    N=colors_.size();
    for(int i=0;i<N;i++)colors.push_back(colors_[i]/255.0f);
    poids=poids_;
    poids_tot=0;
    for(int i=0;i<N;i++)poids_tot+=poids[i];
    int pdscourant=-1;
    for(int i=0;i<N;i++){
        pdscourant+=poids_[i];
        poids_sum.push_back(pdscourant);
    }
}

vcl::vec4 Color::rdcolor(){
    int v=rand()%poids_tot;
    int k=lower_bound(poids_sum.begin(),poids_sum.end(),v)-poids_sum.begin();
    return colorrd(colors[k],randcolor);
}
