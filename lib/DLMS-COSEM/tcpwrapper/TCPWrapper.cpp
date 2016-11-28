#include "ISocket.h"
#include "tcpwrapper/TCPWrapper.h"

namespace EPRI
{
    //
    // TCPWrapper
    //
    TCPWrapper::TCPWrapper(ISocket * pSocket) :
        m_pSocket(pSocket)
    {
    }
    
    TCPWrapper::~TCPWrapper()
    {
    }
    	
    bool TCPWrapper::Process()
    {
        if (!m_bConnectionFired && m_pSocket->IsConnected())
        {
            FireTransportEvent(Transport::TRANSPORT_CONNECTED);
            m_bConnectionFired = true;
        }
        DLMSVector RxData;
        if (Receive(&RxData))
        {
            ProcessReception(&RxData);
        }
        return true;
    }
	
    bool TCPWrapper::Send(const DLMSVector& Data)
    {
        return m_pSocket->Write(Data.GetData(), Data.Size()) == SUCCESSFUL;
    }
    
    bool TCPWrapper::Receive(DLMSVector * pData)
    {
        uint8_t		   Byte;
        uint32_t       CharacterTimeout = 400;
        size_t         BytesReceived = 0;
        //
        // Check to see if there is a byte available.  If so, then we can just stream 
        // until we hit a character timeout.
        //
        ERROR_TYPE     RetVal = m_pSocket->Read(&Byte, sizeof(Byte), 0);
        while (SUCCESSFUL == RetVal)
        {
            pData->Append<uint8_t>(Byte);
            BytesReceived++;
            RetVal = m_pSocket->Read(&Byte, sizeof(Byte), CharacterTimeout);
        }
        return BytesReceived;
    }
    	
}