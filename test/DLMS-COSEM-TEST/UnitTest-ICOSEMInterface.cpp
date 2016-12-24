#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/include/interfaces/ICOSEMInterface.h"

namespace EPRI
{
    
    class ISimpleClass : public ICOSEMInterface
    {
    public:
        ISimpleClass()
            : ICOSEMInterface(0, 0)
        {
        }
        virtual ~ISimpleClass()
        {
        }
        
        enum Attributes : ObjectAttributeIdType
        {
            ATTR_SIMPLE = 2,
            ATTR_SIMPLE_TWO = 3
        };
        
        COSEMAttribute<ATTR_SIMPLE, OctetStringSchema, 0x08>     simple;
        COSEMAttribute<ATTR_SIMPLE_TWO, OctetStringSchema, 0x10> simple2;
        
    };
    
    class ISimpleObject : public ISimpleClass, public ICOSEMObject
    {
    public:
        ISimpleObject()
            : ICOSEMObject({0,1,0,0,0,0})
        {
        }
        virtual ~ISimpleObject()
        {
        }
        
    protected:
        virtual APDUConstants::Data_Access_Result InternalGet(const AssociationContext& Context,
            ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) final
        {
            return APDUConstants::Data_Access_Result::other_reason;
        }
        

    };
    
    class SimpleObjectFixture :
        public ISimpleObject,
        public::testing::Test
    {
    public:
        SimpleObjectFixture()
        {
        }
        
    protected:
        
    };
    
    TEST_F(SimpleObjectFixture, Construct)
    {
    }
    
    TEST_F(SimpleObjectFixture, Get)
    {
    }
    
    TEST_F(SimpleObjectFixture, Set)
    {
    }
    
    TEST_F(SimpleObjectFixture, Action)
    {
    }

}