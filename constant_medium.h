#ifndef CONSTANT_MEDIUM_H 
#define CONSTANT_MEDIUM_H

#include<hittable.h>
#include<material.h>

class constant_medium_t:public hittable_t
{
public:
    std::shared_ptr<hittable_t> boundary;
    double neg_inv_density;
    std::shared_ptr<material_t> phase_function;

    constant_medium_t(std::shared_ptr<hittable_t> b, double d, std::shared_ptr<texture_t> a)
        : boundary(b),neg_inv_density(-1 / d),phase_function(std::make_shared<isotropic_t>(a)){}

    constant_medium_t(std::shared_ptr<hittable_t> b, double d, colour_t c)
        : boundary(b),neg_inv_density(-1 / d),phase_function(std::make_shared<isotropic_t>(c)){}

    virtual std::pair<bool, hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override
    {
        auto [is_hit1,rec1]=boundary->hit(r,-infinity,infinity);
        if(is_hit1==false)
            return {false,{}};
        auto [is_hit2,rec2]=boundary->hit(r,rec1.t+0.0001,infinity);
        if(is_hit2==false)
            return {false,{}};
        if (rec1.t < t_min)
            rec1.t = t_min;
        if (rec2.t > t_max)
            rec2.t = t_max;
        if (rec1.t >= rec2.t)
            return {false,{}};

        if (rec1.t < 0)
            rec1.t = 0;

        const auto ray_length = r.direction().len();
        const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        const auto hit_distance = neg_inv_density * log(rand_uniform());

        if (hit_distance > distance_inside_boundary)
            return {false,{}};

        hit_record_t rec{};
        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);
        rec.mat_ptr = phase_function;
        return {true,rec};
    }
    virtual std::pair<bool, aabb_t> bounding_box(double time0,double time1)const override
    {
        return boundary->bounding_box(time0, time1);
    }
};

#endif