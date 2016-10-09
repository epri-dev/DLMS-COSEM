# DLMS-COSEM

The goal of this project is to create an open reference implementation of a DLMS-COSEM compliant communications stack. 

** IMPORTANT - This is a work in progress and is rapidly changing. ** 

To build:

mkdir bin
cd bin
cmake ..
make

To run:

Open a console.

	socat PTY,link=/tmp/ttyS10 PTY,link=/tmp/ttyS11

Open another console for the Server.

	cd [DLMS-COSEM Directory]/bin/src/Linux
	./Linux SERVER

Open another console for the Client.

	cd [DLMS-COSEM Directory]/bin/src/Linux
	./Linux

To run unit tests.

	cd [DLMS-COSEM Directory]/bin/test/DLMS-COSEM-TEST
	./DLMS-COSEM-TEST



