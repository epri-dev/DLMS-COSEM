#include <iostream>
#include <cstdio>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <iomanip>
#include <../CMSIS_RTOS/cmsis_os.h>

#include "STM32-Server.h"
#include "STM32BaseLibrary.h"
#include "STM32CosemServer.h"

#include "HDLCLLC.h"
#include "COSEM.h"

using namespace std;
using namespace EPRI;

STM32BaseLibrary    g_BL;

void RunServer()
{
    ISerialSocket * pSocket;
    
    //
    // TODO - HDLCServerLLC ServerAddress should be able to handle multiple SAPs
    //
    STM32COSEMServerEngine * pServerEngine = new STM32COSEMServerEngine(COSEMServerEngine::Options(),
        new HDLCServerLLC(HDLCAddress(0x01), 
            (pSocket = Base()->GetCore()->GetSerial()->CreateSocket(STM32Serial::Options(ISerial::Options::BaudRate::BAUD_9600))), 
            HDLCOptions())); 
    if (pSocket->Open() != SUCCESSFUL)
    {
    }

    for (;;)
    {
        osDelay(2000);
    }
    
}
