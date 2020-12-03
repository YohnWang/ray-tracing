#ifndef PLANE_H 
#define PLANE_H

#include<hittable.h>

class plane_t:public hittable_t
{
public:
    
    point3_t center;
    vec3_t width;
    vec3_t height;
    std::shared_ptr<material_t> mat_ptr;
    vec3_t n;
    double d;

    plane_t()=default;
    plane_t(point3_t center,vec3_t width,vec3_t height,std::shared_ptr<material_t> mat):center(center),width(width),height(height),mat_ptr(mat)
    {
        n=cross(width,height); 
        d=-dot(n,center);
    }

    virtual std::pair<bool, hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override
    {
        auto denominator=dot(n,r.direction());
        if(denominator==0)
            return {false,{}};
        auto t=(-d-dot(n,r.origin()))/denominator;
        
        if(t<t_min || t> t_max)
            return {false,{}};

        hit_record_t rec{};
        rec.p=r.at(t);
        rec.t=t;
        rec.mat_ptr=mat_ptr;
        rec.set_face_normal(r,n);
        auto v=rec.p-center;
        rec.u=dot(v,width.unit());
        rec.v=dot(v,height.unit());
        return {true,rec};
    }
    virtual std::pair<bool, aabb_t> bounding_box(double time0,double time1)const override
    {
        return {false,{}};
    }

    void move(vec3_t direction)
    {
        center+=direction;
        d=-dot(n,center);
    }

    void rotate_y(double theta)
    {
        theta=pi/180.0*theta;
        auto cos_theta=cos(theta);
        auto sin_theta=sin(theta);
        width=vec3_t(
            dot(vec3_t(cos_theta,0,sin_theta),width),
            dot(vec3_t(0,1,0),width),
            dot(vec3_t(-sin_theta,0,cos_theta),width)
        );
        height=vec3_t(
            dot(vec3_t(cos_theta,0,sin_theta),height),
            dot(vec3_t(0,1,0),height),
            dot(vec3_t(-sin_theta,0,cos_theta),height)
        );
        //n=cross(width,height); 
        n=vec3_t(
            dot(vec3_t(cos_theta,0,sin_theta),n),
            dot(vec3_t(0,1,0),n),
            dot(vec3_t(-sin_theta,0,cos_theta),n)
        );
        d=-dot(n,center);
    }

    void rotate(point3_t p,vec3_t axis,double theta)
    {
        auto D= -dot(p,n);
        auto distance=(dot(axis,center)+D)/(axis.len());
        auto cos_theta=cos(theta);
    }

};

#endif