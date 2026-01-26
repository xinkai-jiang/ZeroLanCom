cmake_minimum_required(VERSION 3.14)
project(ZEROLANCOM LANGUAGES CXX)

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
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq) # ZeroMQ cannot be found by find_package

# ----------------------------
# Source files
# ----------------------------
file(GLOB_RECURSE ZEROLANCOM_SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# ----------------------------
# Build C++ core library
# ----------------------------
add_library(zerolancom SHARED ${ZEROLANCOM_SOURCES})
target_include_directories(zerolancom
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(zerolancom PUBLIC
  fmt::fmt
  spdlog::spdlog
  ${ZMQ_LIBRARIES}
)

# ----------------------------
# Install library
# ----------------------------
include(GNUInstallDirs)

install(TARGETS zerolancom
    EXPORT zerolancomTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT zerolancomTargets
    FILE zerolancomTargets.cmake
    NAMESPACE zerolancom::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerolancom
)


# ----------------------------
# Create and install config files
# ----------------------------
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/zerolancomConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/zerolancomConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerolancom
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zerolancomConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerolancom
)

export(EXPORT zerolancomTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/zerolancomTargets.cmake
    NAMESPACE zerolancom::
)

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/zerolancomConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/zerolancomConfig.cmake
    @ONLY
)

# ----------------------------
# Add examples
# ----------------------------
add_subdirectory(examples)

# ----------------------------
# Add tests (optional)
# ----------------------------
option(BUILD_TESTS "Build test suite" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()