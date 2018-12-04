# DLMS-COSEM

The goal of this project is to create an open reference implementation of a DLMS-COSEM compliant communications stack. 

To build:

mkdir bin
cd bin
cmake ..
make

To run:

Open a console.

	socat -v -x PTY,link=/tmp/ttyS10 PTY,link=/tmp/ttyS11

Open another console for the Server.

	cd [DLMS-COSEM Directory]/bin/src/Linux
	./Linux -S

Open another console for the Client.

	cd [DLMS-COSEM Directory]/bin/src/Linux
	./Linux

The Client and Server applications are menu-driven.  Start the server listening first, then TCP Connect, COSEM Open, COSEM Get, and COSEM Release.

The Server supports the following objects:

	0-0:96.1.{0-9}*255 [IData Class: 1]
		- Fully implemented for GET and SET.

	0-0:1.0.0*255 [IClock Class: 8]
		- Available with hooks for GET, SET, and ACTION.

	0-0:40.0.{0-1}*255 [IAssociationLN Class: 15]
		- Available for GET

These classes are not fully implemented, but they give the reader an indication of how to complete.  

To run unit tests.

	cd [DLMS-COSEM Directory]/bin/test/DLMS-COSEM-TEST
	./DLMS-COSEM-TEST

For the STM32 Nucleo F207ZG development board:

	Prerequisites:
	
		GCC for ARM (at least 4.8 or above) installed on a WINDOWS PC.
		STM32 ST-LINK Utility from st.com installed on a WINDOWS PC.

		Set the ARM_TOOLCHAIN_ROOT environment variable to the root of where GCC is installed.

	cd [DLMS-COSEM Directory]/src/STM32-NUCLEO-F207ZG
	make

	The output will be located in [DLMS-COSEM Directory]/src/STM32-NUCLEO-F207ZG/Debug

	You can program the development board using the ST-LINK software from STM.  Select the .bin file from 
	the Debug folder.

	The development board code is configured to be a COSEM Server.  The virtual serial port provided
	through the ST-Link interface is used for debugging purposes.  The server USART is USART6.  Connect your
	TX and RX lines to the following:
	
![USART6 Connections](https://cloud.githubusercontent.com/assets/20430436/21267710/5828c7d8-c360-11e6-94ea-61a6eb41e582.PNG)

To create doxygen documentation:

	cd [DLMS-COSEM Directory]
	doxygen

	The output will be located in [DLMS-COSEM Directory]/docs. 

For additional detailed information regarding the project, please refer to the Developer Guide.

![Developer and Maintainer Guide](DeveloperGuide.pdf)
	

