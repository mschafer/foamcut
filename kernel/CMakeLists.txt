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

set (STEPPER_HOST_DIR ${CMAKE_SOURCE_DIR}/stepper/host)
set (DEVICE_COMMON_DIR ${CMAKE_SOURCE_DIR}/stepper/device/common)

include_directories (
  ${STEPPER_HOST_DIR}
  ${DEVICE_COMMON_DIR}
)

# source files
set (KERNEL_SRC
  airfoil.cpp
  dat_file.cpp
  ruled_surface.cpp
  shape.cpp
  spline.cpp
  stepper_info.cpp
)

# headers
file(GLOB KERNEL_HDR *.h*)
source_group("Headers" FILES ${KERNEL_HDR})

link_directories(${Boost_LIBRARY_DIRS})
include_directories ( 
    ${Boost_INCLUDE_DIRS} 
)

add_library (foamcut_kernel OBJECT
	${KERNEL_SRC}
	${KERNEL_HDR}
)

#install (TARGETS foamcut_kernel 
#  RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin
#  LIBRARY DESTINATION ${PROJECT_BINARY_DIR}/bin
#)

#add_custom_command(TARGET foamcut_kernel POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin
#    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:foamcut_kernel> ${PROJECT_BINARY_DIR}/bin
#)
