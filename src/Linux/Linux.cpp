#include <iostream>
#include <cstdio>
#include <time.h>

#include "LinuxBaseLibrary.h"

#include "HDLCLLC.h"

using namespace std;
using namespace EPRI;

int main(int argc, char *argv[])
{
	LinuxBaseLibrary bl;
	ISerial *		 pSerial = bl.GetCore()->GetSerial();

	if (argc == 1)
	{
    	bool bConfirmation = false;
    	HDLCClientLLC::CallbackFunction ConnectConfirm = [&](const BaseCallbackParameter& _) -> HDLCErrorCode
    	{
        	printf("CLIENT: Connection established...\n");
        	bConfirmation = true;
        	return SUCCESS;
    	};

		pSerial->Open(LinuxSerial::SERIAL_PORT_1);
		pSerial->SetOptions(LinuxSerial::Options(LinuxSerial::Options::BAUD_9600));
    	
    	HDLCClientLLC Client(HDLCAddress(0x02), pSerial, HDLCOptions({ 3 }));
    	Client.RegisterConnectConfirm(ConnectConfirm);

    	printf("CLIENT: Connecting...\n");
    	Client.ConnectRequest(ConnectRequestOrIndication(HDLCAddress(0x01)));
    	while (Client.Process() == RUN_WAIT)
    	{
        	if (bConfirmation)
        	{
            	printf("CLIENT: Sending data...\n");
            	const uint8_t SAMPLE_DATA[] = "COME HERE WATSON, I NEED YOU!";
            	std::vector<uint8_t> DATA(SAMPLE_DATA,
                	SAMPLE_DATA + sizeof(SAMPLE_DATA));
            	Client.DataRequest(DataRequestParameter(HDLCAddress(0x01), HDLCControl::INFO, DATA));
            	bConfirmation = false;
            }
    	}
	}
	else
	{
    	bool bIndication = false;
    	HDLCServerLLC::CallbackFunction ConnectIndication = [&](const BaseCallbackParameter& _) -> HDLCErrorCode
    	{
        	printf("SERVER: Connect indication...\n");
        	bIndication = true;
        	return SUCCESS;
    	};
    	HDLCServerLLC::CallbackFunction DataIndication = [&](const BaseCallbackParameter& Param) -> HDLCErrorCode
    	{
        	const DataRequestParameter& Data = dynamic_cast<const DataRequestParameter&>(Param);
        	printf("SERVER: Data indication...\n");
        	std::for_each(Data.Data.begin(),
            	Data.Data.end(), 
            	[](uint8_t B)
            	{
                	printf("%02X [%c] ", B, B);
            	});
        	printf("\n");
        	return SUCCESS;
    	};


    	pSerial->Open(LinuxSerial::SERIAL_PORT_2);
    	pSerial->SetOptions(LinuxSerial::Options(LinuxSerial::Options::BAUD_9600));
   
    	HDLCServerLLC Server(HDLCAddress(0x01), pSerial, HDLCOptions({ 3 }));
    	Server.RegisterConnectIndication(ConnectIndication);
    	Server.RegisterDataIndication(DataIndication);
    	printf("SERVER: Waiting for connect...\n");
    	while (Server.Process() == RUN_WAIT)
    	{
        	if (bIndication)
        	{
            	printf("SERVER: Approving connect...\n");
            	Server.ConnectResponse(ConnectConfirmOrResponse(HDLCAddress(0x02)));
            	bIndication = false;
        	}
    	}
	}

}