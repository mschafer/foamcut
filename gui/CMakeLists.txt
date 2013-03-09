# (C) Copyright Marc Schafer
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#    Marc Schafer

set(Qt5Widgets_DIR /Users/mschafer/Qt5.0.0/5.0.0/clang_64/lib/cmake/Qt5Widgets)
set(Qt5PrintSupport_DIR /Users/mschafer/Qt5.0.0/5.0.0/clang_64/lib/cmake/Qt5PrintSupport)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

set(UI_FILES mainwindow.ui datimportpage.ui)
qt5_wrap_ui(UI_HDRS ${UI_FILES})

include_directories( ../kernel )

add_executable(foamcut_gui
  ${UI_HDRS} 
  datimportpage.cpp
  importwizard.cpp
  main.cpp
  mainwindow.cpp
  qcustomplot.cpp
)

qt5_use_modules(foamcut_gui Widgets PrintSupport)
target_link_libraries( foamcut_gui foamcut_kernel )


