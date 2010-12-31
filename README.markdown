LANA
====

LANA is a massively parallel, **La**rge-scale **N**etwork
**A**nalyzer. Designed to run on [MPI][] clusters, LANA can easily
scale to massive networks. This software is currently under heavy
development.

Compile
-------

Go to the `lana` folder and run:

    cd build && cmake .. && make

Run
---

In the `build` folder run:

    src/lana --help

That's it. However, if you want to specify compile options or have
problems compiling, read on.

Compile options
---------------

Here are some of the most useful options you can pass to [CMake][],
LANA's build system. The syntax is simple:

    cmake -D<option>=<option_value>

* [`CMAKE_BUILD_TYPE`](http://www.cmake.org/cmake/help/cmake-2-8-docs.html#variable:CMAKE_BUILD_TYPE)

    Specifies the build type for make based generators. Possible
    values are empty, `Debug`, `Release`, `RelWithDebInfo` and
    `MinSizeRel`. These values control the level of optimization of
    the resulting binaries and libraries.

* [`CMAKE_CXX_COMPILER`](http://www.cmake.org/cmake/help/cmake-2-8-docs.html#variable:CMAKE_LANG_COMPILER)

    Specifies the compiler to use. For example this can be set to
    `clang++` if you have [clang][] installed and like useful compiler
    error messages.

[CMake][] has tons of variables you can specify. For more informations
take a look at the [CMake documentation][] and the output of these
commands:

        cmake --help
        cmake --help-variable-list
        cmake --help-variable <variable>

### Test-only options

The following variables are defined by us, you cannot find them in the
official [CMake documentation][].

* `WITH_TESTING`

    Build with unit tests. By default it's disabled, set this variable
    to `ON` or `true` to enable it. Once tests have finished building, run:

        ctest

    If you want to see the output of each test, pass `-V` to it. For
    more informations see `ctest --help` and the [CTest documentation][].

* `WITH_PERFORMANCE_TESTING`

    Build with performance tests. Options and usage are the same as
    `WITH_TESTING`, but note that the two variables are independent
    from each other. At the end of the test you will find logs
    (`*.log`) and plots (`*.png`) in the tests/performance build
    directory.

* `PERFORMANCE_TEST_SAME_SERVERS`

    Select the idlest servers at the start of the test and always use
    those. Set this variable to `ON` or `true` to enable this
    behaviour. Defaults to `OFF`, which means that it will re-select
    the idlest ones every time a process is added to the test.

* `${TEST_NAME}_MAX_PROCESSES`

    The max number of processes to use in a performance test. Defaults
    to 10. Must be set individually for each performance test, for
    example:

        -Dpage_rank_MAX_PROCESSES=120

    For a list of performance test names, check
    `tests/performance/CMakeLists.txt` and look for the first argument
    of the function `add_performance_test_executable`.

* `PERFORMANCE_TEST_SERVER_LIST`

    The comma-separated list of servers you want to use for your
    performance tests. Defaults to
    `$(tests/performance/list_csunibo.rb)`, which executes this
    program in a subshell, substituting the aforementioned string with
    its output (refer to a UNIX shell manual for a better
    explanation). *You will probably want to set that option to suit
    your environment*.

* `BOOST_TEST_LOG_OPTIONS`

    Specifies the options to pass to the test executables, see
    `tests/unit/degree_centrality_unit --help` for more informations.

* `MPIEXEC_NUMPROCS`

    Specifies the number of processes to run in parallel for each
    MPI-enabled test.

Compilation Problems
--------------------

Not all systems use standard paths and programs, or maybe you don't
have root access to install things or whatever.

Here is a list of all the problems we encountered. Feel free to file a
bug report with the problem and the solution and we'll add them here.

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
