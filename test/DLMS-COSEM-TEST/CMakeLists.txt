#Generated by VisualGDB project wizard.
#Note: VisualGDB will automatically update this file when you add new sources to the project.

cmake_minimum_required(VERSION 3.1)
project(DLMS-COSEM-TEST)
set(CMAKE_CXX_STANDARD 11)
include_directories(../../lib/DLMS-COSEM/include/ ../../lib/googletest-release-1.7.0/include/)
find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG -DASIO_STANDALONE -DASIO_HAS_STD_CHRONO -DUNITTEST")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DEBUG -DASIO_STANDALONE -DASIO_HAS_STD_CHRONO -DUNITTEST")

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../lib/DLMS-COSEM/Debug/ ${CMAKE_CURRENT_LIST_DIR}/../../lib/googletest-release-1.7.0/Debug/)

add_executable(DLMS-COSEM-TEST DLMS-COSEM-TEST.cpp UnitTest-APDU-AARE.cpp UnitTest-APDU-AARQ.cpp UnitTest-APDU-ACTION-Request.cpp UnitTest-APDU-ACTION-Response.cpp UnitTest-APDU-GET-Request.cpp UnitTest-APDU-GET-Response.cpp UnitTest-APDU-RLRE.cpp UnitTest-APDU-RLRQ.cpp UnitTest-APDU-xDLMS.cpp UnitTest-COSEM-Client.cpp UnitTest-COSEMObjectInstanceID.cpp UnitTest-DLMSVector.cpp UnitTest-HDLC-Address.cpp UnitTest-HDLC-Client.cpp UnitTest-HDLC-LLC.cpp UnitTest-HDLC-MAC.cpp UnitTest-HDLC-Packet.cpp UnitTest-HDLC-Server.cpp UnitTest-ICOSEMInterface.cpp UnitTest-Interfaces-COSEMData.cpp UnitTest-LinuxMemory.cpp UnitTest-StateMachine.cpp)
target_link_libraries(DLMS-COSEM-TEST DLMS-COSEM gtest "${CMAKE_THREAD_LIBS_INIT}")
