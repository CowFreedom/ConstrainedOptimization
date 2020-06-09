# Constrained Optimization #

**Constrained Optimization** is a **UG4-Plugin** that aggregates a number of optimization algorithms fit for various problems.

# Contents
This document should give a broad overview on **ConstrainedOptimization** combined with some usage examples.
It also provides some brief notes concerning the installation of the plugin.

* [Installation Notes](#installation)
* [Description](#description)


A sample problem will be added to the description.
    
# Installation Notes 
# General
The installation equals the standard process for adding UG4 plugins described on [the ughub GitHub page.](https://github.com/UG4/ughub)
Due to the usage of OS specific functions for process generation, the plugin is not necessarily multiplatform. 
In its current form, the package has been shown to work on Windows 10, Linux Ubuntu and macOS Mojave 10.14.4. 
On Windows 10, two compilers were tested, GCC 9.2.0 and Visual Studio v16.3.10. 
It is assumed that the package also works for older compiler versions. On Linux Ubuntu, 
only GCC was tested. On macOS Mojave the Clang compiler was used. Some GCC and Clang require special flags
set in the build process, which is explained below.

### Installation notes for GCC
Follow the steps on [the ughub GitHub page.](https://github.com/UG4/ughub). If errors occur,
proceed with this text.
The plugin makes use of C++ std::threads. This might necessitates activating the -pthread flag
to the build process for ug4. Within your UG4 install library, go to
    cd ug4/ugcore/cmake/
and open 
    ug_includes.cmake
Now search for
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	   add_cxx_flag("-Wall")
	   add_cxx_flag("-Wno-multichar")
	   add_cxx_flag("-Wno-unused-local-typedefs")
	   add_cxx_flag("-Wno-maybe-uninitialized")
and add 

    add_cxx_flag("-pthread")

to the GNU include statements. Now rebuild UG4 as described on [the ughub GitHub page.](https://github.com/UG4/ughub). The plugin should
now be installed without any issues.

### Installation notes for Clang
Follow the steps on [the ughub GitHub page.](https://github.com/UG4/ughub). If errors occur,
proceed with this text.
The plugin makes use of C++11 features, like std::threads and constexpr. This might necessitates activating the -std=c++11 flag
to the build process for ug4. Within your UG4 install library, go to
    cd ug4/ugcore/cmake/
and open 
    ug_includes.cmake
Now search for
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	    add_cxx_flag("-Wall")
	    add_cxx_flag("-Wno-multichar")
and add 

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

to the Clang include statements. This ensures that the -std=c++11 flag is only added to the C++ compiler and not
the C compiler in the build process. Now rebuild UG4 as described on [the ughub GitHub page.](https://github.com/UG4/ughub). The plugin should
now be installed without any issues.


    sudo apt-get install cmake

If GCC is used, UG4 might have to be rebuilt with the -pthread flag. This is facilitates
by adding -lp
and clang might need the -std=c++11 flag.


# Description
The following algorithms are currently implemented:

|Name|Description|
|---|---|
|Newton-Gauss|The Gauss Newton procedure minimizes functions that can be represented as a sum of squares.|
|Particle Swarm Optimization|The Particle Swarm Optimization (PSO) algorithm minimizes any continuous function on a bounded domain.|
