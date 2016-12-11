#pragma once

#include "DLMSVector.h"
#include "COSEMData.h"
#include "COSEMTypes.h"

namespace EPRI
{
#define COSEM_BEGIN_METHODS
        
#define COSEM_METHOD(METH)\
        RegisterMethod(&METH);
        
#define COSEM_END_METHODS

    class SelectiveAccess;
    class ICOSEMInterface;
    
    class ICOSEMMethod : public COSEMType
    {
        friend class ICOSEMInterface;
        
    public:
        ICOSEMMethod() = delete;
        ICOSEMMethod(ObjectAttributeIdType Method, 
            ShortNameOffsetType ShortName,
            SchemaType ParameterDT) : 
            MethodID(Method), 
            ShortNameOffset(ShortName), 
            COSEMType(ParameterDT)
        {
        }
        virtual ~ICOSEMMethod()
        {
        }
            
        const ObjectAttributeIdType MethodID;
        const ShortNameOffsetType   ShortNameOffset;
        
        inline ICOSEMInterface * GetInterface() const
        {
            return m_pInterface;
        }
        
    protected:
        ICOSEMInterface * m_pInterface = nullptr;
        
    };
    
    template <ObjectAttributeIdType Method, SchemaType ParameterDT, ShortNameOffsetType SNO>
        class COSEMMethod : public ICOSEMMethod
        { 
        public:
            COSEMMethod()
                : ICOSEMMethod(Method, SNO, ParameterDT)
            {
            }
            virtual ~COSEMMethod()
            {
            }
            
            ICOSEMMethod& operator=(const DLMSVector& rhs)
            {
                *dynamic_cast<COSEMType *>(this) = rhs;
                return *this;
            }

        };

}