#ifndef CAMERA_H
#define CAMERA_H
#include<vec3.h>
#include<ray.h>

class camera_t
{
    point3_t origin;
    point3_t lower_left_corner;
    vec3_t horizontal;
    vec3_t vertical;
    vec3_t u,v,w;
    double lens_radius;
public:
    camera_t(point3_t look_from,point3_t look_at,vec3_t vup,
            double vfov,double aspect_ratio=16.0/9.0,
            double aperture=0,double focus_dist=0)
    {
        //vfov is 'vertical field-of-view in degrees'
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0*h;
        auto viewport_width = aspect_ratio * viewport_height;
        //auto focal_length = 1.0;
        if(focus_dist==0)
            focus_dist=(look_from-look_at).len();

        w=(look_from-look_at).unit();
        u=cross(vup,w).unit();
        v=cross(w,u);

        origin = look_from;
        horizontal = viewport_width * u * focus_dist;
        vertical = viewport_height * v * focus_dist;
        //horizontal = vec3_t(viewport_width, 0, 0);
        //vertical = vec3_t(0,0 , viewport_height);
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - w * focus_dist;
        lens_radius = aperture / 2;
    }

    ray_t get_ray(double s, double t) const
    {
        vec3_t rd = lens_radius * random_in_unit_disk();
        vec3_t offset = u * rd.x + v * rd.y;
        return ray_t(origin+offset, lower_left_corner + s * horizontal + t * vertical - origin-offset);
    }
};

#endif