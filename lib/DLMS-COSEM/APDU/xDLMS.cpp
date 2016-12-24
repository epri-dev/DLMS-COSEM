#include "APDU/xDLMS.h"

namespace EPRI
{
    namespace xDLMS
    {
        //
        // Context
        //
        Context::Context()
        {
        }
        
        Context::Context(uint16_t APDUSize,
            const ConformanceBitsType& Conformance,
            uint8_t DLMSVersion /*= APDUConstants::CURRENT_DLMS_VERSION*/,
            const QOSType& QOS /*= DLMSOptionalNone*/)
            : m_Initialized(true)
            , m_APDUSize(APDUSize)
            , m_Conformance(Conformance)
            , m_DLMSVersion(DLMSVersion)
            , m_QOS(QOS)
        {
        }
        
        Context::~Context()
        {
        }
        
        void Context::Clear()
        {
            m_Initialized = false;
            m_APDUSize = 0;
            m_Conformance = 0;
            m_DLMSVersion = 0;
            m_DedicatedKey = DLMSOptionalNone;
            m_QOS = DLMSOptionalNone;
        }

        bool Context::Initialized() const
        {
            return m_Initialized;
        }
        
        uint16_t Context::APDUSize() const
        {
            return m_APDUSize;
        }
        
        ConformanceBitsType Context::ConformanceBits() const
        {
            return m_Conformance;
        }
        
        uint8_t Context::DLMSVersion() const
        {
            return m_DLMSVersion;
        }
        
        DLMSVariant Context::DedicatedKey() const
        {
            if (m_DedicatedKey)
            {
                return m_DedicatedKey.value();
            }
            return DLMSBlank;
        }
        
        DLMSVariant Context::QOS() const
        {
            if (m_QOS)
            {
                return m_QOS.value();
            }
            return DLMSBlank;
        }
        //
        // InitiateRequest
        //
        ASN_BEGIN_SCHEMA(InitiateRequest::InitiateRequest_Schema)
            ASN_BEGIN_SEQUENCE
                ASN_OCTET_STRING_TYPE(ASN::OPTIONAL)
                ASN_BOOLEAN_TYPE(ASN::OPTIONAL)
                ASN_BASE_TYPE_WITH_OPTIONS(ASN::DT_Integer8, ASN::IMPLICIT | ASN::OPTIONAL)
                ASN_BASE_TYPE(ASN::DT_Unsigned8)
                // 
                // Encoding Bytes
                //
                ASN_BASE_TYPE(ASN::DT_Unsigned16)
                ASN_BASE_TYPE(ASN::DT_Unsigned8)
                //
                ASN_BIT_STRING_TYPE(ASN::IMPLICIT, 24)
                ASN_BASE_TYPE(ASN::DT_Unsigned16)
            ASN_END_SEQUENCE
        ASN_END_SCHEMA
            
        InitiateRequest::InitiateRequest() :
            InitiateBase(InitiateRequest_Schema)
        {
        }
        
        InitiateRequest::InitiateRequest(uint16_t APDUSize,
            const ConformanceBitsType& Conformance,
            const DedicatedKeyType& DedicatedKey, 
            const ResponseAllowedType& ResponseAllowed,
            uint8_t DLMSVersion /*= APDUConstants::CURRENT_DLMS_VERSION*/,
            const QOSType& QOS /*= DLMSOptionalNone*/) :
            InitiateBase(InitiateRequest_Schema, APDUSize, Conformance, DLMSVersion, QOS),
            m_ResponseAllowed(ResponseAllowed)
        {
            m_DedicatedKey = DedicatedKey;
        }
        
        InitiateRequest::~InitiateRequest()
        {
        }
            
        DLMSVariant InitiateRequest::ResponseAllowed() const
        {
            if (m_ResponseAllowed)
            {
                return m_ResponseAllowed.value();
            }
            return DLMSBlank;
        }
        
        void InitiateRequest::Clear()
        {
            InitiateBase::Clear();
            m_ResponseAllowed = DLMSOptionalNone;
        }

        bool InitiateRequest::Serialize()
        {
            m_Type.Clear();
            DLMSSequence Sequence
            ( {
                DedicatedKey(),
                ResponseAllowed(),
                QOS(),
                DLMSVersion(),
                uint16_t(0x5F1F),
                uint8_t(0x04),
                ConformanceBits(),
                APDUSize()
              });
            return m_Type.Append(Sequence);
        }
        
        bool InitiateRequest::Deserialize()
        {
            DLMSValue Sequence;
            try
            {
                m_Type.Rewind();
                if (ASNType::GetNextResult::VALUE_RETRIEVED == m_Type.GetNextValue(&Sequence) &&
                    IsSequence(Sequence) && DLMSValueGetSequenceSize(Sequence) == 8)
                {
                    DLMSSequence& Elements = DLMSValueGetSequence(Sequence);
                    m_DedicatedKey = DLMSOptionalNone;
                    if (IsInitialized(Elements[0]))
                    {
                        m_DedicatedKey = Elements[0].get<DLMSVector>();
                    }
                    m_ResponseAllowed = DLMSOptionalNone;
                    if (IsInitialized(Elements[1]))
                    {
                        m_ResponseAllowed = Elements[1].get<bool>();
                    }
                    m_QOS = DLMSOptionalNone;
                    if (IsInitialized(Elements[2]))
                    {
                        m_QOS = Elements[2].get<bool>();
                    }
                    m_DLMSVersion = Elements[3].get<uint8_t>();
                    //
                    // Skip the APPLICATION 32 Encoding
                    //
                    m_Conformance = Elements[6].get<ConformanceBitsType>();
                    m_APDUSize = Elements[7].get<uint16_t>();
                    return true;
                }
            }
            catch (const std::exception&)
            {
            }
            return false;
        }
        //
        // InitiateResponse
        //
        ASN_BEGIN_SCHEMA(InitiateResponse::InitiateResponse_Schema)
            ASN_BEGIN_SEQUENCE
                ASN_BASE_TYPE_WITH_OPTIONS(ASN::DT_Integer8, ASN::IMPLICIT | ASN::OPTIONAL)
                ASN_BASE_TYPE(ASN::DT_Unsigned8)
                // 
                // Encoding Bytes
                //
                ASN_BASE_TYPE(ASN::DT_Unsigned16)
                ASN_BASE_TYPE(ASN::DT_Unsigned8)
                //
                ASN_BIT_STRING_TYPE(ASN::IMPLICIT, 24)
                ASN_BASE_TYPE(ASN::DT_Unsigned16)
                ASN_BASE_TYPE(ASN::DT_Unsigned16)
            ASN_END_SEQUENCE
        ASN_END_SCHEMA
            
        InitiateResponse::InitiateResponse() :
            InitiateBase(InitiateResponse_Schema)
        {
        }
        
        InitiateResponse::InitiateResponse(uint16_t APDUSize,
            const ConformanceBitsType& Conformance,
            uint8_t DLMSVersion /*= APDUConstants::CURRENT_DLMS_VERSION*/,
            bool LogicalNameReferencing /*= true*/,
            const QOSType& QOS /*= DLMSOptionalNone*/) :
            InitiateBase(InitiateResponse_Schema, APDUSize, Conformance, DLMSVersion, QOS),
            m_LogicalNameReferencing(LogicalNameReferencing)
        {
        }
        
        InitiateResponse::InitiateResponse(const InitiateRequest& Request, bool LogicalNameReferencing /*= true*/) :
            InitiateBase(InitiateResponse_Schema),
            m_LogicalNameReferencing(LogicalNameReferencing)
        {
            m_Initialized = true;
            m_APDUSize = Request.m_APDUSize;
            m_Conformance = Request.m_Conformance;
            m_DLMSVersion = Request.m_DLMSVersion;
            m_DedicatedKey = Request.m_DedicatedKey;
            m_QOS = Request.m_QOS;
        }

        InitiateResponse::~InitiateResponse()
        {
        }
            
        uint16_t InitiateResponse::VAAName() const
        {
            return m_LogicalNameReferencing ? 0x0007 : 0xFA00;
        }
        
        void InitiateResponse::Clear()
        {
            InitiateBase::Clear();
            m_LogicalNameReferencing = true;
        }
        
        bool InitiateResponse::Serialize()
        {
            m_Type.Clear();
            DLMSSequence Sequence
            ({
                QOS(),
                DLMSVersion(),
                uint16_t(0x5F1F),
                uint8_t(0x04),
                ConformanceBits(),
                APDUSize(),
                VAAName()
            });
            return m_Type.Append(Sequence);
        }
        
        bool InitiateResponse::Deserialize()
        {
            DLMSValue Sequence;
            try
            {
                m_Type.Rewind();
                if (ASNType::GetNextResult::VALUE_RETRIEVED == m_Type.GetNextValue(&Sequence) &&
                    IsSequence(Sequence) && DLMSValueGetSequenceSize(Sequence) == 7)
                {
                    DLMSSequence& Elements = DLMSValueGetSequence(Sequence);
                    m_QOS = DLMSOptionalNone;
                    if (IsInitialized(Elements[0]))
                    {
                        m_QOS = Elements[0].get<int8_t>();
                    }
                    m_DLMSVersion = Elements[1].get<uint8_t>();
                    //
                    // Skip the APPLICATION 32 Encoding
                    //
                    m_Conformance = Elements[4].get<ConformanceBitsType>();
                    m_APDUSize = Elements[5].get<uint16_t>();
                    m_LogicalNameReferencing = (Elements[6].get<uint16_t>() == 0x0007);
                    return true;
                }
            }
            catch (const std::exception&)
            {
            }
            return false;
        }
        
    }
    
}
