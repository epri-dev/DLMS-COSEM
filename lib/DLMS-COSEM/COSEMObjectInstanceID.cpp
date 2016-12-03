#include <cstring>
#include <algorithm>
#include <cassert>
#include <cstdio>

#include "COSEMObjectInstanceID.h"

namespace EPRI
{
    //
    // COSEMObjectInstanceCriteria
    //
    COSEMObjectInstanceCriteria::COSEMObjectInstanceCriteria(const std::initializer_list<ValueGroupCriteria>& List)
    {
        assert(List.size() == COSEMValueGroupElements);
        
        ValueGroupCriteria * pCriteria = &m_Criteria[0];
        std::for_each(List.begin(),
            List.end(), 
            [&pCriteria](const ValueGroupCriteria& Value)
            {
                *pCriteria++ = Value;
            });
    }
    
    COSEMObjectInstanceCriteria::~COSEMObjectInstanceCriteria()
    {
    }
    
    bool COSEMObjectInstanceCriteria::Match(const COSEMObjectInstanceID& InstanceID) const
    {
        uint8_t Matches = 0;
        for (uint8_t Group = COSEMObjectInstanceID::VALUE_GROUP_A;
             Group <= COSEMObjectInstanceID::VALUE_GROUP_F; ++Group)
        {
            uint8_t ValueGroupValue = InstanceID.GetValueGroup(COSEMObjectInstanceID::ValueGroup(Group));
            if (m_Criteria[Group].Type == ValueGroupCriteria::TYPE_PRECISE &&
                (m_Criteria[Group].Value.ValueGroup == ANY ||
                 ValueGroupValue == m_Criteria[Group].Value.ValueGroup))
            {
                ++Matches;
            }
            else if ((m_Criteria[Group].Type == ValueGroupCriteria::TYPE_RANGE) &&
                     ((m_Criteria[Group].Value.ValueRange.Low == ANY &&
                       m_Criteria[Group].Value.ValueRange.High == ANY) ||
                      (m_Criteria[Group].Value.ValueRange.Low == ANY &&
                         ValueGroupValue <= m_Criteria[Group].Value.ValueRange.High) ||
                      (m_Criteria[Group].Value.ValueRange.High == ANY &&
                         ValueGroupValue >= m_Criteria[Group].Value.ValueRange.Low) ||
                      (ValueGroupValue <= m_Criteria[Group].Value.ValueRange.High &&
                         ValueGroupValue >= m_Criteria[Group].Value.ValueRange.Low)))
            {
                ++Matches;
            }
        }
        return Matches == COSEMValueGroupElements;
    }
    //
    // COSEMObjectInstanceID
    //
    COSEMObjectInstanceID::COSEMObjectInstanceID()
    {
    }
    
    COSEMObjectInstanceID::COSEMObjectInstanceID(const DLMSVector& Vector)
    {
        Vector.PeekBuffer(m_OBIS, sizeof(m_OBIS));
    }
    
    COSEMObjectInstanceID::COSEMObjectInstanceID(InstanceIDList List)
    {
        assert(List.size() == COSEMValueGroupElements);
        
        uint8_t * pOBIS = &m_OBIS[0];
        std::for_each(List.begin(),
            List.end(), 
            [&pOBIS](InstanceIDList::value_type Value)
            {
                *pOBIS++ = Value;
            });
    }
    
    COSEMObjectInstanceID::~COSEMObjectInstanceID()
    {
    }

    bool COSEMObjectInstanceID::IsEmpty() const
    {
        for (int Index = 0; Index < sizeof(m_OBIS); ++Index)
        {
            if (m_OBIS[Index] != 0)
            {
                return false;
            }
        }
        return true;
    }

    bool COSEMObjectInstanceID::Parse(DLMSVector * pVector)
    {
        return pVector->GetBuffer(m_OBIS, sizeof(m_OBIS));
    }

    bool COSEMObjectInstanceID::Parse(const std::string& String)
    {
        bool RetVal = 
            std::sscanf(String.c_str(),
                "%hhu%*[.-, :*]%hhu%*[.-, :*]%hhu%*[.-, :*]%hhu%*[.-, :*]%hhu%*[.-, :*]%hhu%*[.-, :*]",
                &m_OBIS[ValueGroup::VALUE_GROUP_A],
                &m_OBIS[ValueGroup::VALUE_GROUP_B],
                &m_OBIS[ValueGroup::VALUE_GROUP_C],
                &m_OBIS[ValueGroup::VALUE_GROUP_D],
                &m_OBIS[ValueGroup::VALUE_GROUP_E],
                &m_OBIS[ValueGroup::VALUE_GROUP_F]) == COSEMValueGroupElements;
        if (!RetVal)
        {
            std::memset(m_OBIS, '\0', sizeof(m_OBIS));
        }
        return RetVal;
    }

    std::string COSEMObjectInstanceID::ToString() const
    {
        char Buffer[30];
        std::snprintf(Buffer, 
                      sizeof(Buffer), 
                      "%u-%u:%u.%u.%u*%u",
                      GetValueGroup(ValueGroup::VALUE_GROUP_A),
                      GetValueGroup(ValueGroup::VALUE_GROUP_B),
                      GetValueGroup(ValueGroup::VALUE_GROUP_C),
                      GetValueGroup(ValueGroup::VALUE_GROUP_D),
                      GetValueGroup(ValueGroup::VALUE_GROUP_E),
                      GetValueGroup(ValueGroup::VALUE_GROUP_F));
        return std::string(Buffer);
    }
    
    bool COSEMObjectInstanceID::operator==(const COSEMObjectInstanceID& LHS) const
    {
        return std::memcmp(m_OBIS, LHS.m_OBIS, sizeof(m_OBIS)) == 0;
    }

    bool COSEMObjectInstanceID::operator!=(const COSEMObjectInstanceID& LHS) const
    {
        return std::memcmp(m_OBIS, LHS.m_OBIS, sizeof(m_OBIS)) != 0;
    }

    COSEMObjectInstanceID::operator DLMSVector() const
    {
        DLMSVector RetVal;
        RetVal.AppendBuffer(m_OBIS, COSEMValueGroupElements);
        return RetVal;
    }

}