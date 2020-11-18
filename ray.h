#ifndef RAY_H
#define RAY_H
#include<vec3.h>

class ray_t
{
public:
    point3_t orig;
    vec3_t   dir;
    double   tm;
    ray_t()=default;
    ray_t(const point3_t &origin,const vec3_t &direction,double tm):orig(origin),dir(direction),tm(tm){}
    point3_t origin()const{return orig;}
    vec3_t direction()const{return dir;}
    point3_t at(double t)const{return orig+dir*t;}
    double time()const{return tm;}
};

#endif