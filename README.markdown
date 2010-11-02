MPSNA
=====

Massively Parallel Social Network Analysis

Compile
=======

In a properly mantained system:

    cd build && cmake .. && make

However, if you don't have this luxury, read on.

Special cases
-------------

You can combine any of these: (paths are provided as examples)

* Boost installed in a non-standard path:

        export BOOST_ROOT=/public/boost
        <same as before>

* Non-standard compiler:

        export CXX=mpCC
        <same as before>

* MPI installed in a non-standard path:

        cd build && cmake .. -DCMAKE_PREFIX_PATH=/usr/lpp/ppe.poe && make

License
=======

MPSNA is released under the MIT license (see the accompanying file LICENSE.txt).
