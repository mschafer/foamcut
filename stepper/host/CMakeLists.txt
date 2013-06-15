# (C) Copyright Marc Schafer
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#    Marc Schafer

# foamcut/stepper/host


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
set (STEPPER_SRC
  stub.cpp
)

# headers
file(GLOB STEPPER_HDR *.hpp)
source_group("Headers" FILES ${STEPPER_HDR})

link_directories(${Boost_LIBRARY_DIRS})
include_directories ( 
    ${Boost_INCLUDE_DIRS} 
)

add_library (foamcut_stepper SHARED
	${STEPPER_SRC}
	${STEPPER_HDR}
)

install (TARGETS foamcut_stepper DESTINATION bin
  LIBRARY DESTINATION bin
)
