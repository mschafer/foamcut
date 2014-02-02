# Hotwire foam cutting software

Import XFoil .dat files, correct for wire kerf, and extrapolate for foam block placement.
Cuts can be simulated on a host PC or control 4 stepper motors via USB connection to an 
embedded microcontroller.

# Setting up the development environment

# Windows
* CMake 2.8.11
* Visual Studio 2012
* Qt 5.2.0 for Windows 64-bit (VS 2012, OpenGL)
* Windows SDK 8.1 because Qt has a dependency on it
* boost 1.55 built from sources for static libraries
** b2 --toolset=msvc address-model=64 link=static threading=multi runtime-link=shared -j N install
* Select Visual Studio 11 Win64 generator for the CMake project.

# Mac
* CMake 2.8.11
* XCode ?
* Qt 5.2.0 for Mac
* boost 1.55 static libraries
** ./bootstrap.sh --prefix=/where/you/want/boost
** ./b2 cxxflags="-std=c++11 -stdlib=libc++" linkflags="-stdlib=libc++" link=static -j N install
** set BOOST_ROOT in CMake for configuration
