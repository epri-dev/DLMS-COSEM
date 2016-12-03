#include "interfaces/IData.h"

namespace EPRI
{
    COSEM_BEGIN_SCHEMA(IData_0::Data_Schema)
        COSEM_BEGIN_CHOICE
            COSEM_NULL_DATA_TYPE
            COSEM_BIT_STRING_TYPE
            COSEM_DOUBLE_LONG_TYPE
            COSEM_DOUBLE_LONG_UNSIGNED_TYPE
            COSEM_OCTET_STRING_TYPE
            COSEM_VISIBLE_STRING_TYPE
            COSEM_UTF8_STRING_TYPE
            COSEM_INTEGER_TYPE
            COSEM_LONG_TYPE
            COSEM_UNSIGNED_TYPE
            COSEM_LONG_UNSIGNED_TYPE
            COSEM_LONG64_TYPE
            COSEM_LONG64_UNSIGNED_TYPE
            COSEM_FLOAT32_TYPE
            COSEM_FLOAT64_TYPE
        COSEM_END_CHOICE
    COSEM_END_SCHEMA
    //
    // IData_0
    //
    IData_0::IData_0() : ICOSEMInterface(CLSID_IData, 0)
    {
        COSEM_BEGIN_ATTRIBUTES
            COSEM_ATTRIBUTE(value)
        COSEM_END_ATTRIBUTES
    }
        
    IData_0::~IData_0()
    {
    }
    //
    // IDataObject
    //
    IDataObject::IDataObject(const COSEMObjectInstanceCriteria& OIDCriteria, 
                             uint16_t ShortNameBase /* = std::numeric_limits<uint16_t>::max() */) : 
        ICOSEMObject(OIDCriteria, ShortNameBase)
    {
    }
        
    IDataObject::~IDataObject()
    {
    }
    
}