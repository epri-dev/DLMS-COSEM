#pragma once

#include <functional>
#include <utility>
#include <vector>

#include "DLMSDataTypes.h"

namespace EPRI
{
    class SelectiveAccess;
    
    using DLMSData = std::vector<uint8_t>;
    
    typedef std::function<bool(AttributeIDType, 
        const SelectiveAccess *)>                   PopulateHandler;
    typedef std::function<bool(AttributeIDType, 
        const DLMSData&, const SelectiveAccess *)>  SetHandler;    
    
#define DLMSAttribute_AssignPopulateHandler(ATTRIBUTE, HANDLER_FUNCTION)\
    ATTRIBUTE.AssignPopulateHandler(std::bind(HANDLER_FUNCTION, this, std::placeholders::_1, std::placeholders::_2))
#define DLMSAttribute_AssignSetHandler(ATTRIBUTE, HANDLER_FUNCTION)\
    ATTRIBUTE.AssignSetHandler(std::bind(HANDLER_FUNCTION, this, std::placeholders::_1, std::placeholders::_2, \
        std::placeholders::_3))
    
        
    template <int8_t Attr, SchemaType Schema, uint8_t ShortNameOffset>
        class DLMSAttribute
        {
        public:
            DLMSAttribute() 
            {
            }
            
            virtual bool Populate(const SelectiveAccess * pSelectiveAccess = nullptr)
            {
                if (m_PopulateHandler)
                {
                    return m_PopulateHandler(m_Attribute, pSelectiveAccess);
                }
                return true;
            }
            
            bool Set(const DLMSData& Data, 
                const SelectiveAccess * pSelectiveAccess = nullptr)
            {
                if (m_SetHandler)
                {
                    return m_SetHandler(m_Attribute, Data, pSelectiveAccess);
                }
                m_AttributeData = Data;
                return true;
            }
            
            void Rewind()
            {
                m_SchemaIndex = 0;
                m_AttributeData.clear();
            }
            
            bool Append(const std::string& Data)
            {
                switch (m_pSchema[m_SchemaIndex])
                {
                case DLMSDataType::OCTET_STRING:
                    m_AttributeData.push_back(DLMSDataType::OCTET_STRING);
                    m_AttributeData.push_back(Data.length());
                    m_AttributeData.insert(m_AttributeData.end(), std::begin(Data), std::end(Data));
                    return true;
                }
            }
            
            PopulateHandler AssignPopulateHandler(PopulateHandler NewHandler)
            {
                PopulateHandler Previous = m_PopulateHandler;
                m_PopulateHandler = NewHandler;
                return Previous;
            }
            
            SetHandler AssignSetHandler(SetHandler NewHandler)
            {
                SetHandler Previous = m_SetHandler;
                m_SetHandler = NewHandler;
                return Previous;
            }
            //
            // Operators
            //
            operator const DLMSData() const
            {
                // Implicit populate since we are returning our data...
                //
                Populate();
                return m_AttributeData;
            }
            bool operator==(const DLMSData& rhs)
            {
                // Implicit populate since we are returning our data...
                //
                Populate();
                return rhs == m_AttributeData;
            }
            
        protected:
            const int8_t            m_Attribute = Attr;
            SchemaType              m_pSchema = Schema;
            const uint8_t           m_ShortNameOffet = ShortNameOffset;
           
            PopulateHandler         m_PopulateHandler = nullptr;
            SetHandler              m_SetHandler = nullptr;
            DLMSData                m_AttributeData;
            uint16_t                m_SchemaIndex = 0;
            
        };

}