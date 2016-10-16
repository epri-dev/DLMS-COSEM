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
            : 
            m_MyClient(HDLCAddress(0x02), &TestSerial, HDLCOptions({ 3 })),
            m_MyServer(HDLCAddress(0x03), &TestSerial, HDLCOptions({ 3 }))
        {
        }
        
    protected:
        virtual void SetUp()
        {
            //
            // Get them connected...
            //
            bool bConfirmation = false;
            HDLCClientLLC::CallbackFunction ConnectConfirm = [&](const BaseCallbackParameter& _) -> bool
            {
                bConfirmation = true;
                return true;
            };
            
            this->RegisterTransport(m_MyClient);

            m_MyClient.RegisterConnectConfirm(ConnectConfirm);
            m_MyServer.RegisterConnectIndication(ConnectConfirm);
            EXPECT_EQ(RUN_WAIT, m_MyClient.ConnectRequest(DLConnectRequestOrIndication(HDLCAddress(0x03))));
            //
            // Run the server...
            //
            EXPECT_EQ(RUN_WAIT, m_MyServer.Process());
            m_MyServer.ConnectResponse(DLConnectConfirmOrResponse(HDLCAddress(0x02)));
            //
            // Run the client...
            //
            EXPECT_EQ(RUN_WAIT, Process());
            ASSERT_TRUE(bConfirmation);
            
        }
        
        HDLCClientLLC m_MyClient;
        HDLCServerLLC m_MyServer;
    };
    
    TEST_F(COSEMClientFixture, RegisterTransport)
    {
    }
    
}