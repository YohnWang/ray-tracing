#ifndef VEC_H
#define VEC_H
#include<cmath>
#include<cstdio>
#include <limits>
#include<cstdlib>

constexpr double pi=3.1415926535897932385;
constexpr double infinity = std::numeric_limits<double>::infinity();

inline double rand_uniform()
{
    return rand()/(RAND_MAX + 1.0);
}

inline double rand_double(double min, double max)
{
    return min + (max-min)*rand_uniform();
}

inline double clamp(double x, double min, double max)
{
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

// inline double dtor(double degrees)
// {
//     return degrees_to_radians(degrees);
// }

class vec3_t 
{
public:
    double x,y,z;

    vec3_t()=default;
    vec3_t(double x,double y,double z):x(x),y(y),z(z){}

    vec3_t operator-()const{return vec3_t(-x,-y,-z);}
    vec3_t& operator+=(const vec3_t &v)
    {
        x+=v.x;
        y+=v.y;
        z+=v.z;
        return *this;
    }
    vec3_t& operator-=(const vec3_t &v)
    {
        x-=v.x;
        y-=v.y;
        z-=v.z;
        return *this;
    }
    vec3_t& operator*=(const double t)
    {
        x*=t;
        y*=t;
        z*=t;
        return *this;
    }
    vec3_t& operator/=(const double t)
    {
        x/=t;
        y/=t;
        z/=t;
        return *this;
    }
    double len() const
    {
        return std::sqrt(len_squared());
    }
    double len_squared() const
    {
        return x*x+y*y+z*z;
    }
    vec3_t unit() const
    {
        vec3_t v=*this;
        v/=len();
        return v;
    }

    static inline vec3_t random() {
        return vec3_t(rand_uniform(), rand_uniform(), rand_uniform());
    }

    static inline vec3_t random(double min, double max) {
        return vec3_t(rand_double(min,max), rand_double(min,max), rand_double(min,max));
    }

};

inline vec3_t operator+(const vec3_t &u,const vec3_t &v)
{
    return vec3_t(u.x+v.x,u.y+v.y,u.z+v.z);
}

inline vec3_t operator-(const vec3_t &u,const vec3_t &v)
{
    return vec3_t(u.x-v.x,u.y-v.y,u.z-v.z);
}

inline vec3_t operator*(const vec3_t &u,const vec3_t &v)
{
    return vec3_t(u.x*v.x,u.y*v.y,u.z*v.z);
}

inline vec3_t operator*(const vec3_t &u,double t)
{
    return vec3_t(u.x*t,u.y*t,u.z*t);
}

inline vec3_t operator*(double t,const vec3_t &u)
{
    return vec3_t(u.x*t,u.y*t,u.z*t);
}

inline vec3_t operator/(vec3_t v,double t)
{
    v/=t;
    return v;
}

inline double dot(const vec3_t &u,const vec3_t &v)
{
    return u.x*v.x+u.y*v.y+u.z*v.z;
}

inline vec3_t cross(const vec3_t &u,const vec3_t &v)
{
    return vec3_t(
        u.y*v.z-u.z*v.y,
        u.z*v.x-u.x*v.z,
        u.x*v.y-u.y*v.x);
}



using colour_t=vec3_t;
using point3_t=vec3_t;

inline void write_clour(const colour_t &pixel_colour)
{
    int r=static_cast<int>(sqrt(pixel_colour.x)*255.999);
    int g=static_cast<int>(sqrt(pixel_colour.y)*255.999);
    int b=static_cast<int>(sqrt(pixel_colour.z)*255.999);
    std::printf("%d %d %d\n",r,g,b);
}

#endif