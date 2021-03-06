# (C) Copyright Marc Schafer
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#    Marc Schafer

project(foamcut)
cmake_minimum_required(VERSION 3.0.0)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -g")
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")
  add_definitions(-D_WIN32_WINNT=0x601)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 

# to find some custom cmake modules
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

add_subdirectory(kernel)
add_subdirectory(gui)
add_subdirectory(stepper)
add_subdirectory(experiments)

set (CPACK_PACKAGE_EXECUTABLES "foamcut")
set (CPACK_GENERATOR "ZIP")
set (CPACK_PACKAGE_VERSION_MAJOR "0")
set (CPACK_PACKAGE_VERSION_MINOR "0")
set (CPACK_PACKAGE_VERSION_PATCH "1")

include(CPack)

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/kernel/test/data
  DESTINATION .
)
