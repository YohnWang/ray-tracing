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
public:
    camera_t()
    {
        auto aspect_ratio = 16.0 / 9.0;
        auto viewport_height = 2.0;
        auto viewport_width = aspect_ratio * viewport_height;
        auto focal_length = 1;

        origin = point3_t(0, 0, 0);
        horizontal = vec3_t(viewport_width, 0, 0);
        vertical = vec3_t(0, viewport_height, 0);
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3_t(0, 0, focal_length);
        
    }

    ray_t get_ray(double u, double v) const
    {
        //fprintf(stderr,"%lf %lf %lf\n",lower_left_corner.x,lower_left_corner.y,lower_left_corner.z);
        return ray_t(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }
};

#endif