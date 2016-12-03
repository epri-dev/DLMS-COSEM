#pragma once

#include "Wrapper.h"

namespace EPRI
{
    class ISocket;
    
    class TCPWrapper : public Wrapper
    {
    public:
        TCPWrapper() = delete;
        TCPWrapper(ISocket * pSocket);
        virtual ~TCPWrapper();
    	//
        // Transport
        // 
        virtual ProcessResultType Process();
	
    protected:
        bool Send(const DLMSVector& Data);
        bool Receive(DLMSVector * pData);
        
        void Socket_Connect(ERROR_TYPE Error);
        void Socket_Receive(ERROR_TYPE Error, size_t BytesReceived);
        
        bool ArmAsyncRead(size_t MinimumSize = Wrapper::HEADER_SIZE);
 
        enum TCPReadState
        {
            HEADER_WAIT,
            BODY_WAIT
        }               m_ReadState = HEADER_WAIT;
        ISocket *	    m_pSocket = nullptr;
        DLMSVector      m_RXVector;
    };

}
