#pragma once

#include <cstdint>
#include <vector>

#include "APDUComponent.h"

namespace EPRI
{
    template <ASN::TagIDType TAG>
    class APDU
    {
        using ComponentVector = std::vector<IAPDUComponent *>;
        
    public:
        const ASN::TagIDType Tag = TAG;
        
        virtual ~APDU()
        {
        }
        
        virtual void RegisterComponent(IAPDUComponent * pComponent)
        {
            m_Components.push_back(pComponent);
        }
        
        virtual std::vector<uint8_t> GetBytes()
        {
            DLMSVector RetVal;
            DLMSVector ComponentData;
            
            RetVal.Append<uint8_t>(Tag);
            for (int Index = 0; Index < m_Components.size(); ++Index)
            {
                if (!m_Components[Index]->Append(&ComponentData))
                {
                    return std::vector<uint8_t>();
                }
            }
            ASNType::AppendLength(ComponentData.Size(), &RetVal);
            RetVal.Append(ComponentData);

            return RetVal.GetBytes();            
        }
        
        virtual bool IsValid() const
        {
            bool RetVal = false;
            for (int Index = 0; Index < m_Components.size(); ++Index)
            {
                if (!(RetVal = m_Components[Index]->IsValid()))
                {
                    break;
                }
            }
            return RetVal;
        }

    protected:
        APDU()
        {
        }
        
        ComponentVector m_Components;
        
    };
    
}