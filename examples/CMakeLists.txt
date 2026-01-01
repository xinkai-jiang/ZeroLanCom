cmake_minimum_required(VERSION 3.16)

# Node example
add_executable(zerolancom_example ${PROJECT_SOURCE_DIR}/examples/zerolancom_example.cpp)
target_include_directories(zerolancom_example PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(zerolancom_example PRIVATE zerolancom)

# Custom message example
add_executable(custom_message_example ${PROJECT_SOURCE_DIR}/examples/custom_message.cpp)
target_include_directories(custom_message_example PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(custom_message_example PRIVATE zerolancom)

# Service example
add_executable(service_example ${PROJECT_SOURCE_DIR}/examples/service_example.cpp)
target_include_directories(service_example PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(service_example PRIVATE zerolancom)