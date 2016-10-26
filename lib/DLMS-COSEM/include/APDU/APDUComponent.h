#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>

#include "ASNType.h"
#include "APDUConstants.h"

namespace EPRI
{
#define ASN_BEGIN_COMPONENTS
        
#define ASN_COMPONENT(COMP)\
        RegisterComponent(&COMP);
        
#define ASN_END_COMPONENTS
   
    extern const ASNType ASNMissing;  

    class IAPDUComponent
    {
        template <ASN::TagIDType TAG> 
            friend class APDU;
        
    public:
        virtual ~IAPDUComponent() 
        {
        }
        virtual bool IsValid() const = 0;
        
    protected:
        virtual bool Append(std::vector<uint8_t> * pVector) = 0;
    };

    template <ASN::TagIDType TAGID, ASN::SchemaType DT, 
        ASN::ComponentOptionType OPT = ASN::NO_OPTIONS, const ASNType& DEFAULT = ASNMissing>    
    class APDUComponent : public IAPDUComponent, public ASNType
    {
    public:
        const ASN::TagIDType           Tag = TAGID;
        const ASN::ComponentOptionType Options = OPT;
        const uint8_t                  APDUTagClass = 0x80;
        
        APDUComponent()
            : ASNType(DT)
        {
        }
        
        virtual ~APDUComponent()
        {
        }
        //
        // ASNType
        //
        virtual std::vector<uint8_t> GetBytes()
        {
            return GetBytes(APDUTagClass | Tag, Options);
        }
        
        virtual bool Append(const ASNType& Value)
        {
            return ASNType::Append(Value);
        }
        //
        // Operators
        //
        operator const std::vector<uint8_t>()
        {
            return GetBytes();
        }
        
        bool operator==(const std::vector<uint8_t>& rhs)
        {
            return GetBytes() == rhs;
        }
        //
        // IAPDUComponent
        //
        virtual bool IsValid() const
        {
            bool RetVal = (!(Options & ASN::OPTIONAL) &&
                            IsEmpty());
            if (RetVal)
            {
                
            }
            return RetVal;
        }
       
    protected:
        //
        // IAPDUComponent
        //
        virtual bool Append(std::vector<uint8_t> * pVector)
        {
            try
            {
                std::vector<uint8_t> Current = GetBytes();
                pVector->reserve(pVector->size() + Current.size());
                pVector->insert(pVector->end(), Current.begin(), Current.end());
            }
            catch (...)
            {
                return false;
            }
            return true;
        }
        
    private:
        //
        // ASNType (Change Visibility)
        //
        virtual std::vector<uint8_t> GetBytes(ASN::TagIDType Tag, ASN::ComponentOptionType Options)
        {
            return ASNType::GetBytes(Tag, Options);
        }
        
    };
    
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::CONSTRUCTED>
        using APDU_application_context_name = APDUComponent<TAGID, ASN::ObjectIdentifierSchema, OPT>;
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::CONSTRUCTED | ASN::OPTIONAL>
        using APDU_AP_title = APDUComponent<TAGID, ASN::OctetStringSchema, OPT>;
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::CONSTRUCTED | ASN::OPTIONAL>
        using APDU_AE_qualifier = APDUComponent<TAGID, ASN::OctetStringSchema, OPT>;
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::CONSTRUCTED | ASN::OPTIONAL>
        using APDU_AP_invocation_identifier = APDUComponent<TAGID, ASN::IntegerSchema, OPT>;
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::CONSTRUCTED | ASN::OPTIONAL>
        using APDU_AE_invocation_identifier = APDUComponent<TAGID, ASN::IntegerSchema, OPT>;
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::OPTIONAL | ASN::IMPLICIT>
        using APDU_Mechanism_name = APDUComponent<TAGID, ASN::ObjectIdentifierSchema, OPT>;
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::OPTIONAL | ASN::IMPLICIT>
        using APDU_Implementation_data = APDUComponent<TAGID, ASN::GraphicStringSchema, OPT>;
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::CONSTRUCTED | ASN::OPTIONAL | ASN::EXPLICIT>
        using APDU_Association_information = APDUComponent<TAGID, ASN::OctetStringSchema, OPT>;
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::IMPLICIT | ASN::OPTIONAL>
        using APDU_Protocol_Version = APDUComponent<TAGID, APDUConstants::protocol_version_Schema, OPT, 
                                                    APDUConstants::protocol_version_default>;
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::OPTIONAL | ASN::IMPLICIT>
        using APDU_ACSE_Requirements = APDUComponent<TAGID, APDUConstants::acse_requirements_Schema, OPT>;
    template <ASN::TagIDType TAGID, ASN::ComponentOptionType OPT = ASN::CONSTRUCTED | ASN::EXPLICIT | ASN::OPTIONAL>
        using APDU_Authentication_Value = APDUComponent<TAGID, APDUConstants::authentication_value_Schema, OPT>;

}