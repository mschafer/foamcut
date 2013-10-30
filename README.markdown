# Hotwire foam cutting software

Import XFoil .dat files, correct for wire kerf, and extrapolate for foam block placement.
Cuts can be simulated on a host PC or control 4 stepper motors via USB connection to an 
embedded microcontroller.

# Depends on boost 1.54 static libraries
build on mac:
./bootstrap.sh --prefix=/where/you/want/boost
./b2 cxxflags="-std=c++11 -stdlib=libc++" linkflags="-stdlib=libc++" link=static -j N install