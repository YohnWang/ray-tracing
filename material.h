#ifndef MATERIAL_H
#define MATERIAL_H

#include<ray.h>
#include<hittable.h>
#include<vec3.h>
#include<utility>


class material_t
{
public:
    virtual std::tuple<bool,colour_t,ray_t> scatter(const ray_t &r_in,const hit_record_t &rec) const = 0;
};

class lambertian_t:public material_t
{
public:
    colour_t albedo;
    lambertian_t():lambertian_t({0,0,0}){};
    lambertian_t(const colour_t &albedo):albedo(albedo){}

    virtual std::tuple<bool,colour_t,ray_t> scatter(const ray_t &r_in,const hit_record_t &rec) const override
    {
        auto scatter_direction = rec.normal.unit() + random_in_unit_sphere().unit();
        if(scatter_direction.near_zero())
            return {true,albedo,{rec.p, rec.normal}};
        return {true,albedo,{rec.p, scatter_direction}};
    }
};

class metal_t:public material_t
{
public:
    colour_t albedo;
    double   fuzz;

    metal_t():metal_t({1,1,1}){};
    metal_t(const colour_t &albedo,double fuzz=0):albedo(albedo),fuzz(fuzz){}

    virtual std::tuple<bool,colour_t,ray_t> scatter(const ray_t &r_in,const hit_record_t &rec) const override
    {
        auto reflected = reflect(r_in.direction(),rec.normal) + fuzz*random_in_unit_sphere().unit();;
        return {dot(reflected,rec.normal)>0,albedo,{rec.p, reflected}};
    }
};

class dielectric_t : public material_t
{

public:
    double ir; // Index of Refraction

    dielectric_t(double index_of_refraction) : ir(index_of_refraction) {}

    virtual std::tuple<bool,colour_t,ray_t> scatter(const ray_t &r_in,const hit_record_t &rec) const override
    {
        
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3_t unit_direction = r_in.direction().unit();
        //vec3_t refracted = refract(unit_direction, rec.normal, refraction_ratio);

        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3_t direction;

        if (cannot_refract)
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        return {true,{1,1,1},ray_t(rec.p,direction)};
    }
};

#endif