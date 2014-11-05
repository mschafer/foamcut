# (C) Copyright Marc Schafer
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#    Marc Schafer

# foamcut/kernel/test

# boost: static libs, no auto link on windows
set(Boost_USE_STATIC_LIBS  ON)
add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
find_package( Boost REQUIRED COMPONENTS 
  system thread date_time chrono unit_test_framework log
)

link_directories(${Boost_LIBRARY_DIRS})
set (DEVICE_COMMON_DIR ${CMAKE_SOURCE_DIR}/stepper/device/common)

include_directories ( 
  ${Boost_INCLUDE_DIRS}
  ${DEVICE_COMMON_DIR}
  ..
)

add_definitions(-DFOAMCUT_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/kernel/test/data")

add_executable(test_foamcut_kernel
  airfoil_test.cpp
  hello_test.cpp
  ruled_surface_test.cpp
  shape_test.cpp
  spline_test.cpp
  $<TARGET_OBJECTS:foamcut_kernel>
  $<TARGET_OBJECTS:stepper_host>
  $<TARGET_OBJECTS:stepper_sim>
)

target_link_libraries(test_foamcut_kernel 
  ${Boost_LIBRARIES}
)
