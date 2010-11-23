LANA
====

LANA is a massively parallel, **La**rge-scale **N**etwork **A**nalyzer. Designed to run on [MPI][] clusters, LANA can easily scale to massive networks. This software is currently under heavy development.

Compile
-------

Go to the `lana` folder and run:

    cd build && cmake .. && make

Run
---

In the `build` folder run:

    src/lana --help

That's it. However, if you want to specify compile options or have problems compiling, read on.

Compile options
---------------

Here are some of the most useful options you can pass to [CMake][], LANA's build system. The syntax is simple:

    cmake -D<option>=<option_value>

* [`CMAKE_BUILD_TYPE`](http://www.cmake.org/cmake/help/cmake-2-8-docs.html#variable:CMAKE_BUILD_TYPE)

    Specifies the build type for make based generators. Possible values are empty, `Debug`, `Release`, `RelWithDebInfo` and `MinSizeRel`. These values control the level of optimization of the resulting binaries and libraries.

* [`CMAKE_CXX_COMPILER`](http://www.cmake.org/cmake/help/cmake-2-8-docs.html#variable:CMAKE_LANG_COMPILER)

    Specifies the compiler to use. For example this can be set to `clang++` if you have [clang][] installed and like useful compiler error messages.

[CMake][] has tons of variables you can specify. For more informations take a look at the [CMake documentation][] and the output of these commands:

        cmake --help
        cmake --help-variable-list
        cmake --help-variable <variable>

### Test-only options

The following variables are defined by us, you cannot find them in the official [CMake documentation][].

* `WITH_TESTING`

    Build with unit tests. By default it's disabled, set this variable to `ON` or `true` to enable it. Once tests have finished building, run:

        ctest

    If you want to see the output of each test, pass `-V` to it. For more informations see `ctest --help` and the [CTest documentation][].

* `BOOST_TEST_LOG_OPTIONS`

    Specifies the options to pass to the test executables, see `tests/test_degree_centrality --help` for more informations.

* `MPIEXEC_NUMPROCS`

    Specifies the number of processes to run in parallel for each MPI-enabled test.

Compilation Problems
--------------------

Not all systems use standard paths and programs, or maybe you don't have root access to install things or whatever.

Here is a list of all the problems we encountered. Feel free to file a bug report with the problem and the solution and we'll add them here.

* Boost installed in a non-standard path:

        export BOOST_ROOT=/path/to/boost/prefix

* Non-standard compiler:

        export CXX=TheCompiler

* MPI installed in a non-standard path:

    declare this cmake variable (see above on how)

        CMAKE_PREFIX_PATH=/path/to/mpi/prefix

License
-------

LANA is released under the MIT license (see the accompanying file LICENSE.txt).

[MPI]: http://en.wikipedia.org/wiki/Message_Passing_Interface
[clang]: http://clang.llvm.org/
[CMake]: http://www.cmake.org/
[CMake documentation]: http://www.cmake.org/cmake/help/cmake-2-8-docs.html
[CTest documentation]: http://www.cmake.org/cmake/help/ctest-2-8-docs.html
