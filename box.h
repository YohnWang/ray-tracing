#ifndef BOX_H 
#define BOX_H
#include<hittable.h>
#include<vec3.h>

class box_t:public hittable_t
{
public:
    point3_t box_min;
    point3_t box_max;
    hittable_list_t sides;

    box_t()=default;
    box_t(const point3_t& p0, const point3_t& p1, std::shared_ptr<material_t> ptr)
    {
        sides.add();
    }
};

#endif