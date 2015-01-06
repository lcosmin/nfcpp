cmake_minimum_required(VERSION 2.8.4)
project(nfcpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)
set(SOURCE_FILES src/nfcpp.cc src/main.cc)
add_executable(nfc-mifare ${SOURCE_FILES})

target_link_libraries(nfc-mifare nfc)