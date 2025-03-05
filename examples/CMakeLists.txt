cmake_minimum_required(VERSION 3.16)

# Publisher example
add_executable(publisher_example publisher_example.cpp)
target_link_libraries(publisher_example PRIVATE lancom-cpp)

# Subscriber example
add_executable(subscriber_example subscriber_example.cpp)
target_link_libraries(subscriber_example PRIVATE lancom-cpp)

# Service example
add_executable(service_example service_example.cpp)
target_link_libraries(service_example PRIVATE lancom-cpp)

# Service client example
add_executable(service_client_example service_client_example.cpp)
target_link_libraries(service_client_example PRIVATE lancom-cpp)

# Multiple nodes example
add_executable(multiple_nodes_example multiple_nodes_example.cpp)
target_link_libraries(multiple_nodes_example PRIVATE lancom-cpp)
