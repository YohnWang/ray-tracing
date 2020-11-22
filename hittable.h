#ifndef HIT_H
#define HIT_H
#include<vec3.h>
#include<ray.h>
#include<utility>
#include<vector>
#include<memory>
#include<aabb.h>

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
    virtual std::pair<bool, aabb_t> bounding_box(double time0,double time1)const=0;
};

class hittable_list_t:public hittable_t
{
public:
    std::vector<std::shared_ptr<hittable_t>> objects;
    hittable_list_t() {}
    hittable_list_t(std::initializer_list<std::shared_ptr<hittable_t>> list)
    {
        for(auto &object:list)
            objects.push_back(object);
    }

    void clear() { objects.clear(); }
    void add(std::shared_ptr<hittable_t> object) { objects.push_back(object); }

    virtual std::pair<bool, hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override
    {
        hit_record_t temp_rec{};
        bool hit_anything = false;
        auto closest_so_far = t_max;

        for (const auto &object : objects)
        {
            auto [is_hit, rec] = object->hit(r, t_min, closest_so_far);
            if (is_hit)
            {
                hit_anything = true;
                closest_so_far = rec.t;
                temp_rec = rec;
            }
        }
        return {hit_anything, temp_rec};
    }
    virtual std::pair<bool, aabb_t> bounding_box(double time0,double time1)const override
    {
        if (objects.empty()) 
            return {false,{}};
        aabb_t output_box;
        bool   is_first_box=true;
        
        for(auto &object:objects)
        {
            auto [exist_box,box]=object->bounding_box(time0,time1);
            if(exist_box==false)
                return {false,{}};
            if(is_first_box)
            {
                is_first_box=false;
                output_box=box;
            }
            else 
            {
                output_box=surrounding_box(output_box,box);
            }
        }
        return {true,output_box};
    }
};

#endif