cmake_minimum_required(VERSION 3.16)

# Node example
add_executable(lancom_example ${PROJECT_SOURCE_DIR}/examples/lancom_example.cpp)
target_include_directories(lancom_example PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(lancom_example PRIVATE ${ZMQ_LIBRARIES} spdlog::spdlog)

# Custom message example
add_executable(custom_message_example ${PROJECT_SOURCE_DIR}/examples/custom_message.cpp)
target_include_directories(custom_message_example PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(custom_message_example PRIVATE ${ZMQ_LIBRARIES} spdlog::spdlog)