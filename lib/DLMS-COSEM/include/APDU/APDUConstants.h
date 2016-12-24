#pragma once

#include "ASNType.h"

namespace EPRI
{
    struct APDUConstants
    {
        static const ASNType protocol_version_default;
        static const uint8_t CURRENT_DLMS_VERSION;
        static const int8_t  DLMS_DEFAULT_QOS;
        
        ASN_DEFINE_SCHEMA(protocol_version_Schema)
        ASN_DEFINE_SCHEMA(authentication_value_Schema)
        ASN_DEFINE_SCHEMA(acse_requirements_Schema)
        
        enum AuthenticationValueChoice : int8_t
        {
            charstring = 0,
            bitstring = 1,
            external = 2,
            other = 3
        };
        
        enum class Data_Access_Result : uint8_t
        {
            success = 0,
            hardware_fault = 1,
            temporary_failure = 2,
            read_write_denied = 3,
            object_unidentified = 4,
            object_class_inconsistent = 9,
            object_unavailable = 11,
            type_unmatched = 12,
            scope_of_access_violated = 13,
            data_block_unavailable = 14,
            long_get_aborted = 15,
            no_long_get_in_progress = 16,
            long_set_aborted = 17,
            no_long_set_in_progress = 18,
            data_block_number_invalid = 19,
            other_reason = 255
        };
        
        enum class Action_Result : uint8_t
        {
            success                    = 0,
            hardware_fault             = 1,
            temporary_failure          = 2,
            read_write_denied          = 3,
            object_unidentified        = 4,
            object_class_inconsistent  = 9,
            object_unavailable         = 11,
            type_unmatched             = 12,
            scope_of_access_violated   = 13,
            data_block_unavailable     = 14,
            long_action_aborted        = 15,
            no_long_action_in_progress = 16,
            other_reason               = 255
        };
        
    };

}