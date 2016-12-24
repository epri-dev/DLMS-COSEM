#pragma once

#include "DLMSVector.h"
#include "COSEMTypes.h"
#include "APDU/APDUConstants.h"
#include "APDU/APDU.h"
#include "APDU/ASNType.h"

namespace EPRI
{
    namespace xDLMS
    {
        enum ConformanceBits
        {
            reserved_zero                    = 0,
            reserved_one                     = 1,
            reserved_two                     = 2,
            read                             = 3,
            write                            = 4,
            unconfirmed_write                = 5,
            reserved_six                     = 6,
            reserved_seven                   = 7,
            attribute0_supported_with_set    = 8,
            priority_mgmt_supported          = 9,
            attribute_0_supported_with_get   = 10,
            block_transfer_with_get_or_read  = 11,
            block_transfer_with_set_or_write = 12,
            block_transfer_with_action       = 13,
            multiple_references              = 14,
            information_report               = 15,
            reserved_sixteen                 = 16,
            reserved_seventeen               = 17,
            parameterized_access             = 18,
            get                              = 19,
            set                              = 20,
            selective_access                 = 21,
            event_notification               = 22,
            action                           = 23,
            max_bits                         = 24
        };
        
        using ConformanceBitsType = DLMSBitSet;
        
        const ConformanceBitsType AvailableStackConformance
        (
            1 << ConformanceBits::get |
            1 << ConformanceBits::set |
            1 << ConformanceBits::action
        );
        
        class Context
        {
        public:
            typedef DLMSOptional<DLMSVector> DedicatedKeyType;
            typedef DLMSOptional<int8_t>     QOSType;
            
            Context();
            Context(uint16_t APDUSize,
                const ConformanceBitsType& Conformance = AvailableStackConformance,
                uint8_t DLMSVersion = APDUConstants::CURRENT_DLMS_VERSION,
                const QOSType& QOS = DLMSOptionalNone);
            virtual ~Context();

            void Clear();
            bool Initialized() const;
            uint16_t APDUSize() const;
            ConformanceBitsType ConformanceBits() const;
            uint8_t DLMSVersion() const;
            DLMSVariant DedicatedKey() const;
            DLMSVariant QOS() const;
            
        protected:
            bool                m_Initialized = false;
            uint16_t            m_APDUSize = 0;
            ConformanceBitsType m_Conformance;
            uint8_t             m_DLMSVersion = 0;
            DedicatedKeyType    m_DedicatedKey;
            QOSType             m_QOS;
        };
        
        template <ASN::TagIDType TAG>
            class InitiateBase : public Context, public APDUSingleType<TAG>
            {
            public:
                virtual ~InitiateBase()
                {
                }
                //
                // APDUSingleType
                //
                virtual std::vector<uint8_t> GetBytes() final
                {
                    if (Serialize())
                    {
                        return APDUSingleType<TAG>::GetBytes();
                    }
                    return std::vector<uint8_t>();
                }
                bool Parse(DLMSVector * pData)
                {
                    if (APDUSingleType<TAG>::Parse(pData))
                    {
                        if (Deserialize())
                        {
                            return (m_Initialized = true);
                        }
                    }
                    return false;
                }
                virtual void Clear()
                {
                    APDUSingleType<TAG>::Clear();
                    Context::Clear();
                }

            protected:
                InitiateBase(ASN::SchemaEntryPtr SchemaEntry)
                    : APDUSingleType<TAG>::APDUSingleType(SchemaEntry)
                {
                }
                InitiateBase(ASN::SchemaEntryPtr SchemaEntry,
                    uint16_t APDUSize,
                    const ConformanceBitsType& Conformance = AvailableStackConformance,
                    uint8_t DLMSVersion = APDUConstants::CURRENT_DLMS_VERSION,
                    const QOSType& QOS = DLMSOptionalNone)
                    : Context(APDUSize, Conformance, DLMSVersion, QOS)
                    , APDUSingleType<TAG>::APDUSingleType(SchemaEntry)
                {
                }
                //
                // IAPDU
                //
                virtual bool IsValid() const final
                {
                    return true;
                }
                
                virtual bool Serialize() = 0;
                virtual bool Deserialize() = 0;
            
            };
        
        class InitiateResponse;
        
        class InitiateRequest : public InitiateBase<1>
        {
            ASN_DEFINE_SCHEMA(InitiateRequest_Schema)
            friend class InitiateResponse;
            
        public:
            typedef DLMSOptional<bool> ResponseAllowedType;
            
            InitiateRequest();
            InitiateRequest(uint16_t APDUSize,
                const ConformanceBitsType& Conformance = AvailableStackConformance,
                const DedicatedKeyType& DedicatedKey = DLMSOptionalNone,
                const ResponseAllowedType& ResponseAllowed = DLMSOptionalNone,
                uint8_t DLMSVersion = APDUConstants::CURRENT_DLMS_VERSION,
                const QOSType& QOS = DLMSOptionalNone);
            virtual ~InitiateRequest();
                        
            DLMSVariant ResponseAllowed() const;
            //
            // InitiateBase
            //
            virtual void Clear() final;

        protected:
            virtual bool Serialize();
            virtual bool Deserialize();
            
            ResponseAllowedType m_ResponseAllowed;
        };
    
        class InitiateResponse : public InitiateBase<8>
        {
            ASN_DEFINE_SCHEMA(InitiateResponse_Schema)

        public:
            InitiateResponse();
            InitiateResponse(const InitiateRequest& Request, bool LogicalNameReferencing = true);
            InitiateResponse(uint16_t APDUSize,
                const ConformanceBitsType& Conformance = AvailableStackConformance,
                uint8_t DLMSVersion = APDUConstants::CURRENT_DLMS_VERSION,
                bool LogicalNameReferencing = true,
                const QOSType& QOS = DLMSOptionalNone);
            virtual ~InitiateResponse();
            
            uint16_t VAAName() const;
            //
            // InitiateBase
            //
            virtual void Clear() final;
            
        protected:
            virtual bool Serialize();
            virtual bool Deserialize();
            
            bool m_LogicalNameReferencing = true;
        };        
    }

}
