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
        box_min=point3_t(fmin(a.x,b.x),fmin(a.y,b.y),fmin(a.z,b.z));
        box_max=point3_t(fmax(a.x,b.x),fmax(a.y,b.y),fmax(a.z,b.z));
        auto v=box_max-box_min;

        sides.add(std::make_shared<rect_t>(vec3_t(0,-1,0),box_min,box_max-vec3_t(0,v.y,0),ptr)); //down
        sides.add(std::make_shared<rect_t>(vec3_t(0,1,0),box_min+vec3_t(0,v.y,0),box_max,ptr));  //up
        sides.add(std::make_shared<rect_t>(vec3_t(0,0,-1),box_min,box_max-vec3_t(0,0,v.z),ptr)); //back
        sides.add(std::make_shared<rect_t>(vec3_t(0,0,1),box_min+vec3_t(0,0,v.z),box_max,ptr)); //front
        sides.add(std::make_shared<rect_t>(vec3_t(-1,0,0),box_min,box_max-vec3_t(v.x,0,0),ptr)); //left
        sides.add(std::make_shared<rect_t>(vec3_t(1,0,0),box_min+vec3_t(v.x,0,0),box_max,ptr)); //right
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

class xbox_t:public hittable_t
{
public:
    xrect_t sides[6];
    point3_t center;
    xbox_t(point3_t center,double xlen,double ylen,double zlen,std::shared_ptr<material_t> ptr):center(center)
    {
        sides[0]=xrect_t(center-vec3_t(0,ylen*0.5,0),{xlen,0,0},{0,0,zlen},ptr);//down
        sides[1]=xrect_t(center+vec3_t(0,ylen*0.5,0),{-xlen,0,0},{0,0,zlen},ptr);//up
        sides[2]=xrect_t(center-vec3_t(0,0,zlen*0.5),{-xlen,0,0},{0,ylen,0},ptr);//back
        sides[3]=xrect_t(center+vec3_t(0,0,zlen*0.5),{xlen,0,0},{0,ylen,0},ptr);//front
        sides[4]=xrect_t(center-vec3_t(xlen*0.5,0,0),{0,-ylen,0},{0,0,zlen},ptr);//left
        sides[5]=xrect_t(center+vec3_t(xlen*0.5,0,0),{0,ylen,0},{0,0,zlen},ptr);//right
    }

    virtual std::pair<bool, hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override
    {
        hit_record_t temp_rec{};
        bool hit_anything = false;
        auto closest_so_far = t_max;

        for (const auto &object : sides)
        {
            auto [is_hit, rec] = object.hit(r, t_min, closest_so_far);
            if (is_hit)
            {
                hit_anything = true;
                closest_so_far = rec.t;
                temp_rec = rec;
            }
        }
        return {hit_anything, temp_rec};
    }
    virtual std::pair<bool, aabb_t> bounding_box(double time0,double time1)const
    {
        return {false,aabb_t()};
    }

    void rotate_y(double theta)
    {
        for(auto &side:sides)
            side.rotate_y(center,theta);
    }
};

#endif