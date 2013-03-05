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

# turn off auto-linking on Windows
add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(-DBOOST_ALL_DYN_LINK)
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
find_package( Boost REQUIRED COMPONENTS system unit_test_framework)

link_directories(${Boost_LIBRARY_DIRS})

include_directories ( 
  ${Boost_INCLUDE_DIRS} 
  ..
)

add_definitions(-DFOAMCUT_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/kernel/test/data")

add_executable(test_foamcut_kernel
  airfoil_test.cpp
  hello_test.cpp
  ruled_surface_test.cpp
  shape_test.cpp
  spline_test.cpp
)

target_link_libraries(test_foamcut_kernel 
  ${Boost_LIBRARIES}
  foamcut_kernel
)
