#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/include/interfaces/IDLMSInterface.h"

namespace EPRI
{
    
    class ISimpleClass : public IDLMSInterface<0, 0>
    {
    public:
        virtual ~ISimpleClass()
        {
        }
        
        enum Attributes : AttributeIDType
        {
            ATTR_SIMPLE = 2,
            ATTR_SIMPLE_TWO = 3
        };
        
        DLMSAttribute<ATTR_SIMPLE, OctetStringSchema, 0x08>     simple;
        DLMSAttribute<ATTR_SIMPLE_TWO, OctetStringSchema, 0x10> simple2;
        
        DLMSMethod<EmptySchema, IntegerSchema, 0x20>            do_simple;
        
    };
    
    template <uint32_t OID>
        class ISimpleObject : public ISimpleClass, public IDLMSObject<OID>
        {
        
        };
    
    class SimpleObjectFixture :
        public ISimpleObject<32>,
        public::testing::Test
    {
    public:
        SimpleObjectFixture()
        {
            DLMSAttribute_AssignPopulateHandler(simple, &SimpleObjectFixture::Get_Simple);
            DLMSAttribute_AssignSetHandler(simple2, &SimpleObjectFixture::Set_Simple);
        }
        
    protected:
        bool Get_Simple(AttributeIDType ID, const SelectiveAccess * pSelectiveAccess)
        {
            switch (ID)
            {
            case ATTR_SIMPLE:
                simple.Rewind();
                return simple.Append("HELLO WORLD!");
            }
        }
        
        bool Set_Simple(AttributeIDType ID, const DLMSData& Data, const SelectiveAccess * pSelectiveAccess)
        {
        }
        
    };
    
    TEST_F(SimpleObjectFixture, Construct)
    {
    }
    
    TEST_F(SimpleObjectFixture, Get)
    {
        //
        // A GET service request is asking the responder to respond with the appropriate
        // data in the schema format.
        //
        const DLMSData DATA1 = 
            { DLMSDataType::OCTET_STRING, 12, 'H', 'E', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D', '!' };
        ASSERT_TRUE(this->simple == DATA1);
        
    }
    
    TEST_F(SimpleObjectFixture, Set)
    {
    }
    
    TEST_F(SimpleObjectFixture, Action)
    {
    }




}