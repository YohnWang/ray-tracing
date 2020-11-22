#ifndef MOVING_H
#define MOVING_H

#include<hittable.h>

class moving_sphere_t: public hittable_t
{
public:
    point3_t center0, center1;
    double time0, time1;
    double radius;
    std::shared_ptr<material_t> mat_ptr;

    moving_sphere_t() {}
    moving_sphere_t(
        point3_t cen0, point3_t cen1, double _time0, double _time1, double r, std::shared_ptr<material_t> m)
        : center0(cen0), center1(cen1), time0(_time0), time1(_time1), radius(r), mat_ptr(m){};

    point3_t center(double time) const
    {
        return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
    }

    virtual std::pair<bool, hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override
    {
        auto AC = r.origin() - center(r.time());
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
        auto outward_normal = (rec.p - center(r.time())) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return {true, rec};
    }

    virtual std::pair<bool, aabb_t> bounding_box(double time0,double time1)const override
    {
        auto vec=vec3_t(radius,radius,radius);
        aabb_t box0(
            center(time0)-vec,
            center(time0)+vec
        );
        aabb_t box1(
            center(time1)-vec,
            center(time1)+vec
        );
        return {true,surrounding_box(box0,box1)};
    }
};

#endif