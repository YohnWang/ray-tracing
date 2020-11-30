#ifndef AARECT_H 
#define AARECT_H

#include<aabb.h>
#include<hittable.h>

class rect_t:public hittable_t
{
public:
    vec3_t n;
    point3_t min,max;
    std::shared_ptr<material_t> mat_ptr;
    double d;
    rect_t()=default;
    rect_t(vec3_t normal,point3_t a,point3_t b,std::shared_ptr<material_t> mat):n(normal),mat_ptr(mat)
    {
        min=vec3_t(fmin(a.x,b.x),fmin(a.y,b.y),fmin(a.z,b.z));
        max=vec3_t(fmax(a.x,b.x),fmax(a.y,b.y),fmax(a.z,b.z));
        d=-dot(n,min);
    }
    virtual std::pair<bool, hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override
    {
        auto denominator=dot(n,r.direction());
        if(denominator==0)
            return {false,{}};
        auto t=(-d-dot(n,r.origin()))/denominator;
        
        if(t<t_min || t> t_max)
            return {false,{}};
        auto p=r.at(t);
        if(is_in_rect(p)==false)
            return {false,{}};
        hit_record_t rec{};
        rec.p=p;
        rec.t=t;
        rec.mat_ptr=mat_ptr;
        rec.set_face_normal(r,n);
        rec.u=1;
        rec.v=1;
        return {true,rec};
    }
    virtual std::pair<bool, aabb_t> bounding_box(double time0,double time1)const
    {
        auto offset=n.unit()*0.0001;
        return {true,aabb_t(min-offset,max+offset)};
    }

    bool is_in_rect(const point3_t &p) const
    {
        constexpr auto e=1e-8;
        auto x_in=p.x>=min.x-e && p.x<=max.x+e;
        auto y_in=p.y>=min.y-e && p.y<=max.y+e;
        auto z_in=p.z>=min.z-e && p.z<=max.z+e;
        if( x_in && y_in && z_in )
            return true;
        else 
            return false;
    }
};

#endif