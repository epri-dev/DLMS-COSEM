#pragma once

#include <functional>
#include "COSEMTypes.h"
#include "Callback.h"
#include "Transport.h"

namespace EPRI
{
    class Wrapper : public Transport
    {
    public:
        static const uint16_t  CURRENT_VERSION = 0x0001;
        static const size_t    HEADER_SIZE  = (2 * sizeof(uint16_t) + 2 * sizeof(COSEMAddressType));
            
        Wrapper();
        virtual ~Wrapper();
        //
        // Transport
        //
        virtual bool DataRequest(const DataRequestParameter& Parameters);
        
    protected:
        virtual bool Send(const DLMSVector& Data) = 0;
        virtual bool Receive(DLMSVector * pData) = 0;
        virtual bool ProcessReception(DLMSVector * pData);      
        virtual ssize_t ParseMessageLength(const DLMSVector& Data);
	
    };
	
} /* namespace EPRI */
