#ifndef SPHERE_H
#define SPHERE_H
#include<aabb.h>
#include<material.h>
#include<hittable.h>
#include<ray.h>
#include<memory>

class sphere_t:public hittable_t
{
public:
    point3_t center;
    double   radius;
    std::shared_ptr<material_t> mat_ptr;

    sphere_t()=default;
    sphere_t(point3_t center,double radius,std::shared_ptr<material_t> m=std::make_shared<lambertian_t>()):center(center),radius(radius),mat_ptr(m){}

    virtual std::pair<bool,hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override
    {
        auto AC = r.origin() - center;
        auto a = dot(r.direction(), r.direction());
        auto b = 2 * dot(r.direction(), AC);
        auto c = dot(AC, AC) - radius * radius;
        auto discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
            return {false, {}};

        // Find the nearest root that lies in the acceptable range.
        auto sqrtd = sqrt(discriminant);

        auto root = (-b - sqrtd) / (2 * a);
        if (root < t_min || t_max < root)
        {
            root = (-b + sqrtd) / (2 * a);
            if (root < t_min || t_max < root)
                return {false, {}};
        }
        hit_record_t rec;
        rec.t = root;
        rec.p = r.at(rec.t);
        auto outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return {true, rec};
    }
    virtual std::pair<bool,aabb_t> bounding_box(double time0, double time1) const override
    {
        auto vec = vec3_t(radius, radius, radius);
        return {true, aabb_t(center - vec, center + vec)};
    }
};

#endif