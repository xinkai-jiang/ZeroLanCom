cmake_minimum_required(VERSION 3.14)
project(LANCOM_CPP LANGUAGES CXX)

# ----------------------------
# C++ Standard
# ----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ----------------------------
# Debug settings
# ----------------------------
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ----------------------------
# External dependencies
# ----------------------------
find_package(spdlog REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
# find_package(msgpack-cpp REQUIRED)

# ----------------------------
# Include directory
# ----------------------------
include_directories(${PROJECT_SOURCE_DIR}/include)

# ----------------------------
# Source files
# ----------------------------
file(GLOB_RECURSE LANCOM_SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# ----------------------------
# Build C++ core library
# ----------------------------
add_library(lancom INTERFACE)
target_include_directories(lancom INTERFACE
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(lancom INTERFACE
    ${ZMQ_LIBRARIES}
    spdlog::spdlog
    msgpackc
)


# ----------------------------
# Add examples
# ----------------------------
add_subdirectory(examples)