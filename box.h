#ifndef BOX_H 
#define BOX_H
#include<hittable.h>
#include<vec3.h>
#include<aarect.h>

class box_t:public hittable_t
{
public:
    point3_t box_min;
    point3_t box_max;
    hittable_list_t sides;

    box_t()=default;
    box_t(const point3_t& a, const point3_t& b, std::shared_ptr<material_t> ptr):box_min(a),box_max(b)
    {
        //box_min=point3_t(fmin(a.x,b.x),fmin(a.y,b.y),fmin(a.z,b.z));
        //box_max=point3_t(fmax(a.x,b.x),fmax(a.y,b.y),fmax(a.z,b.z));

        sides.add(std::make_shared<rect_t>(vec3_t(0,-1,0),box_min,box_max-vec3_t(0,box_max.y,0),ptr)); //down
        sides.add(std::make_shared<rect_t>(vec3_t(0,1,0),box_min+vec3_t(0,box_max.y,0),box_max,ptr));  //up
        sides.add(std::make_shared<rect_t>(vec3_t(0,0,-1),box_min,box_max-vec3_t(0,0,box_max.z),ptr)); //front
        sides.add(std::make_shared<rect_t>(vec3_t(0,0,1),box_min+vec3_t(0,0,box_max.z),box_max,ptr)); //back
        sides.add(std::make_shared<rect_t>(vec3_t(-1,0,0),box_min,box_max-vec3_t(box_max.x,0,0),ptr)); //left
        sides.add(std::make_shared<rect_t>(vec3_t(1,0,0),box_min+vec3_t(box_max.x,0,0),box_max,ptr)); //right
    }

    virtual std::pair<bool, hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override
    {
        return sides.hit(r,t_min,t_max);
    }
    virtual std::pair<bool, aabb_t> bounding_box(double time0,double time1)const
    {
        return {true,aabb_t(box_min,box_max)};
    }

    void move(vec3_t direction)
    {
        for(auto &rect:sides.objects)
        {
            auto r=std::dynamic_pointer_cast<rect_t>(rect);
            r->move(direction);
        }
    }

};

#endif