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
    	
        virtual bool Process();
	
    protected:
        bool Send(const DLMSVector& Data);
        bool Receive(DLMSVector * pData);
 
        ISocket *	    m_pSocket = nullptr;
        bool            m_bConnectionFired = false;
    };

}
