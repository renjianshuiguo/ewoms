Why CMake
=========

You can use CMake 2.8 or higher as alternative to the build system 
provided by DUNE. CMake is included in most GNU/Linux distributions 
or can be downloaded at www.cmake.org. Using CMake has several 
advantages compared to the default DUNE build system:

 - Out-of-tree builds are the default way to build software: The
   directory where the source code resides won't get modified during
   compilation.
 - Much faster configuration step
 - Much simpler to write your own build system modules
 - Less noisy output during compilation

But alas, it comes with a cost:

 - Parameters to the compiler are usually not exactly the same as
   those picked by DUNE's build system. This might sometimes lead to
   problems.
 - Dependencies between DUNE modules are not handled automatically
   yet. This means that all DUNE modules on which eWoms depends (->
   common, grid, istl, geometry, localfunctions) need to be installed
   already.

Preparing the installation
--------------------------

After installing cmake on your system, make sure that the 'cmake'
executable is in your PATH. To configure the project, create an empty
directory which will be used to store the files stemming from the
build process and run

cd path/to/empty/build/directory
cmake path/to/ewoms/source/directory

You might need to set a few parameters if you didn't install the
required libraries system wide. CMake cache variables can be set using
the "-D" command line switch to 'cmake'. Common parameters are:

DUNE_DIR        Path where all DUNE modules have been compiled and can be
                found in subdirectories with the module name
DUNE_common_DIR Path where the DUNE-common module can be found. If there 
                is a 'dune-common' subdirectory in $DUNE_DIR, this is not
                required.
(All other DUNE modules are analogous)

UG_DIR            Location of the UG grid manager
ALUGrid_DIR       Location of the ALUGrid grid manager
METIS_DIR         Location of the METIS graph partioning library (required
                  for the parallel version of ALUGrid)
CMAKE_BUILD_TYPE  Type of build. Either 'debug', 'release' or
                  'ctest'. The 'ctest' build type is identical to the
                  'debug' one, but it adds the compiler flags
                  'fprofile-arcs' and '-ftest-coverage' which allows a
                  coverage analysis of the tests. The default build
                  type is 'release'.
ENABLE_TESTS       Set this variable to '1' if you want to compile the unit
                   tests. Since this takes quite some time and the
                   unit tests are not interesting for most people,
                   this variable defaults to '0'.
CMAKE_INSTALL_PREFIX The directory to which eWoms will be
                     installed. The default is '/usr/local'.

If cmake did not report any errors, the project can be build by 

    make

Finally, everything can be installed:

   make install

Examples
========

    cd path/to/empty/build/directory
    cmake -DCMAKE_BUILD_TYPE=debug \
          -DDUNE_DIR=/usr/local/dune/ \
          -DUG_DIR=/usr/local/ug \
          -DALUGrid_DIR=/usr/local/alugrid \
          -DMETIS_DIR=/usr/local/metis \
          path/to/ewoms/source/directory

You can also specify the compiler explicitly using the cmake options
-DCMAKE_CXX_COMPILER and -DCMAKE_C_COMPILER.

Testing
=======

eWoms uses CTest for regression testing [0]. We encourage you to
participate - it's easy: If you set up an build directory for eWoms
using cmake as described above, just run

   ctest -D Experimental

After a while, your results should appear on the eWoms CDash 
dashboard [0].

References
==========

[0] The eWoms CDash board: http://opm-project.org/CDash/index.php?project=eWoms
