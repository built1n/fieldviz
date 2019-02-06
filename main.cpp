#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "vec3.h"
#include "curve.h"

using namespace std;

/* A current or charge distribution */
struct Entity {
    enum { CHARGE, CURRENT } type;
    union {
        double Q_density; /* linear charge density */
        double I; /* current */
    };

    Curve *path;
};

vec3 point;

/* dl x r / (|r| ^ 2) */
vec3 dB(vec3 s, vec3 ds)
{
    vec3 r = point - s;

    scalar r2 = r.magnitudeSquared();

    vec3 rnorm = r / std::sqrt(r2);

    return ds.cross(rnorm) / r2;
}

/* dl * r / (|r| ^ 2) */
vec3 dE(vec3 s, vec3 ds)
{
    vec3 r = point - s;

    scalar r2 = r.magnitudeSquared();

    vec3 rnorm = r / std::sqrt(r2);

    return rnorm * ds.magnitude() / r2;
}

vector<Entity> entities;

void add_current(scalar I, Curve *path)
{
    Entity n = { Entity::CURRENT, .I = I, .path = path };
    entities.push_back(n);
}

void add_charge(scalar Q_density, Curve *path)
{
    Entity n = { Entity::CHARGE, .Q_density = Q_density, .path = path };
    entities.push_back(n);
}

const scalar U0 = 4e-7 * M_PI;
const scalar C = 299792458;
const scalar E0 = 1 / ( U0 * C * C );
const scalar K_E = 1 / (4 * M_PI * E0);
const scalar D = 1e-1;

vec3 calc_Bfield(vec3 x)
{
    point = x;

    vec3 B = 0;

    for(int i = 0; i < entities.size(); i++)
    {
        if(entities[i].type == Entity::CURRENT)
            B += entities[i].path->integrate(dB, D) * U0 * entities[i].I;
    }

    return B;
}

vec3 calc_Efield(vec3 x)
{
    point = x;

    vec3 E = 0;

    for(int i = 0; i < entities.size(); i++)
    {
        if(entities[i].type == Entity::CHARGE)
            E += entities[i].path->integrate(dE, D) * K_E * entities[i].Q_density;
    }

    return E;
}

ostream *dump_ofs = NULL;

vec3 dump(vec3 s, vec3 ds)
{
    *dump_ofs << s << " " << ds << endl;
    //cout << "Magn: " << ds.magnitude() << endl;

    return 0;
}

void dump_path(ostream &out, Curve *c)
{
    dump_ofs = &out;
    c->integrate(dump, D);
}

void dump_paths(ostream &out, vector<Entity> &e)
{
    for(int i = 0; i < e.size(); i++)
    {
        dump_path(out, e[i].path);

        /* two blank lines mark an index in gnuplot */
        out << endl << endl;
    }
}

/* dump the field vectors with a spacing of `delta' */
/* requires x0 < x1, y0 < y1, z0 < z1 */
enum FieldType { E, B };

/* dump field in a region of space */
void dump_field(enum FieldType type,
                vec3 lower_corner, vec3 upper_corner,
                scalar delta)
{
    for(scalar z = lower_corner[2]; z <= upper_corner[2]; z += delta)
        for(scalar y = lower_corner[1]; y <= upper_corner[1]; y += delta)
            for(scalar x = lower_corner[0]; x <= upper_corner[0]; x += delta)
            {
                vec3 pt(x, y, z);
                vec3 field = (type == E) ? calc_Efield(pt) : calc_Bfield(pt);

                field = field.normalize() / 10;
                cout << pt << " " << field << endl;
            }
}

/* trace a field line */
void dump_fieldline(ostream &out, vec3 x, scalar len)
{
    point = x;
    scalar delta = .1;
    while(len > 0)
    {
        out << point << endl;

        vec3 B = calc_Bfield(point);

        point += delta * B;
        len -= delta;
    }
}

/* dump field magnitudes along a line */
void dump_values(vec3 start, vec3 del, int times)
{
    point = start;
    while(times--)
    {


        point += del;
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2)
        return 1;

    Toroid toroid(vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 0, 1), 2 * M_PI, .1, atof(argv[1]));
    Spiral solenoid(vec3(-1,0,0), vec3(0,1,0), vec3(1,0,0), 2*M_PI * 10, .2);
    //LineSegment wire(vec3(0, -10, 0), vec3(0, 10, 0));

    add_charge(1, (Curve*) &toroid);
    add_current(1, (Curve*) &solenoid);
    //add_current(1, (Curve*) &wire);

    dump_field(FieldType::E,
               vec3(-3, -3, -1),
               vec3(3, 3, 1),
               .5);

    stringstream ss;
    ss << "curve.fld";
    ofstream ofs(ss.str());

    dump_paths(ofs, entities);

    ofs.close();


    //LineSegment wire(vec3(0, -100, 0), vec3(0, 100, 0));

    //std::cout << "length = " << loop.integrate(integrand, 1e-2) << endl;

    //vec3 dx = .01;

    //point = 0;

    //scalar I = 1;

    //for(int i = 0; i < 1000; i++, point += dx)
    //std::cout << point[0] << " " << U0 / ( 4 * M_PI ) * loop.integrate(dB, 1e-2)[0] << endl;

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
