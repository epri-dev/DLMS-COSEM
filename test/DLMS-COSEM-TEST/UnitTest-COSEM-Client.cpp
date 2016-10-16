#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/COSEMClient.cpp"
#include "../../lib/DLMS-COSEM/include/HDLCLLC.h"
#include "DummySerial.h"

namespace EPRI
{
    static DummySerial    TestSerial;

    class COSEMClientFixture : public COSEMClient, public::testing::Test
    {
    public:
        COSEMClientFixture()
            : m_MyClient(HDLCAddress(0x02), &TestSerial, HDLCOptions({ 3 }))
        {
        }
        
    protected:
        virtual void SetUp()
        {
        }
        
        HDLCClientLLC m_MyClient;
    };
    
    TEST_F(COSEMClientFixture, RegisterTransport)
    {
        this->RegisterTransport(m_MyClient);
    }
    
}