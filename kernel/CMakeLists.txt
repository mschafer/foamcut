# (C) Copyright Marc Schafer
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#    Marc Schafer

# foamcut/kernel

# boost: static libs, no auto link on windows
set(Boost_USE_STATIC_LIBS  ON)
add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
find_package( Boost REQUIRED COMPONENTS 
  system thread date_time chrono
)

# directory for unit tests
add_subdirectory(test)

# source files
set (KERNEL_SRC
  airfoil.cpp
  dat_file.cpp
  machine.cpp
  ruled_surface.cpp
  shape.cpp
  spline.cpp
)

# headers
file(GLOB KERNEL_HDR *.hpp)
source_group("Headers" FILES ${KERNEL_HDR})

link_directories(${Boost_LIBRARY_DIRS})
include_directories ( 
    ${Boost_INCLUDE_DIRS} 
)

add_library (foamcut_kernel SHARED
	${KERNEL_SRC}
	${KERNEL_HDR}
)

install (TARGETS foamcut_kernel DESTINATION bin
  LIBRARY DESTINATION bin
)
