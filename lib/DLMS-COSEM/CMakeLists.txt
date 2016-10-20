set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include/ ${CMAKE_CURRENT_LIST_DIR}/include/hdlc/)

set(libDLMS-COSEM-SRC-FILES
	DLMS-COSEM.cpp
	StateMachine.cpp
	COSEM.cpp
	COSEMClient.cpp
	COSEMServer.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCClient.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCHelpers.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCLLC.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCMAC.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCServer.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCStatistics.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/modcnt.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/packet.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/AARQ.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/ASNType.cpp
	)

add_library(DLMS-COSEM ${libDLMS-COSEM-SRC-FILES})


