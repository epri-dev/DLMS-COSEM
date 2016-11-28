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
        const uint16_t                          CURRENT_VERSION = 0x0001;
            
        Wrapper();
        virtual ~Wrapper();
        virtual bool Process() = 0;
        //
        // Transport
        //
        virtual bool DataRequest(const DataRequestParameter& Parameters);
        
    protected:
        virtual bool Send(const DLMSVector& Data) = 0;
        virtual bool Receive(DLMSVector * pData) = 0;
        virtual bool ProcessReception(DLMSVector * pData);        
	
    };
	
} /* namespace EPRI */
