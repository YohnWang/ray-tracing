#ifndef RAY_H
#define RAY_H
#include<vec3.h>

class ray_t
{
public:
    point3_t orig;
    vec3_t   dir;
    ray_t()=default;
    ray_t(const point3_t &origin,const vec3_t &direction):orig(origin),dir(direction){}
    point3_t origin()const{return orig;}
    vec3_t direction()const{return dir;}
    point3_t at(double t)const{return orig+dir*t;}
    
};

#endif