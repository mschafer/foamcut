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
  system thread date_time chrono log
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5SerialPort REQUIRED)

set(UI_FILES mainwindow.ui cutdialog.ui movedialog.ui
 datimportpage.ui setupdialog.ui simulator_dialog.ui)
qt5_wrap_ui(UI_HDRS ${UI_FILES})

set (DEVICE_COMMON_DIR ${CMAKE_SOURCE_DIR}/stepper/device/common)
set (STEPPER_HOST_DIR ${CMAKE_SOURCE_DIR}/stepper/host)
set (STEPPER_SIM_DIR ${CMAKE_SOURCE_DIR}/stepper/device/sim)

include_directories( 
  ../kernel
  ${Boost_INCLUDE_DIRS}
  ${DEVICE_COMMON_DIR}
  ${STEPPER_HOST_DIR}
  ${STEPPER_SIM_DIR}
)

# headers
file(GLOB GUI_HDR "*.h*")
source_group("Header Files" FILES ${GUI_HDR})

add_executable(foamcut_gui
  ${UI_HDRS}
  ${GUI_HDR}
  cutdialog.cpp
  cutplotmgr.cpp
  datimportpage.cpp
  datimportwizard.cpp
  fixedarplot.cpp
  foamcutapp.cpp
  importdat.cpp
  logdoublespinbox.cpp
  main.cpp
  mainwindow.cpp
  movedialog.cpp
  qcustomplot.cpp
  setupdialog.cpp
  shapeplotmgr.cpp
  simdialog.cpp
  $<TARGET_OBJECTS:foamcut_kernel>
  $<TARGET_OBJECTS:stepper_host>
  $<TARGET_OBJECTS:stepper_sim>
)

target_link_libraries( foamcut_gui
  Qt5::Widgets
  Qt5::PrintSupport
  Qt5::SerialPort
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


#if (WIN32)
#    add_custom_command(TARGET foamcut_gui POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin
#		COMMAND ${CMAKE_COMMAND} -E copy_directory ${QT_BIN_DIR} "${PROJECT_BINARY_DIR}/bin"
#	)
#endif (WIN32)

#include the debug runtime libraries in the package as well as release
set(CMAKE_INSTALL_DEBUG_LIBRARIES 1)
include(InstallRequiredSystemLibraries)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_property( TARGET foamcut_gui
    PROPERTY MACOSX_BUNDLE true
  )
  get_filename_component(QT_FRAMEWORK_DIR ${QtCore_location_Release} DIRECTORY)
  set (QT_BIN_DIR ${QT_FRAMEWORK_DIR}/../../bin)
  set(CONFIG_SUBDIR
    $<$<CONFIG:Debug>:Debug>
    $<$<CONFIG:Release>:Release>
  )
  add_custom_command(
    TARGET foamcut_gui
    POST_BUILD
    COMMAND ${QT_BIN_DIR}/macdeployqt
    ARGS ${CONFIG_SUBDIR}/foamcut_gui.app -dmg
  )
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  get_filename_component(QT_BIN_DIR ${QtCore_location_Release} PATH)
  set(CONFIG_SUBDIR 
    $<$<CONFIG:Debug>:Debug>
    $<$<CONFIG:Release>:Release> 
  )
  add_custom_command(
    TARGET foamcut_gui
	POST_BUILD
	COMMAND ${CMAKE_SOURCE_DIR}/gui/deploy.bat 
	ARGS ${QT_BIN_DIR} ${CONFIG_SUBDIR}/foamcut_gui.exe
  )
  
  install (
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Debug/
	DESTINATION bin
  )
  
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
