#include<cstdio>
#include<cmath>
#include<vec3.h>
#include<ray.h>
#include<hittable.h>
#include<camera.h>
#include<material.h>
#include<thread>
#include<sphere.h>
#include<moving_sphere.h>

using namespace std;


colour_t ray_colour(const ray_t &r,const hittable_list_t &world,int depth=50)
{
    if(depth<=0)
        return {0,0,0};
    auto [is_hit,rec]=world.hit(r, 0.001, infinity);
    if (is_hit) 
    {
        auto [is_reflect,attenuation,scattered]=rec.mat_ptr->scatter(r,rec);
        if(is_reflect)
            return attenuation*ray_colour(scattered,world,depth-1);
        else 
            return {0,0,0};
    }
    auto t=(r.dir.unit().y+1.0)*0.5;
    return (1-t)*colour_t(1,1,1)+t*colour_t(0.5, 0.7, 1);
}

hittable_list_t rand_world()
{
    hittable_list_t world;
    auto ground_material = make_shared<lambertian_t>(make_shared<checker_texture_t>(colour_t(0.2, 0.3, 0.6), colour_t(0.9, 0.9, 0.9)));
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
                    if(choose_mat<0.3)
                    {
                        auto albedo = colour_t::random() * colour_t::random();
                        auto tex=make_shared<checker_texture_t>(albedo,colour_t(1,1,1)-albedo);
                        sphere_material=make_shared<lambertian_t>(tex);
                        world.add(make_shared<sphere_t>(center, 0.2, sphere_material));
                    }
                    else 
                    {
                        auto albedo = colour_t::random() * colour_t::random();
                        sphere_material = make_shared<lambertian_t>(albedo);
                        world.add(make_shared<sphere_t>(center, 0.2, sphere_material));
                    }
                    
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

    auto material3 = make_shared<metal_t>(colour_t(1, 1, 1));
    world.add(make_shared<sphere_t>(point3_t(4, 1, 0), 1.0, material3));

    auto material4 = make_shared<dielectric_t>(1.5,colour_t{1,1,1},0.01);
    world.add(make_shared<moving_sphere_t>(point3_t(0, 1, 2.5),point3_t(0, 1, 2),0,1, 1.0, material4));

    return world;
}


void image_render(int image_height,int image_width,int image_height_begin,int image_height_end,const camera_t &camera,const hittable_list_t &world,vector<colour_t> &out)
{
    for(int i=image_height_begin-1;i>=image_height_end;i--)
    {   
        for(int j=0;j<image_width;j++)
        {
            constexpr int samples=20;
            colour_t pixel_colour(0, 0, 0);
            for(int k=0;k<samples;k++)
            {
                auto v = (i+rand_double(-1,1)) / image_height;
                auto u = (j+rand_double(-1,1)) / image_width;
                auto r=camera.get_ray(u,v);
                pixel_colour += ray_colour(r, world);
            }
            pixel_colour*=1.0/samples;
            out.push_back(pixel_colour);
        }
    }
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
    auto lookat=point3_t{0,1,0};
    lookfrom=lookfrom+(lookfrom-lookat).unit()*2;
    camera_t camera(lookfrom,lookat,{0,1,0},25,aspect_ratio,0,0,0,1);


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

    constexpr int thread_num=10;
    constexpr auto part=image_height/thread_num;
    vector<vector<colour_t>> output(thread_num);
    vector<thread> thread_pool(thread_num);
    for(int i=0;i<thread_num;i++)
    {
        auto start=image_height-i*part;
        auto end  =image_height-(i+1)*part;
        if(i==thread_num-1) end=0;
        thread_pool[i]=thread(image_render,image_height,image_width,start,end,cref(camera),cref(world),ref(output[i]));
    }
    for(auto &t:thread_pool)
        t.join();

    for(auto &thread_result:output)
    {
        for(auto &c:thread_result)
            write_clour(c);
    }
    return 0;
}
