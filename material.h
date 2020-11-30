#ifndef MATERIAL_H
#define MATERIAL_H

#include<ray.h>
#include<hittable.h>
#include<vec3.h>
#include<utility>
#include<texture.h>

class material_t
{
public:
    virtual std::tuple<bool,colour_t,ray_t> scatter(const ray_t &r_in,const hit_record_t &rec) const = 0;
    virtual colour_t emitted(double u,double v,const point3_t &p)const{ return {0,0,0}; }
};

class lambertian_t:public material_t
{
public:
    std::shared_ptr<texture_t> albedo;
    lambertian_t():lambertian_t(colour_t{0,0,0}){};
    lambertian_t(const colour_t &c):albedo(std::make_shared<solid_colour_t>(c)){}
    lambertian_t(std::shared_ptr<texture_t> a):albedo(a){}

    virtual std::tuple<bool,colour_t,ray_t> scatter(const ray_t &r_in,const hit_record_t &rec) const override
    {
        auto scatter_direction = rec.normal.unit() + random_in_unit_sphere().unit();
        auto attenuation = albedo->value(rec.u, rec.v, rec.p);
        if(scatter_direction.near_zero())
            return {true,attenuation,{rec.p, rec.normal,r_in.time()}};
        return {true,attenuation,{rec.p, scatter_direction,r_in.time()}};
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
        auto reflected = reflect(r_in.direction(),rec.normal).unit() + fuzz*random_in_unit_sphere().unit();;
        return {dot(reflected,rec.normal)>0,albedo,{rec.p, reflected,r_in.time()}};
    }
};

class dielectric_t : public material_t
{

public:
    double ir; // Index of Refraction
    colour_t attenuation;
    bool is_fresnel_reflectance;
    double fuzz;

    dielectric_t():dielectric_t(1,{1,1,1}){}
    dielectric_t(double index_of_refraction,colour_t attenuation,double fuzz=0,bool fresnel_ref=true) : ir(index_of_refraction),attenuation(attenuation),is_fresnel_reflectance(fresnel_ref) {}
    dielectric_t(colour_t attenuation):dielectric_t(1,attenuation){}
    dielectric_t(double index_of_refraction):dielectric_t(index_of_refraction,{1,1,1}){}


    virtual std::tuple<bool,colour_t,ray_t> scatter(const ray_t &r_in,const hit_record_t &rec) const override
    {
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3_t unit_direction = r_in.direction().unit();

        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3_t direction;

        if (cannot_refract || (is_fresnel_reflectance && reflectance(cos_theta, refraction_ratio) > rand_uniform()) )
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        direction = direction + fuzz*random_in_unit_sphere();
        return {true,attenuation,ray_t(rec.p,direction,r_in.time())};
    }
private:
    static double reflectance(double cosine, double ref_idx)
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

class diffuse_light_t : public material_t  
{
public:
    std::shared_ptr<texture_t> emit;
    diffuse_light_t(std::shared_ptr<texture_t> a) : emit(a) {}
    diffuse_light_t(colour_t c) : emit(std::make_shared<solid_colour_t>(c)) {}

    virtual std::tuple<bool, colour_t, ray_t> scatter(const ray_t &r_in, const hit_record_t &rec) const override
    {
        return {false, {}, {}};
    }
    virtual colour_t emitted(double u, double v, const point3_t &p) const
    {
        return emit->value(u, v, p);
    }
};

class material_test_t:public material_t
{
public:
    virtual std::tuple<bool,colour_t,ray_t> scatter(const ray_t &r_in,const hit_record_t &rec) const override
    {
        return {false,{},{}};
    }
    virtual colour_t emitted(double u,double v,const point3_t &p)const{ return (p.unit()+colour_t{1,1,1})*0.5; }
};

#endif