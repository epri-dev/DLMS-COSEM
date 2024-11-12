add_library(test_setup INTERFACE)
target_compile_definitions(test_setup INTERFACE USE_CATCH2_VERSION=${Catch2_VERSION_MAJOR})
target_link_libraries(test_setup INTERFACE $<IF:$<STREQUAL:"${Catch2_VERSION_MAJOR}","2">,Catch2::Catch2,Catch2::Catch2WithMain>)

set(my_tests UnitTest-APDU-AARE
    UnitTest-APDU-AARQ
    UnitTest-APDU-ACTION-Request
    UnitTest-APDU-ACTION-Response
    UnitTest-APDU-GET-Request
    UnitTest-APDU-GET-Response
    UnitTest-APDU-RLRE
    UnitTest-APDU-RLRQ
    UnitTest-APDU-xDLMS
    UnitTest-COSEM-Client
    UnitTest-COSEMObjectInstanceID
    UnitTest-DLMSVector
    UnitTest-HDLC-Address
    UnitTest-HDLC-Client
    UnitTest-HDLC-LLC
    UnitTest-HDLC-MAC
    UnitTest-HDLC-Packet
    UnitTest-HDLC-Server
    UnitTest-ICOSEMInterface
    UnitTest-Interfaces-COSEMData
    UnitTest-LinuxMemory
    UnitTest-StateMachine
)

foreach(TEST ${my_tests})
    add_executable(${TEST} ${TEST}.cpp)
    add_test(${TEST} ${TEST})
    target_compile_features(${TEST} PRIVATE cxx_std_17)
    target_include_directories(${TEST} PRIVATE ${PROJECT_SOURCE_DIR}/lib/DLMS-COSEM/include)
    target_link_libraries(${TEST} DLMS-COSEM test_setup) 
endforeach(TEST)
