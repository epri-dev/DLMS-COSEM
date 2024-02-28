set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include/ ${CMAKE_CURRENT_LIST_DIR}/include/hdlc/)

set(libDLMS-COSEM-SRC-FILES
	COSEM.cpp
	COSEMClient.cpp
	COSEMDevice.cpp
	COSEMEngine.cpp
	COSEMObjectInstanceID.cpp
	COSEMSecurity.cpp
	COSEMServer.cpp
	DLMS-COSEM.cpp
	DLMSVector.cpp
	LogicalDevice.cpp
	StateMachine.cpp
	Wrapper.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCAddress.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCClient.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCHelpers.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCLLC.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCMAC.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCServer.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/HDLCStatistics.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/modcnt.cpp
	${CMAKE_CURRENT_LIST_DIR}/hdlc/packet.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/AARE.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/AARQ.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/APDUConstants.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/APDUFactory.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/ASNType.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/ACTION-Request.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/ACTION-Response.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/GET-Request.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/GET-Response.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/SET-Request.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/SET-Response.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/RLRE.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/RLRQ.cpp
	${CMAKE_CURRENT_LIST_DIR}/APDU/xDLMS.cpp
	${CMAKE_CURRENT_LIST_DIR}/interfaces/COSEMData.cpp
	${CMAKE_CURRENT_LIST_DIR}/interfaces/IClock.cpp
	${CMAKE_CURRENT_LIST_DIR}/interfaces/ICOSEMInterface.cpp
	${CMAKE_CURRENT_LIST_DIR}/interfaces/IData.cpp
	${CMAKE_CURRENT_LIST_DIR}/interfaces/IAssociationLN.cpp
	${CMAKE_CURRENT_LIST_DIR}/serialwrapper/SerialWrapper.cpp
	${CMAKE_CURRENT_LIST_DIR}/tcpwrapper/TCPWrapper.cpp
	)

add_library(DLMS-COSEM ${libDLMS-COSEM-SRC-FILES})


