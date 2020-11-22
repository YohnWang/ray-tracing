#ifndef BVH_H
#define BVH_H

#include<hittable.h>
#include<aabb.h>
#include<algorithm>

class bvh_node_t:public hittable_t
{
public:
    aabb_t box;
    std::shared_ptr<hittable_t> left;
    std::shared_ptr<hittable_t> right;

    bvh_node_t()=default;
    bvh_node_t(std::vector<std::shared_ptr<hittable_t>> objects,size_t start,size_t end,double time0,double time1)
    {
        int axis=rand_int(0,3);
        static decltype(box_x_compare) *tab[]={box_x_compare,box_y_compare,box_z_compare};
        auto comparator=tab[axis];
        size_t object_span=end-start;
        if(object_span==1)
            left=right=objects[start];
        else if(object_span==2)
        {
            if(comparator(objects[start],objects[start+1]))
            {
                left=objects[start];
                right=objects[start+1];
            }
            else 
            {
                left=objects[start+1];
                right=objects[start];
            }
        }
        else 
        {
            std::sort(begin(objects)+start,begin(objects)+end,comparator);
            auto mid=start+object_span/2;
            left=std::make_shared<bvh_node_t>(objects,start,mid,time0,time1);
            right=std::make_shared<bvh_node_t>(objects,mid,end,time0,time1);
        }
        auto [exist_box_left,box_left]=left->bounding_box(time0,time1);
        auto [exist_box_right,box_right]=right->bounding_box(time0,time1);
        if(exist_box_left==false || exist_box_right==false)
            std::fprintf(stderr,"error! bvh constructor\n");
        box=surrounding_box(box_left,box_right);
    }

    virtual std::pair<bool, hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override
    {
        if(box.hit(r,t_min,t_max)!=true)
            return {false,{}};
        auto [is_left_hit,left_rec]=left->hit(r,t_min,t_max);
        auto [is_right_hit,right_rec]=right->hit(r,t_min,is_left_hit?left_rec.t:t_max);
        if(is_left_hit||is_right_hit)
            return {true,left_rec.t<right_rec.t?left_rec:right_rec};
        else if(is_left_hit)
            return {true,left_rec};
        else if(is_right_hit)
            return {true,right_rec};
        else 
            return {false,{}};
    }
    virtual std::pair<bool, aabb_t> bounding_box(double time0,double time1) const override
    {
        return {true,box};
    }

    static bool box_x_compare(const std::shared_ptr<hittable_t> a,const std::shared_ptr<hittable_t> b)
    {
        auto [exist_box_a,box_a]=a->bounding_box(0,0);
        auto [exist_box_b,box_b]=b->bounding_box(0,0);
        if(exist_box_a==false || exist_box_b==false)
            fprintf(stderr,"error!");
        return box_a.min().x<box_b.min().x;
    }
    static bool box_y_compare(const std::shared_ptr<hittable_t> a,const std::shared_ptr<hittable_t> b)
    {
        auto [exist_box_a,box_a]=a->bounding_box(0,0);
        auto [exist_box_b,box_b]=b->bounding_box(0,0);
        if(exist_box_a==false || exist_box_b==false)
            fprintf(stderr,"error!");
        return box_a.min().y<box_b.min().y;
    }
    static bool box_z_compare(const std::shared_ptr<hittable_t> a,const std::shared_ptr<hittable_t> b)
    {
        auto [exist_box_a,box_a]=a->bounding_box(0,0);
        auto [exist_box_b,box_b]=b->bounding_box(0,0);
        if(exist_box_a==false || exist_box_b==false)
            std::fprintf(stderr,"error!");
        return box_a.min().z<box_b.min().z;
    }
};

#endif