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
            std::vector<uint8_t> RetVal;
            std::vector<uint8_t> ComponentData;
            
            RetVal.push_back(Tag);
            for (int Index = 0; Index < m_Components.size(); ++Index)
            {
                if (!m_Components[Index]->Append(&ComponentData))
                {
                    return std::vector<uint8_t>();
                }
            }
            ASNType::AppendLength(ComponentData.size(), &RetVal);
            RetVal.reserve(RetVal.size() + ComponentData.size());
            RetVal.insert(RetVal.end(), ComponentData.begin(), ComponentData.end());

            return RetVal;            
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