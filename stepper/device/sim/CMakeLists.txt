# (C) Copyright Marc Schafer
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#    Marc Schafer

# foamcut/stepper/device/sim


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
set (SIM_SRC
  ${DEVICE_COMMON_DIR}/Engine.cpp
  ${DEVICE_COMMON_DIR}/Line.cpp
  ${DEVICE_COMMON_DIR}/Message.cpp
  ${DEVICE_COMMON_DIR}/StatusFlags.cpp
  ${DEVICE_COMMON_DIR}/Stepper.cpp
  ASIOReceiver.cpp
  ASIOSender.cpp
  SimCommunicator.cpp
  Simulator.cpp
)

# headers
file(GLOB DEVICE_COMMON_HDR "${DEVICE_COMMON_DIR}/*.h*" )
file(GLOB DEVICE_SIM_HDR "*.h*" )
source_group("Headers" FILES ${DEVICE_COMMON_HDR} ${DEVICE_SIM_HDR})

include_directories ( 
    ${Boost_INCLUDE_DIRS}
    ${DEVICE_COMMON_DIR}
    ${DEVICE_SIM_DIR}
)

add_library (stepper_sim OBJECT
	${SIM_SRC}
	${DEVICE_COMMON_HDR}
	${DEVICE_SIM_HDR}
)
