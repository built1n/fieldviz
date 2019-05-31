#include <cmath>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "gnuplot_i.hpp"

#include <fml/fml.h>

// under $HOME
#define HISTORY_FILE ".fieldviz_history"

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

    Manifold *path;
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

int ent_counter = 0;
map<int, Entity> entities;

int add_entity(Entity e)
{
    entities[ent_counter] = e;
    return ent_counter++;
}

int add_current(scalar I, Manifold *path)
{
    return add_entity((Entity){ Entity::CURRENT, I, path });
}

int add_charge(scalar Q_density, Manifold *path)
{
    return add_entity((Entity){ Entity::CHARGE, Q_density, path });
}

const scalar U0 = 4e-7 * M_PI;
const scalar C = 299792458;
const scalar E0 = 1 / ( U0 * C * C );
const scalar K_E = 1 / (4 * M_PI * E0);
const scalar DEFAULT_D = 1e-1;
scalar D = DEFAULT_D;

vec3 calc_Bfield(vec3 x)
{
    point = x;

    vec3 B = 0;

    for(map<int, Entity>::iterator i = entities.begin(); i != entities.end(); i++)
    {
        Entity &e = i->second;
        if(e.type == Entity::CURRENT)
            B += e.path->integrate(dB, D) * U0 * e.I;
    }

    return B;
}

vec3 calc_Efield(vec3 x)
{
    point = x;

    vec3 E = 0;

    for(map<int, Entity>::iterator i = entities.begin(); i != entities.end(); i++)
    {
        Entity &e = i->second;
        if(e.type == Entity::CHARGE)
            E += e.path->integrate(dE, D) * K_E * e.Q_density;
    }

    return E;
}

ostream *dump_ofs = NULL;

vec3 dump(vec3 s, vec3 ds)
{
    *dump_ofs << s << " " << ds << endl;

    return 0;
}

void dump_points(ostream &out, Manifold *c)
{
    dump_ofs = &out;
    c->integrate(dump, D);
}

int dump_entities(ostream &out, int which, map<int, Entity> &en)
{
    int count = 0;
    for(map<int, Entity>::iterator i = en.begin(); i != en.end(); i++)
    {
        Entity &e = i->second;
        if(which & e.type)
        {
            dump_points(out, e.path);

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

Manifold *parse_curve(stringstream &ss)
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
    else if(type == "plane")
    {
        vec3 origin, v1, v2;
        ss >> origin >> v1 >> v2;
        return (Surface*)new Plane(origin, v1, v2);
    }
    else if(type == "disk")
    {
        vec3 center, radius, normal;
        scalar angle;
        ss >> center >> radius >> normal >> angle;
        return (Surface*)new Disk(center, radius, normal, angle);
    }
    else if(type == "sphere")
    {
        vec3 center;
        scalar radius;
        ss >> center >> radius;

        return (Surface*)new Sphere(center, radius);
    }
    else if(type == "opencylinder")
    {
        vec3 origin, axis;
        scalar rad;
        ss >> origin >> axis >> rad;
        return (Surface*)new OpenCylinder(origin, axis, rad);
    }
    else if(type == "closedcylinder")
    {
        vec3 origin, axis;
        scalar rad;
        ss >> origin >> axis >> rad;
        return (Surface*)new ClosedCylinder(origin, axis, rad);
    }
    else throw "unknown curve type (must be line, arc, spiral, or toroid)";
}

void print_help()
{
    cout << endl;
    cout << "fieldviz 0.1" << endl;
    cout << "Copyright (C) 2019 Franklin Wei" << endl << endl;

    cout << "Commands:" << endl;
    cout << "  add {I CURRENT|Q DENSITY} MANIFOLD" << endl;
    cout << "    Add an entity of the specified type and the shape MANIFOLD, which can be:" << endl;
    cout << "    of (<X> is a 3-tuple specifying a vector):" << endl;
    cout << "     1-manifolds:" << endl;
    cout << "      line <a> <b>" << endl;
    cout << "      arc <center> <radius> <normal> angle" << endl;
    cout << "      solenoid <center> <radius> <normal> angle pitch" << endl;
    cout << "      toroid <center> <radius> <maj_normal> min_radius maj_angle pitch" << endl;
    cout << "     2-manifolds:" << endl;
    cout << "      plane <origin> <vec1> <vec2>" << endl;
    cout << "      disk <center> <radius> <normal> angle" << endl;
    cout << "      sphere <center> radius" << endl;
    cout << endl;
    cout << "  delete [ID..]" << endl;
    cout << "    Delete an entity by its previously returned identifier." << endl;
    cout << endl;
    cout << "  draw [I|Q] ..." << endl;
    cout << "    Draw the specified current/charge distributions" << endl;
    cout << endl;
    cout << "  field [E|B] <lower_corner> <upper_corner> DELTA" << endl;
    cout << "    Plot the E or B field in the rectangular prism bounded by lower and upper." << endl;
    cout << "    DELTA specifies density." << endl;
    cout << endl;
    cout << "  newwindow" << endl;
    cout << "    Make future plots go into a new window" << endl;
    cout << endl;
    cout << "  delta D" << endl;
    cout << "    Set integration fineness to D (smaller is better but slower)" << endl;
}

vec3 dA(vec3 s, vec3 dA)
{
    /* will cast to vec3 */
    return dA.magnitude();
}

string hist_path;

void exit_handler()
{
    write_history(hist_path.c_str());
}

void int_handler(int a)
{
    (void) a;
    exit(0);
}

int main(int argc, char *argv[])
{
    Surface *surf = new Sphere(vec3(0, 0, 1), 1);
    cout << "Area of 10x10 square = " << surf->integrate(dA, D) << endl;

    hist_path = getenv("HOME");
    hist_path += "/";
    hist_path += HISTORY_FILE;

    using_history();
    read_history(hist_path.c_str());
    atexit(exit_handler);
    signal(SIGINT, int_handler);

    Gnuplot *gp = NULL;

    try {
        Gnuplot::set_terminal_std("qt");
        gp = new Gnuplot();
    }
    catch(GnuplotException e) {
        Gnuplot::set_terminal_std("dumb");
        gp = new Gnuplot();
    }

    *gp << "set view equal xyz";

    cout << "Welcome to fieldviz!" << endl << endl;
    cout << "Type `help' for a command listing." << endl;

    /* will change to replot on subsequent commands */
    string plot_cmd = "splot";

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

                Manifold *path = parse_curve(ss);

                cout << "Manifold type: " << path->name() << endl;

                int idx;
                if(type == "i")
                    idx = add_current(val, path);
                else if(type == "q")
                    idx = add_charge(val, path);
                else throw "unknown distribution type (must be I or Q)";

                cout << "Index: " << idx << endl;
            }
            else if(cmd == "delete")
            {
                int id;
                while(ss >> id)
                {
                    if(entities.erase(id))
                        cout << "Deleted " << id << "." << endl;
                    else
                        cerr << "No entity " << id << "!" << endl;
                }
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

                string cmd = plot_cmd + " '" + fname + "' w vectors";
                *gp << cmd;

                plot_cmd = "replot";
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

                string cmd = plot_cmd + " for[i = 0:" + itoa(n - 1) + "] '" + fname + "' i i w vectors";
                *gp << cmd;

                plot_cmd = "replot";
            }
            else if(cmd == "delta")
            {
                ss >> D;
                if(D <= 0)
                {
                    cerr << "D must be positive and non-zero!" << endl;
                    D = DEFAULT_D;
                }
            }
            else if(cmd == "newwindow")
            {
                plot_cmd = "splot";
            }
            else if(cmd == "help")
                print_help();
            else
                cerr << "invalid" << endl;
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
