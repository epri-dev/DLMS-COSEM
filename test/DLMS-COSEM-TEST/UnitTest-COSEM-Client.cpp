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
        COSEMClientFixture() : 
            COSEMClient(0x02),
            m_MyClient(HDLCAddress(0x02), &TestSerial, HDLCOptions({ false, 3, 500 })),
            m_MyServer(HDLCAddress(0x03), &TestSerial, HDLCOptions({ false, 3, 500 }))
        {
        }
        
    protected:
        virtual void SetUp()
        {
            //
            // Register the transport with COSEM...
            //
            RegisterTransport(&m_MyClient);
        }
        
        HDLCClientLLC m_MyClient;
        HDLCServerLLC m_MyServer;
    };
    
    TEST_F(COSEMClientFixture, APPConnectRequestFailBeforeDLConnect)
    {
        // We should not be able to perform a COSEM-CONNECT until
        // we are connected at the transport.
        //
        ASSERT_FALSE(OpenRequest(APPOpenRequestOrIndication(0x02, 0x03)));
    }
    
    TEST_F(COSEMClientFixture, ConnectRequest)
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
        //
        // Register the other callbacks...
        //
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
        EXPECT_EQ(RUN_WAIT, m_MyClient.Process());
        ASSERT_TRUE(bConfirmation);
        //
        // We are DL connected, we can now issue an APP Connect
        //
        ASSERT_TRUE(OpenRequest(APPOpenRequestOrIndication(0x02, 0x03)));
    }

}