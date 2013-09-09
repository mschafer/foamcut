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

set (DEVICE_COMMON_DIR ${CMAKE_SOURCE_DIR}/stepper/device/common)
set (DEVICE_SIM_DIR ${CMAKE_SOURCE_DIR}/stepper/device/sim)

# source files
set (STEPPER_SRC
  ${DEVICE_COMMON_DIR}/Line.cpp
  ${DEVICE_COMMON_DIR}/Stepper.cpp
  ${DEVICE_SIM_DIR}/Platform.cpp
)

# headers
file(GLOB DEVICE_COMMON_HDR "${DEVICE_COMMON_DIR}/*.h*" )
file(GLOB DEVICE_SIM_HDR "${DEVICE_SIM_DIR}/*.h*" )
file(GLOB STEPPER_HDR *.h*)
source_group("Headers" FILES ${DEVICE_COMMON_HDR} ${STEPPER_HDR} ${DEVICE_SIM_HDR})

# directory for unit tests
add_subdirectory(test)

# headers
#source_group("Headers" FILES ${STEPPER_HDR})

link_directories(${Boost_LIBRARY_DIRS})
include_directories ( 
    ${Boost_INCLUDE_DIRS}
    ${DEVICE_COMMON_DIR}
    ${DEVICE_SIM_DIR}
)

add_library (foamcut_stepper SHARED
	${STEPPER_SRC}
	${STEPPER_HDR}
	${DEVICE_COMMON_HDR}
	${DEVICE_SIM_HDR}
)

install (TARGETS foamcut_stepper 
  RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin
  LIBRARY DESTINATION ${PROJECT_BINARY_DIR}/bin
)
