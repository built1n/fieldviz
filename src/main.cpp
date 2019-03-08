#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include <readline/readline.h>
#include <readline/history.h>

#include "gnuplot_i.hpp"

#include "curve.h"
#include <fml/fml.h>

using namespace fml;
using namespace std;

/* A current or charge distribution */
struct Entity {
    /* can bitwise-OR together */
    enum { CHARGE = 1 << 0, CURRENT = 1 << 1 } type;
    union {
        scalar Q_density; /* linear charge density */
        scalar I; /* current */
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

int add_current(scalar I, Curve *path)
{
    Entity n = { Entity::CURRENT, I, path };
    entities.push_back(n);

    /* index */
    return entities.size() - 1;
}

int add_charge(scalar Q_density, Curve *path)
{
    Entity n = { Entity::CHARGE, Q_density, path };
    entities.push_back(n);

    return entities.size() - 1;
}

const scalar U0 = 4e-7 * M_PI;
const scalar C = 299792458;
const scalar E0 = 1 / ( U0 * C * C );
const scalar K_E = 1 / (4 * M_PI * E0);
const scalar D = 1e-2;

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

    return 0;
}

void dump_path(ostream &out, Curve *c)
{
    dump_ofs = &out;
    c->integrate(dump, D);
}

int dump_entities(ostream &out, int which, vector<Entity> &e)
{
    int count = 0;
    for(int i = 0; i < e.size(); i++)
    {
        if(which & e[i].type)
        {
            dump_path(out, e[i].path);

            /* two blank lines mark an index in gnuplot */
            out << endl << endl;

            count++;
        }
    }
    return count;
}

/* dump the field vectors with a spacing of `delta' */
/* requires x0 < x1, y0 < y1, z0 < z1 */

enum FieldType { E, B };

/* dump field in a region of space to vectors */
void dump_field(ostream &out,
                enum FieldType type,
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
                out << pt << " " << field << endl;
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

void all_lower(string &str)
{
    for(int i = 0; i < str.length(); i++)
        str[i] = tolower(str[i]);
}

string itoa(int n)
{
    stringstream ss;
    ss << n;
    return ss.str();
}

Curve *parse_curve(stringstream &ss)
{
    string type;
    ss >> type;
    if(type == "line" || type == "linesegment")
    {
        vec3 a, b;
        ss >> a >> b;
        return (Curve*)new LineSegment(a, b);
    }
    else if(type == "arc")
    {
        vec3 center, radius, normal;
        scalar angle;
        ss >> center >> radius >> normal;
        ss >> angle;
        return (Curve*)new Arc(center, radius, normal, angle);
    }
    else if(type == "spiral" || type == "solenoid")
    {
        vec3 origin, radius, normal;
        scalar angle, pitch;
        ss >> origin >> radius >> normal >> angle >> pitch;
        return (Curve*)new Spiral(origin, radius, normal, angle, pitch);
    }
    else if(type == "toroid")
    {
        vec3 origin, maj_radius, maj_normal;
        scalar min_radius, maj_angle, pitch;
        ss >> origin >> maj_radius >> maj_normal;
        ss >> min_radius >> maj_angle >> pitch;

        return (Curve*)new Toroid(origin, maj_radius, maj_normal, maj_angle, min_radius, pitch);
    }
    else throw "unknown curve type (must be line, arc, spiral, or toroid)";
}

void print_help()
{
    cout << endl;
    cout << "fieldviz 0.1" << endl;
    cout << "Copyright (C) 2019 Franklin Wei" << endl << endl;

    cout << "Commands:" << endl;
    cout << "  add {I CURRENT|Q DENSITY} CURVE" << endl;
    cout << "    Add an entity of the specified type and the shape CURVE, where CURVE is one" << endl;
    cout << "    of (<X> is a 3-tuple specifying a vector):" << endl;
    cout << "      line <a> <b>" << endl;
    cout << "      arc <center> <radius> <normal> angle" << endl;
    cout << "      solenoid <center> <radius> <normal> angle pitch" << endl;
    cout << "      toroid <center> <radius> <maj_normal> min_radius maj_angle pitch" << endl;
    cout << endl;
    cout << "  draw [I|Q] ..." << endl;
    cout << "    Draw the specified current/charge distributions" << endl;
    cout << endl;
    cout << "  field [E|B] <lower_corner> <upper_corner> DELTA" << endl;
    cout << "    Plot the E or B field in the rectangular prism bounded by lower and upper." << endl;
    cout << "    DELTA specifies density." << endl;
}

int main(int argc, char *argv[])
{
    Toroid loop(vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 0, 1), M_PI * 2, .1, 2*M_PI / 10);
    //add_current(1, (Curve*)&loop);

    Gnuplot *gp = NULL;

    try {
        gp = new Gnuplot();
    }
    catch(GnuplotException e) {
        Gnuplot::set_terminal_std("dumb");
        gp = new Gnuplot();
    }

    *gp << "set view equal xyz";

    cout << "Welcome to fieldviz!" << endl << endl;
    cout << "Type `help' for a command listing." << endl;

    while(1)
    {
        char *cs = readline("fieldviz> ");
        if(!cs)
            return 0;
        add_history(cs);
        
        string line(cs);

        free(cs);
        
        all_lower(line);

        /* parse */
        stringstream ss(line);

        string cmd;
        ss >> cmd;

        try {
            if(cmd == "add")
            {
                /* add a current or charge distribution */
                Entity e;

                string type;
                ss >> type;

                /* union */
                double val;
                ss >> val;

                Curve *path = parse_curve(ss);

                cout << "Curve type: " << path->name() << endl;

                int idx;
                if(type == "i")
                    idx = add_current(val, path);
                else if(type == "q")
                    idx = add_charge(val, path);
                else throw "unknown distribution type (must be I or Q)";

                cout << "Index: " << idx << endl;
            }
            else if(cmd == "field")
            {
                string type;

                vec3 lower, upper;
                scalar delta;

                if(!(ss >> type >> lower >> upper >> delta))
                    throw "plot requires <E/B> <lower> <upper> delta";

                FieldType t = (type == "e") ? FieldType::E : FieldType::B;

                ofstream out;
                string fname = gp->create_tmpfile(out);

                dump_field(out,
                           t,
                           lower, upper, delta);

                out.close();

                string cmd = "splot '" + fname + "' w vectors";
                *gp << cmd;
            }
            else if(cmd == "draw")
            {
                int e_types = 0;

                while(ss)
                {
                    string type_str;
                    if(ss >> type_str)
                    {
                        if(type_str == "i")
                            e_types |= Entity::CURRENT;
                        else if(type_str == "q")
                            e_types |= Entity::CHARGE;
                        else
                            throw "unknown entity type (must be I or Q)";
                    }
                }

                if(!e_types)
                    e_types |= Entity::CHARGE | Entity::CURRENT;

                ofstream out;
                string fname = gp->create_tmpfile(out);
                int n = dump_entities(out, e_types,
                                      entities);
                out.close();

                string cmd = "splot for[i = 0:" + itoa(n - 1) + "] '" + fname + "' i i w lines";
                *gp << cmd;
            }
            else if(cmd == "help")
                print_help();
        } catch(const char *err) {
            cerr << "parse error: " << err << endl;
        }
    }

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
