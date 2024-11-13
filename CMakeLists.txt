cmake_minimum_required(VERSION 3.20)
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(DLMS-COSEM VERSION 1.0.0)

# load additional detection for asio
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# options off-by-default that you can enable
option(WITH_DOC "Create project documentation" OFF)
option(WITH_TEST "Build the test suite" OFF)

find_package(Asio 1.13 REQUIRED)
add_subdirectory(lib)
add_subdirectory(src)
if (WITH_TEST)
    find_package(Catch2 REQUIRED)
    enable_testing()
    add_subdirectory(test)
endif()
if (WITH_DOC)
    add_subdirectory(doc)
endif()
message(WARNING "Found Asio version ${ASIO_VERSION_MAJOR}.${ASIO_VERSION_MINOR}")
message(WARNING "Found Catch2 version ${Catch2_VERSION_MAJOR}.${Catch2_VERSION_MINOR}")
