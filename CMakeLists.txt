cmake_minimum_required(VERSION 3.20)
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(DLMS-COSEM VERSION 1.0.0)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)
