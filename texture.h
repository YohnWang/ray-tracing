#ifndef TEXTURE_H
#define TEXTURE_H

#include<vec3.h>
#include<memory>
#include<perlin.h>

class texture_t
{
public:
    virtual colour_t value(double u,double v,const point3_t &p)const=0;
};

class solid_colour_t:public texture_t
{
    colour_t colour_value;
public:
    solid_colour_t()=default;
    solid_colour_t(colour_t c):colour_value{c}{}
    virtual colour_t value(double u,double v,const point3_t &p)const override
    {
        return colour_value;
    }
};

class checker_texture_t:public texture_t
{
public:
    std::shared_ptr<texture_t> even;
    std::shared_ptr<texture_t> odd;

    checker_texture_t()=default;
    checker_texture_t(std::shared_ptr<texture_t> e,std::shared_ptr<texture_t> o):even{e},odd{o}{}
    checker_texture_t(colour_t c1,colour_t c2):checker_texture_t(std::make_shared<solid_colour_t>(c1),std::make_shared<solid_colour_t>(c2)){}
    virtual colour_t value(double u,double v,const point3_t &p)const override
    {
        auto sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }
};

class noise_texture_t : public texture_t
{
public:
    perlin_t noise;
    double   scale=1;

    noise_texture_t() {}
    noise_texture_t(double scale):scale(scale){}

    virtual colour_t value(double u, double v, const point3_t &p) const override
    {
        return colour_t(1, 1, 1) * noise.turb(scale*p);
    }

};

#endif