#include<cstdio>
#include<cmath>
#include<vec3.h>
#include<ray.h>
#include<hittable.h>
#include<camera.h>

using namespace std;

class sphere_t:public hittable_t
{
public:
    point3_t center;
    double   radius;

    sphere_t()=default;
    sphere_t(point3_t center,double radius):center(center),radius(radius){}

    virtual std::pair<bool,hit_record_t> hit(const ray_t &r, double t_min, double t_max) const override;

};

std::pair<bool,hit_record_t> sphere_t::hit(const ray_t& r, double t_min, double t_max)const
{
    auto AC=r.origin()-center;
    auto a=dot(r.direction(),r.direction());
    auto b=2*dot(r.direction(),AC);
    auto c=dot(AC,AC)-radius*radius;
    auto discriminant = b*b - 4*a*c;
    if(discriminant<0)
        return {false,{}};
    
    // Find the nearest root that lies in the acceptable range.
    auto sqrtd = sqrt(discriminant);

    auto root = (-b-sqrtd)/(2*a);
    if (root < t_min || t_max < root) {
        root = (-b+sqrtd)/(2*a);
        if (root < t_min || t_max < root)
            return {false,{}};
    }
    hit_record_t rec; 
    rec.t = root;
    rec.p = r.at(rec.t);
    auto outward_normal=(rec.p - center) / radius;
    rec.set_face_normal(r,outward_normal);

    return {true,rec};
}

std::pair<bool, hit_record_t> hittable_list_t::hit(const ray_t &r, double t_min, double t_max) const
{
    hit_record_t temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto &object : objects)
    {
        auto [is_hit,rec]=object->hit(r, t_min, closest_so_far);
        if (is_hit)
        {
            hit_anything = true;
            closest_so_far = rec.t;
            temp_rec = rec;
        }
    }
    return {hit_anything,temp_rec};
}

colour_t ray_colour(const ray_t &r,const hittable_list_t &world)
{
    auto [is_hit,rec]=world.hit(r, 0, infinity);
    if (is_hit) 
    {
        return 0.5 * (rec.normal.unit() + colour_t(1,1,1));
    }
    auto t=(r.dir.unit().y+1.0)*0.5;
    return (1-t)*colour_t(1,1,1)+t*colour_t(0.5, 0.7, 1.0);
}

int main(int argc, const char *argv[])
{
    //image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1024;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    printf("P3\n%d %d\n255\n",image_width,image_height);

    //camera 
    camera_t camera{};

    sphere_t s1(point3_t(0,0,-1), 0.5),s2(point3_t(0,-100.5,-1), 100);
    hittable_list_t world;
    world.add(&s1);
    world.add(&s2);
    //
    for(int i=image_height-1;i>=0;i--)
    {
        for(int j=0;j<image_width;j++)
        {
            colour_t pixel_colour(0, 0, 0);
            for(int k=0;k<100;k++)
            {
                auto v = (i+rand_uniform()) / image_height;
                auto u = (j+rand_uniform()) / image_width;
                // auto v = (i+0.0) / image_height;
                // auto u = (j+0.0) / image_width;
                auto r=camera.get_ray(u,v);
                pixel_colour += ray_colour(r, world);
            }
            pixel_colour*=0.01;
            write_clour(pixel_colour);
        }
    }
    return 0;
}
 