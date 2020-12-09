#ifndef RAY_COLOUR_H 
#define RAY_COLOUR_H
#include<ray.h>
#include<hittable.h>

class ray_colour_t
{
public:
    const hittable_list_t &world;
    colour_t background;
    double min_albedo;
    ray_colour_t(const hittable_list_t &world,const colour_t &background={0,0,0},double min_albedo=0.001):world(world),background(background),min_albedo(min_albedo)
    {

    }

    colour_t operator()(const ray_t &r,colour_t albedo={1,1,1},int depth=50)
    {
        if(depth<=0 || std::max(albedo.x,std::max(albedo.y,albedo.z))<min_albedo)
            return {0,0,0};
        auto [is_hit,rec]=world.hit(r, 0.001, infinity);
        if(is_hit==false)
            return background;

        auto [is_reflect, attenuation, scattered] = rec.mat_ptr->scatter(r, rec);
        auto emitted=rec.mat_ptr->emitted(rec.u,rec.v,rec.p);
        if (is_reflect)
            return emitted + attenuation * (*this)(scattered,albedo*attenuation, depth - 1);
        else
            return emitted;
    }
};

#endif