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
#include<aarect.h>
#include<box.h>
#include<plane.h>

using namespace std;


colour_t ray_colour(const ray_t &r,const hittable_list_t &world,const colour_t &background={0,0,0},int depth=50)
{
    if(depth<=0)
        return {0,0,0};
    auto [is_hit,rec]=world.hit(r, 0.001, infinity);
    if(is_hit==false)
        return background;

    auto [is_reflect, attenuation, scattered] = rec.mat_ptr->scatter(r, rec);
    auto emitted=rec.mat_ptr->emitted(rec.u,rec.v,rec.p);
    if (is_reflect)
        return emitted + attenuation * ray_colour(scattered, world, background, depth - 1);
    else
        return emitted;
}

hittable_list_t rand_world()
{
    hittable_list_t world;
    auto material5 = make_shared<lambertian_t>(make_shared<noise_texture_t>(4));
    auto ground_material = make_shared<lambertian_t>(make_shared<checker_texture_t>(colour_t(0.2, 0.3, 0.6), colour_t(0.9, 0.9, 0.9)));
    world.add(make_shared<sphere_t>(point3_t(0, -1000, 0), 1000, material5));

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

    auto material2 = make_shared<diffuse_light_t>(colour_t(2, 2, 2));
    world.add(make_shared<sphere_t>(point3_t(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal_t>(colour_t(1, 1, 1));
    world.add(make_shared<sphere_t>(point3_t(4, 1, 0), 1.0, material3));

    auto material4 = make_shared<dielectric_t>(1.5,colour_t{1,1,1},0.01);
    world.add(make_shared<moving_sphere_t>(point3_t(0, 1, 2.5),point3_t(0, 1, 2),0,1, 1.0, material4));

    //auto material5 = make_shared<lambertian_t>(make_shared<noise_texture_t>());
    //world.add(make_shared<sphere_t>(point3_t(0, 1, 2.5)-vec3_t(8,1,5).unit()*2, 1.0, material5));

    world.add(make_shared<rect_t>(vec3_t(0,0,-1),point3_t(0,0,0),point3_t(2,1,1),material2));

    return world;
}

hittable_list_t cornell_box() 
{
    hittable_list_t objects;

    auto red   = make_shared<lambertian_t>(colour_t(.65, .05, .05));
    auto white = make_shared<lambertian_t>(colour_t(.73, .73, .73));
    auto green = make_shared<lambertian_t>(colour_t(.12, .45, .15));
    auto light = make_shared<diffuse_light_t>(colour_t(15, 15, 15));
    auto light1 = make_shared<diffuse_light_t>(colour_t(1, 1, 1));

    objects.add(make_shared<rect_t>(vec3_t(1,0,0), point3_t(0,0,-100),point3_t(0,100,0), green)); //left
    objects.add(make_shared<rect_t>(vec3_t(-1,0,0),point3_t(100,0,-100),point3_t(100,100,0), red)); //right
    objects.add(make_shared<rect_t>(vec3_t(0,-1,0),point3_t(20,100-0.0001,-65),point3_t(80,100-0.0001,-35), light));
    objects.add(make_shared<rect_t>(vec3_t(0,0,1),point3_t(0,0,-100),point3_t(100,100,-100), white)); //back
    objects.add(make_shared<rect_t>(vec3_t(0,-1,0),point3_t(0,100,-100),point3_t(100,100,0), white)); //up
    objects.add(make_shared<rect_t>(vec3_t(0,1,0),point3_t(0,0,-100),point3_t(100,0,0), white)); //down

    auto box=make_shared<xbox_t>(point3_t(30,25,-50),20,50,20,green);
    box->rotate_y(-30);
    objects.add(box);
    // auto glass=make_shared<dielectric_t>(1.5,colour_t{1,1,1},0);
    // auto box=make_shared<box_t>(point3_t(20,0,-20),point3_t(80,60,-80),glass);
    
    // box->move({0,20,0});
    // objects.add(box);

     
    // auto tex=make_shared<lambertian_t>(make_shared<checker_texture_t>(colour_t(1,1,1),colour_t(0,0,1)));
    // objects.add(std::make_shared<sphere_t>(point3_t(50,50,-150),100,tex));
    // auto plane=make_shared<xrect_t>(point3_t(50,50,130),vec3_t(10,0,0),vec3_t(0,10,0), tex);
    // plane->rotate_y(60);
     //objects.add(make_shared<plane_t>(point3_t(0,0,1000),vec3_t(1,0,0),vec3_t(0,1,0), light1));
    //objects.add(make_shared<xrect_t>(point3_t(50,50,100),vec3_t(10,0,0),vec3_t(0,10,0), tex));
    //objects.add(plane);
    return objects;
}

void image_render(int image_height,int image_width,int image_height_begin,int image_height_end,const camera_t &camera,const hittable_list_t &world,vector<colour_t> &out)
{
    for(int i=image_height_begin-1;i>=image_height_end;i--)
    {   
        for(int j=0;j<image_width;j++)
        {
            constexpr int samples=100;
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
    //srand(time(NULL));
    //image
    const auto aspect_ratio = 9.0 / 9.0;
    const int image_width = 512;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    printf("P3 %d %d 255\n",image_width,image_height);

    //camera 
    // auto lookfrom=point3_t{8,2,5};
    // auto lookat=point3_t{0,1,0};
    auto lookfrom=point3_t{50,50,150};//-vec3_t(1000,1000,1000);
    auto lookat=point3_t{50,50,-10};//-vec3_t(1000,1000,1000);
    //lookfrom=lookfrom+(lookfrom-lookat).unit()*2;
    camera_t camera(lookfrom,lookat,{0,1,0},37,aspect_ratio,0,0,0,1);


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
    
    //world=rand_world();
    world=cornell_box();

    constexpr int thread_num=12;
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
