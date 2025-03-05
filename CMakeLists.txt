cmake_minimum_required(VERSION 3.16)
project(lancom_cpp VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(LANCOM_BUILD_EXAMPLES "Build examples" ON)
option(LANCOM_BUILD_TESTS "Build tests" ON)

# Find dependencies
# Find libzmq package
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
# find_package(ZeroMQ REQUIRED)
# find_package(cppzmq REQUIRED)
find_package(msgpack-cxx REQUIRED)
find_package(spdlog REQUIRED)

# Library sources
set(LANCOM_SOURCES
    src/config.cpp
    src/lancom.cpp
    src/log.cpp
    src/nodes/abstract_node.cpp
    src/nodes/lancom_node.cpp
    src/nodes/silent_node.cpp
    src/sockets/publisher.cpp
    src/sockets/subscriber.cpp
    src/sockets/service.cpp
    src/sockets/service_proxy.cpp
    src/sockets/streamer.cpp
    src/utils/message_utils.cpp
    src/utils/node_utils.cpp
)

# Create the library
add_library(lancom-cpp ${LANCOM_SOURCES})
target_include_directories(lancom-cpp 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link dependencies
target_link_libraries(lancom-cpp 
    PUBLIC 
        libzmq-static
        cppzmq
        msgpackc
        spdlog::spdlog
)

# Compiler-specific settings
if(MSVC)
    target_compile_options(lancom-cpp PRIVATE /W4)
else()
    target_compile_options(lancom-cpp PRIVATE -Wall -Wextra -pedantic)
endif()

# Install rules
install(TARGETS lancom-cpp
    EXPORT lancom-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

# Build examples if requested
if(LANCOM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Build tests if requested
if(LANCOM_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Export targets
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/lancom-cpp-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT lancom-targets
    FILE lancom-cpp-targets.cmake
    NAMESPACE lancom::
    DESTINATION lib/cmake/lancom-cpp
)

configure_file(cmake/lancom-cpp-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/lancom-cpp-config.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/lancom-cpp-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lancom-cpp-config-version.cmake"
    DESTINATION lib/cmake/lancom-cpp
)