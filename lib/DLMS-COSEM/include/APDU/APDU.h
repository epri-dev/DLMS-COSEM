#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "APDUComponent.h"
#include "COSEMAddress.h"

namespace EPRI
{
    class IAPDU
    {
    public:
        virtual ~IAPDU()
        {
        }
        virtual void RegisterComponent(IAPDUComponent * pComponent) = 0;
        virtual std::vector<uint8_t> GetBytes() = 0;
        virtual bool Parse(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress,
            DLMSVector * pData) = 0;
        virtual bool IsValid() const = 0;
        virtual ASN::TagIDType GetTag() = 0;
        virtual void Clear() = 0;
        virtual COSEMAddressType GetSourceAddress()
        {
            return m_SourceAddress;
        }
        
        virtual COSEMAddressType GetDestinationAddress()
        {
            return m_DestinationAddress;
        }

    protected:
        COSEMAddressType m_SourceAddress = ReservedAddresses::NO_STATION;
        COSEMAddressType m_DestinationAddress = ReservedAddresses::NO_STATION;

    };
    
    typedef std::unique_ptr<IAPDU> IAPDUPtr;
    
    template <ASN::TagIDType TAG>
    class APDU : public IAPDU
    {
        using ComponentVector = std::vector<IAPDUComponent *>;
        
    public:
        static const ASN::TagIDType Tag = TAG;
        
        virtual ~APDU()
        {
        }
        //
        // IAPDU
        //
        virtual ASN::TagIDType GetTag()
        {
            return Tag;
        }
        
        virtual void Clear()
        {
            for (int Index = 0; Index < m_Components.size(); ++Index)
            {
                m_Components[Index]->Clear();
            }
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
                if (!m_Components[Index]->AppendToVector(&ComponentData))
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
        
        virtual bool Parse(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress,
            DLMSVector * pData)
        {
            bool   RetVal = false;
            size_t Length = 0;
            
            if (Tag == pData->Peek<uint8_t>() &&
                pData->Skip(sizeof(uint8_t)) &&
                ASNType::GetLength(pData, &Length) &&
                (pData->Size() - pData->GetReadPosition()) >= Length)
            {
                size_t                    ComponentsProcessed = 0;
                ComponentVector::iterator it = m_Components.begin();
                while (it != m_Components.end())
                {
                    RetVal = (*it++)->Parse(pData);
                    if (RetVal)
                    {
                        ComponentsProcessed++;
                    }
                }
                RetVal = ComponentsProcessed;
            }
            if (RetVal)
            {
                m_SourceAddress = SourceAddress;
                m_DestinationAddress = DestinationAddress;
            }
            return RetVal;
        }

    protected:
        APDU()
        {
        }
        
        ComponentVector  m_Components;
        
    };
    
    template <ASN::TagIDType TAG>
    class APDUSingleType : public IAPDU
    {
    public:
        static const ASN::TagIDType Tag = TAG;
        
        APDUSingleType() = delete;
        virtual ~APDUSingleType()
        {
        }
        //
        // IAPDU
        //
        virtual ASN::TagIDType GetTag()
        {
            return Tag;
        }
        
        virtual void Clear()
        {
            m_Type.Clear();
        }
        
        virtual std::vector<uint8_t> GetBytes()
        {
            DLMSVector RetVal;
            RetVal.Append<uint8_t>(Tag);
            RetVal.Append(m_Type.GetBytes());
            return RetVal.GetBytes();            
        }
        
        virtual bool Parse(COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress,
            DLMSVector * pData)
        {
            if (Tag == pData->Peek<uint8_t>() &&
                pData->Skip(sizeof(uint8_t)))
            {
                if (m_Type.Parse(pData))
                {
                    m_SourceAddress = SourceAddress;
                    m_DestinationAddress = DestinationAddress;
                    return true;
                }
            }
            return false;            
        }
        
        // NOTE: DERIVED CLASSES MUST IMPLEMENT IsValid()

    protected:
        APDUSingleType(ASN::SchemaEntryPtr SchemaEntry) :
            m_Type(SchemaEntry)
        {
        }
        
        ASNType          m_Type;
       
    private:
        virtual void RegisterComponent(IAPDUComponent * /*pComponent*/) final
        {
            // NOT IMPLEMENTED AND VISIBILITY CHANGE
        }
        
    };

    
}