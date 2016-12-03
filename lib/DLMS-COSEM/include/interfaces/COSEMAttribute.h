#pragma once

#include "DLMSVector.h"
#include "COSEMData.h"
#include "COSEMTypes.h"

namespace EPRI
{
#define COSEM_BEGIN_ATTRIBUTES
        
#define COSEM_ATTRIBUTE(ATTR)\
        RegisterAttribute(&ATTR);
        
#define COSEM_END_ATTRIBUTES

    class SelectiveAccess;
    class ICOSEMInterface;
    
    class ICOSEMAttribute : public COSEMType
    {
        friend class ICOSEMInterface;
        
    public:
        ICOSEMAttribute() = delete;
        ICOSEMAttribute(ObjectAttributeIdType Attr, 
                        ShortNameOffsetType ShortName,
                        SchemaType DT) :
           AttributeID(Attr),
           ShortNameOffset(ShortName),
           COSEMType(DT)
        {
        }
        virtual ~ICOSEMAttribute()
        {
        }
            
        const ObjectAttributeIdType AttributeID;
        const ShortNameOffsetType   ShortNameOffset;
        
        inline ICOSEMInterface * GetInterface() const
        {
            return m_pInterface;
        }
        
    protected:
        ICOSEMInterface * m_pInterface = nullptr;
        
    };
    
    template <ObjectAttributeIdType Attr, SchemaType DT, ShortNameOffsetType SNO>
        class COSEMAttribute : public ICOSEMAttribute
        { 
        public:
            COSEMAttribute() :
                ICOSEMAttribute(Attr, SNO, DT)
            {
            }
            virtual ~COSEMAttribute()
            {
            }
            
            ICOSEMAttribute& operator=(const DLMSVector& rhs)
            {
                *dynamic_cast<COSEMType *>(this) = rhs;
                return *this;
            }

        };

}