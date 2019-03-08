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
