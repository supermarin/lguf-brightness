cmake_minimum_required(VERSION 3.6)
project(lgufbrightness)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

set(libusb_1_FIND_REQUIRED TRUE)
include(${PROJECT_SOURCE_DIR}/cmake/Findlibusb-1.0.cmake)
include_directories(${LIBUSB_1_INCLUDE_DIRS})

set(SOURCE_FILES main.cpp)
add_executable(lgufbrightness ${SOURCE_FILES})
target_link_libraries(lgufbrightness ${LIBUSB_1_LIBRARIES})
