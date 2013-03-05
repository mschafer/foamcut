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

link_directories(${Boost_LIBRARY_DIRS})
include_directories ( 
    ${Boost_INCLUDE_DIRS} 
)

add_library (foamcut_kernel SHARED
	${KERNEL_SRC}
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
