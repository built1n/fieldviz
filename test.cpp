#include <cmath>
#include <iostream>
#include "curve.h"
using namespace std;

vec3 integrand(vec3 s, vec3 ds)
{
    cout << "point " << s << "ds = " << ds << endl;

    return ds;
}

vec3 point;

/* dl x r / (|r| ^ 2) */
vec3 dB(vec3 s, vec3 ds)
{
    vec3 r = s - point;

    double r2 = r.magnitudeSquared();

    vec3 rnorm = r / std::sqrt(r2);

    return ds.cross(rnorm) / r2;
}

//Arc loop(vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 0, 0), M_PI * 2);
//Spiral loop(vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 0, 0), M_PI * 2 * 10, 1);
LineSegment loop(vec3(0, 0, 0), vec3(10, 0, 0));

/* dump the field (gnuplot format) at z = 0 */
/* requires x0 < x1, y0 < y1 */
void dump_field(double x0, double y0, double z0, double x1, double y1, double z1)
{
    const double delta = .2;

    for(double z = z0; z <= z1; z += delta)
        for(double y = y0; y <= y1; y += delta)
            for(double x = x0; x <= x1; x += delta)
            {
                point = vec3(x, y, z);
                const double U0 = 4e-7 * M_PI;
                const double I = 1;

                vec3 B = loop.integrate(dB, 1e-1) * U0 * I;

                if(B.magnitude() > 1e-8)
                {
                    B=B.normalize() / 10;
                    cout << point[0] << " " << point[1] << " " << point[2] << " ";
                    cout << B[0] << " " << B[1] << " " << B[2] << endl;
                }
            }
}

int main()
{
    //LineSegment wire(vec3(0, -100, 0), vec3(0, 100, 0));

    //std::cout << "length = " << loop.integrate(integrand, 1e-2) << endl;

    //vec3 dx = .01;

    //point = 0;

    //double I = 1;

    //for(int i = 0; i < 1000; i++, point += dx)
    //std::cout << point[0] << " " << U0 / ( 4 * M_PI ) * loop.integrate(dB, 1e-2)[0] << endl;

    dump_field(-1, -1.5, -1.5,
               11, 1.5, 1.5);
}
