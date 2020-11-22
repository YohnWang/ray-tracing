#ifndef AABB_H
#define AABB_H

#include<vec3.h>
#include<ray.h>

class aabb_t
{
public:
    point3_t minimum;
    point3_t maximum;

    aabb_t()=default;
    aabb_t(point3_t a,point3_t b):minimum{a},maximum{b}{}

    point3_t min()const{return minimum;}
    point3_t max()const{return maximum;}

    bool hit(const ray_t &r,double t_min, double t_max)const
    {
        double A[3]={r.origin().x,r.origin().y,r.origin().z};
        double b[3]={r.direction().x,r.direction().y,r.direction().z};
        double x0[3]={minimum.x,minimum.y,minimum.z};
        double x1[3]={maximum.x,maximum.y,maximum.z};
        for(int i=0;i<3;i++)
        {
            auto invD=1.0/b[i];
            auto t0=(x0[i]-A[i])*invD;
            auto t1=(x1[i]-A[i])*invD;
            if(t0>t1)
                std::swap(t0,t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }
};

inline aabb_t surrounding_box(aabb_t box0,aabb_t box1)
{
    point3_t small(
        fmin(box0.min().x,box1.min().x),
        fmin(box0.min().y,box1.min().y),
        fmin(box0.min().z,box1.min().z)
    );
    point3_t big(
        fmax(box0.max().x,box1.max().x),
        fmax(box0.max().y,box1.max().y),
        fmax(box0.max().z,box1.max().z)
    );
    return aabb_t(small,big);
}

#endif