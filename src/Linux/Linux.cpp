#include <iostream>
#include <cstdio>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <iomanip>

#include "LinuxBaseLibrary.h"

#include "HDLCLLC.h"
#include "COSEM.h"
#include "serialwrapper/SerialWrapper.h"

using namespace std;
using namespace EPRI;

int main(int argc, char *argv[])
{
    LinuxBaseLibrary     bl;
    ISerial *		     pSerial = bl.GetCore()->GetSerial();
    int                  opt;
    bool                 StartWithIEC = false;
    bool                 Server = false;
    bool                 IsSerialWrapper = false;
    char *               pCOMPortName = nullptr;
    char *               pSourceAddress = nullptr;
    char *               pDestinationAddress = nullptr;
    char *               pPassword = nullptr;
    COSEM::SecurityLevel Security = COSEM::SECURITY_NONE;
    
    while ((opt =::getopt(argc, argv, "SC:s:d:IWp:P:")) != -1)
    {
        switch (opt)
        {
        case 'I':
            StartWithIEC = true;
            break;
        case 'W':
            IsSerialWrapper = true;
            break;
        case 'S':
            Server = true;
            break;
        case 's':
            pSourceAddress = optarg;
            break;
        case 'd':
            pDestinationAddress = optarg;
            break;
        case 'C':
            pCOMPortName = optarg;
            break;
        case 'p':
            pPassword = optarg;
            Security = COSEM::SECURITY_LOW_LEVEL;
            break;
        case 'P':
            pPassword = optarg;
            Security = COSEM::SECURITY_HIGH_LEVEL;
            break;
        default:
            std::cerr << "Internal error!\n";
            return -1;
        }
    }
    
    if (Server)
    {
        std::cerr << "NOT IMPLEMENTED!\n";
        exit(-1);
    }
    else if (nullptr != pCOMPortName &&
             nullptr != pSourceAddress &&
             nullptr != pDestinationAddress)
    {
        uint8_t                     SourceAddress = uint8_t(strtol(pSourceAddress, nullptr, 16));
        uint8_t                     DestinationAddress = uint8_t(strtol(pDestinationAddress, nullptr, 16));
        ISerial::Options::BaudRate  BR = LinuxSerial::Options::BAUD_9600;
        COSEMClient                 APClient;
        HDLCClientLLC               DLClient(HDLCAddress(SourceAddress), 
                                        pSerial, HDLCOptions({ StartWithIEC, 3, 500 }));
        SerialWrapper               DLWrapper(pSerial, SerialWrapper::WrapperPorts(SourceAddress, DestinationAddress));
        
        std::cout << "Operating as a Client (0x" << hex << uint16_t(SourceAddress) << ")... Opening " << pCOMPortName << "\n";
        if (pSerial->Open(pCOMPortName) != SUCCESS ||
            pSerial->SetOptions(LinuxSerial::Options(BR)) != SUCCESS)
        {
            std::cerr << "Error opening port " << pCOMPortName << "\n";
            exit(-1);
        }

        std::cout << "Connecting to Server (0x" << std::hex << uint16_t(DestinationAddress) << ")\n";
        if (IsSerialWrapper)
        {
            bool bProcessed = false;
            
            APClient.RegisterTransport(&DLWrapper);
            while (DLWrapper.Process())
            {
                if (!bProcessed)
                {
                    APClient.OpenRequest(APPOpenRequestOrIndication(Security, pPassword == nullptr ? "" : pPassword));
                    bProcessed = true;                   
                }
            }    
        }
        else
        {
            bool                        bConfirmation = false;
            HDLCClientLLC::CallbackFunction 
                                        ConnectConfirm = [&](const BaseCallbackParameter& _) -> bool
                                        {
                                            std::cout << "Datalink connection established!\n";
                                            bConfirmation = true;
                                            return true;
                                        };
            
            DLClient.RegisterConnectConfirm(ConnectConfirm);
            APClient.RegisterTransport(&DLClient);

            DLClient.ConnectRequest(DLConnectRequestOrIndication(HDLCAddress(DestinationAddress)));
            while (DLClient.Process() == RUN_WAIT)
            {
                if (bConfirmation)
                {
                    bConfirmation = false;
                    //
                    // Now we can COSEM open!
                    //
                    APClient.OpenRequest(APPOpenRequestOrIndication());
                }
            }    
        }
    }
    
#if 0
	if (argc == 1)
	{
    	bool bConfirmation = false;
    	HDLCClientLLC::CallbackFunction ConnectConfirm = [&](const BaseCallbackParameter& _) -> HDLCErrorCode
    	{
        	printf("CLIENT: Connection established...\n");
        	bConfirmation = true;
        	return SUCCESS;
    	};

    	pSerial->Open("/tmp/ttyS10");
		pSerial->SetOptions(LinuxSerial::Options(LinuxSerial::Options::BAUD_9600));
    	
    	HDLCClientLLC Client(HDLCAddress(0x02), pSerial, HDLCOptions({ 3 }));
    	Client.RegisterConnectConfirm(ConnectConfirm);

    	printf("CLIENT: Connecting...\n");
    	Client.ConnectRequest(DLConnectRequestOrIndication(HDLCAddress(0x01)));
    	while (Client.Process() == RUN_WAIT)
    	{
        	if (bConfirmation)
        	{
            	printf("CLIENT: Sending data...\n");
            	const uint8_t SAMPLE_DATA[] = "COME HERE WATSON, I NEED YOU!";
            	std::vector<uint8_t> DATA(SAMPLE_DATA,
                	SAMPLE_DATA + sizeof(SAMPLE_DATA));
            	Client.DataRequest(DLDataRequestParameter(HDLCAddress(0x01), HDLCControl::INFO, DATA));
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
        	const DLDataRequestParameter& Data = dynamic_cast<const DLDataRequestParameter&>(Param);
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


    	pSerial->Open("/tmp/ttyS11");
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
            	Server.ConnectResponse(DLConnectConfirmOrResponse(HDLCAddress(0x02)));
            	bIndication = false;
        	}
    	}
	}
#endif

}