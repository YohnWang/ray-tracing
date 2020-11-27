#ifndef PERLIN_H 
#define PERLIN_H

#include<vector>
#include<vec3.h>

class perlin_t
{
    std::vector<vec3_t> ranvec;
    std::vector<int>    perm_x;
    std::vector<int>    perm_y;
    std::vector<int>    perm_z;

    static constexpr int point_count=256;

    static std::vector<int> perlin_generate_perm()
    {
        std::vector<int> p;
        for (int i = 0; i < point_count; i++)
            p.push_back(i);
        for(auto i=p.size()-1;i>0;i--)
        {
            auto swap_target=rand_int(0,i);
            std::swap(p[i],p[swap_target]);
        }
        return p;
    }

    static void permute(std::vector<int> &p)
    {
        for (int i = p.size()-1; i > 0; i--)
        {
            int target = rand_int(0, i);
            std::swap(p[i],p[target]);
        }
    }

    static double trilinear_interp(double c[2][2][2], double u, double v, double w)
    {
        auto accum = 0.0;
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                    accum += (i * u + (1 - i) * (1 - u)) *
                             (j * v + (1 - j) * (1 - v)) *
                             (k * w + (1 - k) * (1 - w)) * c[i][j][k];

        return accum;
    }

    static double perlin_interp(vec3_t c[2][2][2], double u, double v, double w)
    {
        auto uu = u * u * (3 - 2 * u);
        auto vv = v * v * (3 - 2 * v);
        auto ww = w * w * (3 - 2 * w);
        auto accum = 0.0;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                {
                    vec3_t weight_v(u - i, v - j, w - k);
                    accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
                }

        return accum;
    }

public:
    perlin_t()
    {
        for (int i = 0; i < point_count; ++i)
            ranvec.push_back(vec3_t::random(-1,1).unit());
        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }
    double noise(const point3_t &p) const
    {
        auto u = p.x - floor(p.x);
        auto v = p.y - floor(p.y);
        auto w = p.z - floor(p.z);

        auto i = static_cast<int>(floor(p.x)) ;
        auto j = static_cast<int>(floor(p.y)) ;
        auto k = static_cast<int>(floor(p.z)) ;

        vec3_t c[2][2][2];

        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                {
                    auto n=ranvec[perm_x[(i + di) & 255] ^perm_y[(j + dj) & 255] ^perm_z[(k + dk) & 255]];              
                    c[di][dj][dk] = n;
                }
                     
        return perlin_interp(c, u, v, w);
    }
    double turb(const point3_t &p, int depth = 7) const
    {
        auto accum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; i++)
        {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return fabs(accum);
    }
};


#endif