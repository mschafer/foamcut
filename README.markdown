# Hotwire foam cutting software

Import XFoil .dat files, correct for wire kerf, and extrapolate for foam block placement.
Cuts can be simulated on a host PC or control 4 stepper motors via USB connection to an 
embedded microcontroller.

# Setting up the development environment

# Windows
* > CMake 3.6
* Visual Studio 2015
* Qt 5.7.x for Windows 64-bit
* boost 1.61 static libraries built from source using these commands
```
bootstrap.bat
b2 --prefix="C:\Program Files\boost\boost_1_61_vc14"   --toolset=msvc-11.0 address-model=64 link=static threading=multi runtime-link=shared -j N install
```
* configure CMake project with BOOST_ROOT in CMake to the prefix path above.
* Select Visual Studio 14 Win64 generator for the CMake project.

# Mac
* > CMake 3.0
* XCode ?
* Qt 5.3.x for Mac clang 64
* boost 1.56 static libraries built from source using these commands
```
./bootstrap.sh --prefix=/where/you/want/boost
./b2 cxxflags="-std=c++11 -stdlib=libc++" linkflags="-stdlib=libc++" link=static -j N install
```
* configure CMake project with BOOST_ROOT in CMake to the prefix path above.
