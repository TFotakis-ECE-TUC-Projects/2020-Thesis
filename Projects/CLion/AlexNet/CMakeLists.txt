cmake_minimum_required(VERSION 3.14)
project(AlexNet C)

set(CMAKE_C_STANDARD 11)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

add_executable(AlexNet main.c imageUtils.h terminalColors.h nn-tools.h floatMatrix-tools.h)
target_link_libraries(AlexNet m jpeg)
