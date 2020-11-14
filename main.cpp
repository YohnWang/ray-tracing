#include<cstdio>
#include<cmath>
#include<vec3.h>
#include<ray.h>
#include<hittable.h>
#include<camera.h>
#include<material.h>
#include<thread>

using namespace std;


class sphere_t:public hittable_t
{
public:
    point3_t center;
    double   radius;
    shared_ptr<material_t> mat_ptr;

    sphere_t()=default;
    sphere_t(point3_t center,double radius,shared_ptr<material_t> m=make_shared<lambertian_t>()):center(center),radius(radius),mat_ptr(m){}

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
    rec.mat_ptr=mat_ptr;
    return {true,rec};
}

std::pair<bool, hit_record_t> hittable_list_t::hit(const ray_t &r, double t_min, double t_max) const
{
    hit_record_t temp_rec{};
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

colour_t ray_colour(const ray_t &r,const hittable_list_t &world,int deep=30)
{
    if(deep<=0)
        return {0,0,0};
    auto [is_hit,rec]=world.hit(r, 0.001, infinity);
    if (is_hit) 
    {
        //point3_t target = rec.p + rec.normal + random_in_unit_sphere().unit();
        //return colour_t(1.1,1.01,0.95)  * ray_colour(ray_t(rec.p, target - rec.p), world,deep-1);
        auto [is_reflect,attenuation,scattered]=rec.mat_ptr->scatter(r,rec);
        if(is_reflect)
            return attenuation*ray_colour(scattered,world,deep-1);
        else 
            return {0,0,0};
    }
    auto t=(r.dir.unit().y+1.0)*0.5;
    return (1-t)*colour_t(1,1,1)+t*colour_t(0.5, 0.7, 1);
}

void render(int &begin,int &end,int &image_height,int &image_width,camera_t &camera,hittable_list_t &world)
{
    for(int i=begin;i>=end;i--)
    {
        for(int j=0;j<image_width;j++)
        {
            colour_t pixel_colour(0, 0, 0);
            for(int k=0;k<80;k++)
            {
                auto v = (i+rand_double(-0.5,0.5)) / image_height;
                auto u = (j+rand_double(-0.5,0.5)) / image_width;
                auto r=camera.get_ray(u,v);
                pixel_colour += ray_colour(r, world);
            }
            pixel_colour*=1.0/80;
            write_clour(pixel_colour);
        }
    }
}

int tmain(int argc, char const *argv[])
{
        //image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1024;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    printf("P3\n%d %d\n255\n",image_width,image_height);

    //camera 
    //camera_t camera({0,0,0},{0,0,-1},vec3_t(0,1,0), 90, aspect_ratio);
    camera_t camera({0,0,1},{0,0,0});
    auto lambertian1=make_shared<lambertian_t>(colour_t{0.6,0.9,0.4});
    auto lambertian2=make_shared<lambertian_t>(colour_t{0.1,0.5,0.8});
    auto metal1=make_shared<metal_t>(colour_t{1,1,1},0.1);
    auto metal2=make_shared<metal_t>(colour_t{1,1,1});
    auto metal3=make_shared<metal_t>(colour_t{1,0.9,0.9},0.05);
    auto dielectric1 = make_shared<dielectric_t>(1.5);
    auto dielectric2 = make_shared<dielectric_t>(1);

    sphere_t s1(),s2(),s3(),s4();
    sphere_t s5();
    hittable_list_t world;
    world.add(make_shared<sphere_t>(point3_t(0,0,-1), 0.5,dielectric1));
    world.add(make_shared<sphere_t>(point3_t(0,-1000.5,-1), 1000,lambertian2));
    world.add(make_shared<sphere_t>(point3_t{0,0,-1},-0.25,dielectric1));
    world.add(make_shared<sphere_t>(point3_t(-3,1,-4),2,dielectric1));
    world.add(make_shared<sphere_t>(point3_t(3,1,-4),2,metal3));
    world.add(make_shared<sphere_t>(point3_t(0,0,-20000),9900,metal2));

    //thread th1(render,image_height-1,0,image_height,image_width,camera,world);
    //th1.join();

    return 0;
}


int main(int argc, const char *argv[])
{
    //image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1024;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    printf("P3\n%d %d\n255\n",image_width,image_height);

    //camera 
    //camera_t camera({0,0,0},{0,0,-1},vec3_t(0,1,0), 90, aspect_ratio);
    camera_t camera({0,0,1},{0,0,0});
    auto lambertian1=make_shared<lambertian_t>(colour_t{0.6,0.9,0.4});
    auto lambertian2=make_shared<lambertian_t>(colour_t{0.1,0.5,0.8});
    auto metal1=make_shared<metal_t>(colour_t{1,1,1},0.1);
    auto metal2=make_shared<metal_t>(colour_t{1,1,1});
    auto metal3=make_shared<metal_t>(colour_t{1,0.9,0.9},0.05);
    auto dielectric1 = make_shared<dielectric_t>(1.5);
    auto dielectric2 = make_shared<dielectric_t>(1);

    sphere_t s1(),s2(),s3(),s4();
    sphere_t s5();
    hittable_list_t world;
    world.add(make_shared<sphere_t>(point3_t(0,0,-1), 0.5,dielectric1));
    world.add(make_shared<sphere_t>(point3_t(0,-1000.5,-1), 1000,lambertian2));
    world.add(make_shared<sphere_t>(point3_t{0,0,-1},-0.25,dielectric1));
    world.add(make_shared<sphere_t>(point3_t(-3,1,-4),2,dielectric1));
    world.add(make_shared<sphere_t>(point3_t(3,1,-4),2,metal3));
    world.add(make_shared<sphere_t>(point3_t(0,0,-20000),9900,metal2));

    // shared_ptr<material_t> tab[]={lambertian1,lambertian2,metal1,metal2,metal3,dielectric1,dielectric2};
    // for(int i=0;i<20;i++)
    // {
    //     int r = rand();
    //     point3_t p=vec3_t::random(-20,20);
    //     p.z=-fabs(p.z);
    //     p.y=fabs(p.y);
    //     world.add(make_shared<sphere_t>(p, rand_uniform() * 5, tab[r%7]));
    // }

    int count=0;
    for(int i=image_height-1;i>=0;i--)
    {
        count++;
        if(count>=10)
        {
            fprintf(stderr,"completion %lf%%\n",100.0*(image_height-i)/(image_height));
            count=0;
        }
        
        for(int j=0;j<image_width;j++)
        {
            colour_t pixel_colour(0, 0, 0);
            for(int k=0;k<80;k++)
            {
                auto v = (i+rand_double(-0.5,0.5)) / image_height;
                auto u = (j+rand_double(-0.5,0.5)) / image_width;
                // auto v = (i+0.0) / image_height;
                // auto u = (j+0.0) / image_width;
                auto r=camera.get_ray(u,v);
                pixel_colour += ray_colour(r, world);
            }
            pixel_colour*=1.0/80;
            write_clour(pixel_colour);
        }
    }
    return 0;
}
