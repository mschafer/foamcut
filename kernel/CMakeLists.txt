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

# this adds all .cpp file in the directory w/o explicitly listing them
aux_source_directory(${PROJECT_SOURCE_DIR}/kernel KERNEL_SRCS)

link_directories(${Boost_LIBRARY_DIRS})
include_directories ( 
    ${Boost_INCLUDE_DIRS} 
    ${PROJECT_SOURCE_DIR}/device/common
    ${PROJECT_SOURCE_DIR}/device/sim
    ${PROJECT_SOURCE_DIR}/host/lib
)

add_library (foamcut_kernel SHARED
	${KERNEL_SRCS}
)

install (TARGETS foamcut_kernel DESTINATION bin
  LIBRARY DESTINATION bin
)
  
install (FILES 
    spline.hpp
    DESTINATION include
)

if (NOT WIN32)
    target_link_libraries(foamcut_kernel boost_system boost_thread)
endif (NOT WIN32)
