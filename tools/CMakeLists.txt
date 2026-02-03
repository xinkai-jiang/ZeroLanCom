cmake_minimum_required(VERSION 3.16)

# ZLC Node Discovery CLI Tool
add_executable(zlc ${PROJECT_SOURCE_DIR}/tools/zlc.cpp)
target_include_directories(zlc PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(zlc PRIVATE zerolancom)

# Install the CLI tool
install(TARGETS zlc
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
