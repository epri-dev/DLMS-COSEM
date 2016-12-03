#pragma once

#include <cstring>

#include "DLMSVector.h"

namespace EPRI
{
    class COSEMObjectInstanceID;
    
    constexpr uint8_t COSEMValueGroupElements = 6;
    
    class COSEMObjectInstanceCriteria
    {
        friend class COSEMObjectInstanceID;
        
    public:
        static constexpr uint16_t ANY = 0x0100;
        static constexpr uint16_t MAX = 0xFFFF;
        
        struct ValueGroupCriteria
        {
            ValueGroupCriteria()
            {
                Type = TYPE_PRECISE;
                Value.ValueGroup = ANY;
            }
            ValueGroupCriteria(uint16_t VG)
            {
                Type = TYPE_PRECISE;
                Value.ValueGroup = VG;
            }
            ValueGroupCriteria(uint16_t L, uint16_t H)
            {
                Type = TYPE_RANGE;
                Value.ValueRange.Low = L;
                Value.ValueRange.High = H;
            }
            enum CriteriaType
            {
                TYPE_PRECISE,
                TYPE_RANGE
            }                           Type;
            union 
            {
                uint16_t ValueGroup;
                struct Range
                {
                    uint16_t Low;
                    uint16_t High;
                }        ValueRange;
            }                           Value;
            
        };
        
        COSEMObjectInstanceCriteria() = delete;
        COSEMObjectInstanceCriteria(const std::initializer_list<ValueGroupCriteria>& List);
        virtual ~COSEMObjectInstanceCriteria();
        
        virtual bool Match(const COSEMObjectInstanceID& InstanceID) const;
        
    protected:
        ValueGroupCriteria m_Criteria[COSEMValueGroupElements];
        
    };
    
    class COSEMObjectInstanceID
    {
        using InstanceIDList = std::initializer_list<uint8_t>;

    public:
        COSEMObjectInstanceID();
        COSEMObjectInstanceID(const DLMSVector& Vector);
        COSEMObjectInstanceID(InstanceIDList List);
        virtual ~COSEMObjectInstanceID();
        
        enum ValueGroup : uint8_t
        {
            VALUE_GROUP_A = 0,
            VALUE_GROUP_B = 1,
            VALUE_GROUP_C = 2,
            VALUE_GROUP_D = 3,
            VALUE_GROUP_E = 4,
            VALUE_GROUP_F = 5
        };
        inline uint8_t GetValueGroup(ValueGroup Grp) const
        {
            return m_OBIS[Grp];
        }
        
        bool IsEmpty() const;
        bool Parse(DLMSVector * pVector);
        bool Parse(const std::string& String);
        std::string ToString() const;
        //
        // Operators
        //
        bool operator==(const COSEMObjectInstanceID& LHS) const;
        bool operator!=(const COSEMObjectInstanceID& LHS) const;
        operator DLMSVector() const;
        
    protected:
        uint8_t m_OBIS[COSEMValueGroupElements] = { };
        
    };
   
}
