#include <cmath>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>

#include "vec3.h"
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

    scalar r2 = r.magnitudeSquared();

    vec3 rnorm = r / std::sqrt(r2);

    return ds.cross(rnorm) / r2;
}

//Arc loop(vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 0, 0), M_PI * 2);
//Spiral loop(vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 0, 0), M_PI * 2 * 10, 1);
//LineSegment loop(vec3(-.1, .1, 0), vec3(.1, .1, 0));
Toroid loop(vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 0, 1), M_PI * 2, .1, 2*M_PI / 10);

ostream *dump_ofs = NULL;

vec3 dump(vec3 s, vec3 ds)
{
    *dump_ofs << s << endl;
    //cout << "Magn: " << ds.magnitude() << endl;
        
    return 0;
}

void dump_path(ostream &out, const Curve *c)
{
    dump_ofs = &out;
    c->integrate(dump, 1e-2);
}

/* dump the field (gnuplot format) at z = 0 */
/* requires x0 < x1, y0 < y1 */

const scalar U0 = 4e-7 * M_PI;
const scalar I = 1;
const scalar delta = .2;

void dump_field(scalar x0, scalar y0, scalar z0, scalar x1, scalar y1, scalar z1)
{
    for(scalar z = z0; z <= z1; z += delta)
        for(scalar y = y0; y <= y1; y += delta)
            for(scalar x = x0; x <= x1; x += delta)
            {
                point = vec3(x, y, z);

                vec3 B = loop.integrate(dB, 1e-1) * U0 * I;

                if(B.magnitude() > 1e-8)
                {
                    B=B.normalize() / 10;
                    cout << point[0] << " " << point[1] << " " << point[2] << " ";
                    cout << B[0] << " " << B[1] << " " << B[2] << endl;
                }
            }
}

void dump_fieldline(ostream &out, vec3 x, scalar len)
{
    point = x;
    scalar delta = .1;
    while(len > 0)
    {
        out << point << endl;
        
        vec3 B = loop.integrate(dB, 1e-1) * U0 * I;

        point += delta * B;
        len -= delta;
    }
}

int main()
{
    //LineSegment wire(vec3(0, -100, 0), vec3(0, 100, 0));

    //std::cout << "length = " << loop.integrate(integrand, 1e-2) << endl;

    //vec3 dx = .01;

    //point = 0;

    //scalar I = 1;

    //for(int i = 0; i < 1000; i++, point += dx)
    //std::cout << point[0] << " " << U0 / ( 4 * M_PI ) * loop.integrate(dB, 1e-2)[0] << endl;
    
    dump_field(-2, -2, 0,
                12, 2, 0);
    //dump_field(0,0,0,0,0,0);

    stringstream ss;
    ss << "curve.fld";
    ofstream ofs(ss.str());

    dump_path(ofs, (Curve*)&loop);
    
    ofs.close();
    
    
#if 0
    mkdir("field", 0755);

    for(scalar y = -1.5; y <= 1.5; y += .1)
    {
        stringstream ss;
        ss << "field/" << y << ".fld";
        ofstream ofs(ss.str());

        dump_fieldline(ofs, vec3(0, y, 0), 10);

        ofs.close();
    }
#endif
}
