
# experiments/serial

# boost: static libs, no auto link on windows
set(Boost_USE_STATIC_LIBS  ON)
add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
find_package( Boost REQUIRED COMPONENTS 
  system thread date_time chrono unit_test_framework log
)

link_directories(${Boost_LIBRARY_DIRS})

include_directories ( 
  ${Boost_INCLUDE_DIRS} 
  ..
)

set (HEADERS
  AppPacket.hpp
  debug_stream.hpp
  LinkPair.hpp
  Packet.hpp
  Port.hpp
  SerialPackets.hpp
  SerialProtocol.hpp
  Synchronizer.hpp
)

source_group("Headers" FILES ${HEADERS})

add_executable(test_tse_serial
  AppPacket.cpp
  hello_test.cpp
  LinkPair.cpp
  Port.cpp
  port_test.cpp
  SerialProtocol.cpp
  ${HEADERS}
)

target_link_libraries(test_tse_serial 
  ${Boost_LIBRARIES}
)
