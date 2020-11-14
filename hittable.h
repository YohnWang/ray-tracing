#ifndef HIT_H
#define HIT_H
#include<vec3.h>
#include<ray.h>
#include<utility>
#include<vector>
#include<memory>

class material_t;

struct hit_record_t
{
    point3_t p;
    vec3_t   normal;
    double   t;
    bool front_face;
    std::shared_ptr<material_t> mat_ptr;
    void set_face_normal(const ray_t &r, const vec3_t &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable_t
{
public:
    virtual std::pair<bool, hit_record_t> hit(const ray_t &r, double t_min, double t_max) const = 0;
};

class hittable_list_t:public hittable_t
{
public:
    std::vector<std::shared_ptr<hittable_t>> objects;
    hittable_list_t() {}
    hittable_list_t(std::shared_ptr<hittable_t> object) { objects.push_back(object);}

    void clear() { objects.clear(); }
    void add(std::shared_ptr<hittable_t> object) { objects.push_back(object); }

    virtual std::pair<bool, hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override;

};

#endif