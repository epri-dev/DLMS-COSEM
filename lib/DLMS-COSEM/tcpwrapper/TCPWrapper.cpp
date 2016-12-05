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
        m_pSocket->RegisterConnectHandler(
            std::bind(&TCPWrapper::Socket_Connect, this, std::placeholders::_1));
        m_pSocket->RegisterReadHandler(
            std::bind(&TCPWrapper::Socket_Receive, this, std::placeholders::_1, std::placeholders::_2));
        m_pSocket->RegisterCloseHandler(
            std::bind(&TCPWrapper::Socket_Close, this, std::placeholders::_1));
    }
    
    TCPWrapper::~TCPWrapper()
    {
    }
    	
    Transport::ProcessResultType TCPWrapper::Process()
    {
        return true;
    }
	
    bool TCPWrapper::Send(const DLMSVector& Data)
    {
        return m_pSocket->Write(Data) == SUCCESSFUL;
    }
    
    bool TCPWrapper::Receive(DLMSVector * pData)
    {
        //
        // Not implemented
        //
        return false;
    }
    
    void TCPWrapper::Socket_Receive(ERROR_TYPE Error, size_t BytesReceived)
    {
        if (SUCCESSFUL == Error || BytesReceived)
        {
            m_pSocket->AppendAsyncReadResult(&m_RXVector, BytesReceived);
            switch (m_ReadState)
            {
            case HEADER_WAIT:
                if (m_RXVector.Size() == Wrapper::HEADER_SIZE)
                {
                    ArmAsyncRead(ParseMessageLength(m_RXVector));
                    m_ReadState = BODY_WAIT;
                }
                break;
            case BODY_WAIT:
                if (m_RXVector.Size() == Wrapper::HEADER_SIZE + ParseMessageLength(m_RXVector))
                {
                    ProcessReception(&m_RXVector);
                    //
                    // Rearm Reception
                    //
                    m_RXVector.Clear();
                    ArmAsyncRead();
                    m_ReadState = HEADER_WAIT;
                }
                break;
            }
        }
    }
    
    void TCPWrapper::Socket_Connect(ERROR_TYPE Error)
    {
        if (!Error)
        {
            FireTransportEvent(Transport::TRANSPORT_CONNECTED);
            //
            // Arm read to catch a header at least
            //
            ArmAsyncRead();
        }
    }
    
    void TCPWrapper::Socket_Close(ERROR_TYPE Error)
    {
        FireTransportEvent(Transport::TRANSPORT_DISCONNECTED);
    }
    
    bool TCPWrapper::ArmAsyncRead(size_t MinimumSize /* = Wrapper::HEADER_SIZE*/)
    {
        return SUCCESSFUL == m_pSocket->Read(nullptr, MinimumSize);
    }
    	
}