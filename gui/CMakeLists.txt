# (C) Copyright Marc Schafer
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#    Marc Schafer

# foamcut/gui

# boost: static libs, no auto link on windows
set(Boost_USE_STATIC_LIBS  ON)
add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
find_package( Boost REQUIRED COMPONENTS 
  system thread date_time chrono
)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set (QT_PREFIX_PATH "C:/Qt/5.3/msvc2013_64_opengl/lib/cmake")
#  set (WINSDK_LIB_PATH "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x64")
  set (CMAKE_PREFIX_PATH ${QT_PREFIX_PATH} ${WINSDK_LIB_PATH})
ELSE (${CMAKE_SYSTEM_NAME} MATCHES "Windows") 
  set (CMAKE_PREFIX_PATH /Users/mschafer/Qt5.3.1/5.3/clang_64/lib/cmake)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows") 

# todo find a better way to do this
#if (WIN32)
#  set (CMAKE_PREFIX_PATH C:/Qt/Qt5.2.0/5.2.0/msvc2012_64_opengl/lib/cmake)
#else (WIN32)
#  set(Qt5Widgets_DIR /Users/mschafer/Qt5.0.0/5.0.0/clang_64/lib/cmake/Qt5Widgets)
#  set(Qt5PrintSupport_DIR /Users/mschafer/Qt5.0.0/5.0.0/clang_64/lib/cmake/Qt5PrintSupport)
#endif (WIN32)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

set(UI_FILES mainwindow.ui cutdialog.ui movedialog.ui datimportpage.ui setupdialog.ui)
qt5_wrap_ui(UI_HDRS ${UI_FILES})

include_directories( 
  ../kernel
  ${Boost_INCLUDE_DIRS}  
)

add_executable(foamcut_gui
  ${UI_HDRS}
  cutdialog.cpp
  cutplotmgr.cpp
  datimportpage.cpp
  importwizard.cpp
  logdoublespinbox.cpp
  main.cpp
  mainwindow.cpp
  movedialog.cpp
  qcustomplot.cpp
  shapeplotmgr.cpp
  $<TARGET_OBJECTS:foamcut_kernel>
)

qt5_use_modules(foamcut_gui Widgets PrintSupport)
target_link_libraries( foamcut_gui
  ${Boost_LIBRARIES}
)

get_target_property(QtCore_location_Release Qt5::Core LOCATION_Release)
#get_target_property(QtWidgets_location_Release Qt5::Widgets LOCATION_Release)
#get_target_property(QtPrintSupport_location_Release Qt5::PrintSupport LOCATION_Release)
#get_target_property(QtGui_location_Release Qt5::Gui LOCATION_Release)
#get_target_property(QtCore_location_Debug Qt5::Core LOCATION_Debug)
#get_target_property(QtWidgets_location_Debug Qt5::Widgets LOCATION_Debug)
#get_target_property(QtPrintSupport_location_Debug Qt5::PrintSupport LOCATION_Debug)
#get_target_property(QtGui_location_Debug Qt5::Gui LOCATION_Debug)

#add_custom_command(TARGET foamcut_gui POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${QtCore_location_Release} ${PROJECT_BINARY_DIR}/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${QtWidgets_location_Release} ${PROJECT_BINARY_DIR}/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${QtPrintSupport_location_Release} ${PROJECT_BINARY_DIR}/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${QtGui_location_Release} ${PROJECT_BINARY_DIR}/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${QtCore_location_Debug} ${PROJECT_BINARY_DIR}/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${QtWidgets_location_Debug} ${PROJECT_BINARY_DIR}/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${QtPrintSupport_location_Debug} ${PROJECT_BINARY_DIR}/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${QtGui_location_Debug} ${PROJECT_BINARY_DIR}/bin
#)

get_filename_component(QT_BIN_DIR ${QtCore_location_Release} PATH)

if (WIN32)
    add_custom_command(TARGET foamcut_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_BIN_DIR} "${PROJECT_BINARY_DIR}/bin"
	)
endif (WIN32)
