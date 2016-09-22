cmake_minimum_required(VERSION 3.0)
set(EXECUTABLE_NAME "countdown")
add_executable(${EXECUTABLE_NAME} main.cpp)
INSTALL(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
