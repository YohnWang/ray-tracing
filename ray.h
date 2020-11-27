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

inline vec3_t reflect(const vec3_t &v, const vec3_t &n)
{
    return v - 2 * dot(v, n.unit()) * n.unit();
}

inline vec3_t refract(const vec3_t &uv, const vec3_t &n, double etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3_t r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3_t r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.len_squared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif