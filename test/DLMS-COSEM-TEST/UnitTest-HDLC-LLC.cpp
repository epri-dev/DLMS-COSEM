#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/include/HDLCLLC.h"
#include "../../lib/DLMS-COSEM/hdlc/HDLCLLC.cpp"
#include "DummySerial.h"

namespace EPRI
{

    static HDLCStatistics Stats;
    static DummySerial    TestSerial;

    class HDLCLLCFixture : public HDLCClientLLC, public ::testing::Test
    {
    public:
        HDLCLLCFixture()
            : HDLCClientLLC(HDLCAddress(0x02), &TestSerial, HDLCOptions({ 3 }))
            , m_MyServer(HDLCAddress(0x01), &TestSerial, HDLCOptions({ 3 }), 10)
        {
        }
        virtual void SetUp()
        {
        }
        virtual void TearDown()
        {
        }
    
        HDLCServerLLC m_MyServer;
    };

    TEST_F(HDLCLLCFixture, ConnectTest)
    {
        bool          bConfirmation = false;
        bool          bIndication = false;
        HDLCLLCFixture::CallbackFunction ConnectConfirm = [&](const BaseCallbackParameter& _) -> bool
        {
            bConfirmation = true;
            return true;
        };
        HDLCLLCFixture::CallbackFunction ConnectIndication = [&](const BaseCallbackParameter& _) -> bool
        {
            bIndication = true;
            return true;
        };
        //
        // Attempt to connect to our dummy server...
        //
        RegisterConnectConfirm(ConnectConfirm);
        m_MyServer.RegisterConnectIndication(ConnectIndication);
        EXPECT_EQ(RUN_WAIT, ConnectRequest(DLConnectRequestOrIndication(HDLCAddress(0x01))));
        //
        // Run the server...
        //
        EXPECT_EQ(RUN_WAIT, m_MyServer.Process());
        EXPECT_EQ(true, bIndication);
        m_MyServer.ConnectResponse(DLConnectConfirmOrResponse(HDLCAddress(0x02)));
        //
        // Run the client...
        //
        EXPECT_EQ(RUN_WAIT, Process());
        ASSERT_EQ(true, bConfirmation);
        //
        // We are already connected, so let's send a simple message...
        //
        const uint8_t SAMPLE_DATA[] = "COME HERE WATSON, I NEED YOU!";
        std::vector<uint8_t> DATA(SAMPLE_DATA,
            SAMPLE_DATA + sizeof(SAMPLE_DATA));
        EXPECT_EQ(RUN_WAIT, DataRequest(DLDataRequestParameter(HDLCAddress(0x01), HDLCControl::INFO, DATA)));
        //
        // Run the server...
        //
        EXPECT_EQ(RUN_WAIT, m_MyServer.Process());

    }

}