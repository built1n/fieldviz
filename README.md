# fieldviz: A electric/magnetic field integrator

This program allows the calculation of the electric or magnetic field
in a region with arbitrary charge/current distributions.

# Installation

You will need my math library from
https://github.com/built1n/libfml/. Build and install that before
proceeding.

Then build using cmake:

    mkdir build
    cd build
    cmake ..
    make

This should give you a `fieldviz` executable. Run in-place or do

    sudo make install

To install system-wide.

# Usage

It's all command-line based:

Use the "add" command along with its requisite parameters to add a
current or charge element.

For example,

    add I 10 line 0 0 0 1 0 0

will add a 10 amp current on a line from <0 0 0> to <1 0 0>. Different
shapes are possible, including circular arcs, solenoids, and toroids. Each will need different parameters, which are documented below.

From there, you can either "draw" the current/charge distributions, or
plot the electric/magnetic fields they produce with the "field" command.

## Curve parameters

1. Line segment

    line [a] [b]

Pretty self-explanatory.

2. Arc

    arc [center] [radius] [normal] angle

center, radius, and normal are all vectors. Normal should be
normalized, or bad things might happen. Angle is the extent in radians.

3. Solenoid

   solenoid [center] [radius] [normal] angle pitch

The first four parameters have the same meaning as they do with
arcs. The pitch parameter controls the linear distance between
successive turns.

4. Toroid

   toroid [center] [radius] [maj_normal] min_radius maj_angle pitch

center and radius have the same meaning as before. See the diagram
below for the meanings of the rest.

![Toroid](https://github.com/built1n/fieldviz/raw/master/toroid.png)

NOTE: pitch is angular distance between successive turns, or at least
it's supposed to be. Currently there's a bug somewhere.
