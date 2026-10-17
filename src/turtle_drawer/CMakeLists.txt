cmake_minimum_required(VERSION 3.8)
project(turtle_drawer)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)

add_executable(drawer src/drawer.cpp)
add_executable(position_talker src/position_talker.cpp)

ament_target_dependencies(drawer
    rclcpp
    geometry_msgs
    turtlesim
)
ament_target_dependencies(position_talker
  rclcpp  
  turtlesim
)

install(TARGETS
    drawer
    position_talker
    DESTINATION lib/${PROJECT_NAME}
)

ament_package()