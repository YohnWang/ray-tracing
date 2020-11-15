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

colour_t ray_colour(const ray_t &r,const hittable_list_t &world,int deep=50)
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

hittable_list_t rand_world()
{
    hittable_list_t world;
    auto ground_material = make_shared<lambertian_t>(colour_t(0.1, 0.2, 0.7));
    world.add(make_shared<sphere_t>(point3_t(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a+=2)
    {
        for (int b = -11; b < 11; b+=2)
        {
            auto choose_mat = rand_uniform();
            point3_t center(a + 0.9 * rand_uniform(), 0.2, b + 0.9 * rand_uniform());

            if ((center - point3_t(4, 0.2, 0)).len() > 0.9)
            {
                shared_ptr<material_t> sphere_material;
                if (choose_mat < 0.8)
                {
                    // diffuse
                    auto albedo = colour_t::random() * colour_t::random();
                    sphere_material = make_shared<lambertian_t>(albedo);
                    world.add(make_shared<sphere_t>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    // metal
                    auto albedo = colour_t::random(0.5, 1);
                    auto fuzz = rand_double(0, 0.5);
                    sphere_material = make_shared<metal_t>(albedo, fuzz);
                    world.add(make_shared<sphere_t>(center, 0.2, sphere_material));
                }
                else
                {
                    // glass
                    sphere_material = make_shared<dielectric_t>(1.5);
                    world.add(make_shared<sphere_t>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric_t>(1.5);
    world.add(make_shared<sphere_t>(point3_t(0, 1, 0), 1.0, material1));
    world.add(make_shared<sphere_t>(point3_t(0, 1, 0), -0.9, material1));

    auto material2 = make_shared<lambertian_t>(colour_t(0.4, 0.2, 0.1));
    world.add(make_shared<sphere_t>(point3_t(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal_t>(colour_t(1, 1, 1), 0.0);
    world.add(make_shared<sphere_t>(point3_t(4, 1, 0), 1.0, material3));

    return world;
}

int main(int argc, const char *argv[])
{
    //image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1024;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    printf("P3 %d %d 255\n",image_width,image_height);
    
    //camera 
    auto lookfrom=point3_t{8,2,5};
    auto lookat=point3_t{0,0,0};
    camera_t camera(lookfrom,lookat,{0,1,0},25,aspect_ratio);


    auto lambertian1=make_shared<lambertian_t>(colour_t{0.6,0.9,0.4});
    auto lambertian2=make_shared<lambertian_t>(colour_t{0.1,0.5,0.8});
    auto metal1=make_shared<metal_t>(colour_t{1,1,1},0.1);
    auto metal2=make_shared<metal_t>(colour_t{0.9,1,1},0.01);
    auto metal3=make_shared<metal_t>(colour_t{1,0.9,0.9},0.05);
    auto dielectric1 = make_shared<dielectric_t>(1.5);
    auto dielectric2 = make_shared<dielectric_t>(1);

    hittable_list_t world;
    // world.add(make_shared<sphere_t>(point3_t(0,0,-1), 0.5,dielectric1));
    // world.add(make_shared<sphere_t>(point3_t(0,-1000.5,-1), 1000,lambertian2));
    // world.add(make_shared<sphere_t>(point3_t{0,0,-1},-0.4,dielectric1));
    // world.add(make_shared<sphere_t>(point3_t(-1,0,-1),0.5,dielectric1));
    // world.add(make_shared<sphere_t>(point3_t(1,0,-1),0.5,metal3));
    // world.add(make_shared<sphere_t>(point3_t(0,0,-20000),9900,metal2));
    
    world=rand_world();

    int count=0;
    fprintf(stderr,"completion %2d%%",count);
    for(int i=image_height-1;i>=0;i--)
    {
        if(100.0*(image_height-i)/(image_height)>=count+2)
            fprintf(stderr,"\b\b\b%2d%%",count+=2);
        
        for(int j=0;j<image_width;j++)
        {
            colour_t pixel_colour(0, 0, 0);
            for(int k=0;k<50;k++)
            {
                auto v = (i+rand_double(-0.5,0.5)) / image_height;
                auto u = (j+rand_double(-0.5,0.5)) / image_width;
                auto r=camera.get_ray(u,v);
                pixel_colour += ray_colour(r, world);
            }
            pixel_colour*=1.0/50;
            write_clour(pixel_colour);
        }
    }
    return 0;
}
