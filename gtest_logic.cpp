#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "chatlogic.h"
#include "MessageProcessor.hpp"

// =======================
// CLIENT BUSINESS LOGIC TESTS
// =======================

TEST(ChatLogicTest, AcceptsValidUsername) {
    ChatLogic logic;
    EXPECT_TRUE(logic.validateUsername("Ibrahim"));
}

TEST(ChatLogicTest, RejectsEmptyUsername) {
    ChatLogic logic;
    EXPECT_FALSE(logic.validateUsername(""));
}

TEST(ChatLogicTest, RejectsSpacesOnlyUsername) {
    ChatLogic logic;
    EXPECT_FALSE(logic.validateUsername("     "));
}

TEST(ChatLogicTest, AcceptsValidMessage) {
    ChatLogic logic;
    EXPECT_TRUE(logic.validateMessage("Hello"));
}

TEST(ChatLogicTest, FormatsMessageCorrectly) {
    ChatLogic logic;
    EXPECT_EQ(logic.formatMessage("Ibrahim", "Hello"), "Ibrahim: Hello");
}


// =======================
// SERVER BUSINESS LOGIC TESTS
// =======================

TEST(MessageProcessorTest, FirstLoginSucceeds) {
    MessageProcessor mp;

    string result = mp.processLogin("Hana");

    EXPECT_NE(result.find("Connected successfully"), string::npos);
}

TEST(MessageProcessorTest, DuplicateLoginFails) {
    MessageProcessor mp;

    mp.processLogin("Hana");
    string result = mp.processLogin("Hana");

    EXPECT_NE(result.find("Username already exists"), string::npos);
}

TEST(MessageProcessorTest, LogoutRemovesUsername) {
    MessageProcessor mp;

    mp.processLogin("Hana");
    mp.handleLogout("Hana");

    string result = mp.processLogin("Hana");

    EXPECT_NE(result.find("Connected successfully"), string::npos);
}

TEST(MessageProcessorTest, DetectsPrivateMessage) {
    MessageProcessor mp;

    string msg = R"({"type":"private_message","sender":"Hana","receiver":"Ibrahim","message":"hi"})";

    EXPECT_TRUE(mp.isPrivate(msg));
}

TEST(MessageProcessorTest, RejectsInvalidJsonAsPrivateMessage) {
    MessageProcessor mp;

    string msg = "this is not json";

    EXPECT_FALSE(mp.isPrivate(msg));
}

TEST(ChatLogicTest, RejectsEmptyMessage) {
    ChatLogic logic;
    EXPECT_FALSE(logic.validateMessage(""));
}
// =======================
// GOOGLE MOCK NETWORK TESTS
// =======================

class INetworkInterfaceForTest {
public:
    virtual ~INetworkInterfaceForTest() = default;
    virtual void sendMessage(const QString& msg) = 0;
    virtual QString receiveServerResponse() = 0;
};

class MockNetworkInterface : public INetworkInterfaceForTest {
public:
    MOCK_METHOD(void, sendMessage, (const QString& msg), (override));
    MOCK_METHOD(QString, receiveServerResponse, (), (override));
};

TEST(ClientMockTest, SendsValidMessageThroughMockNetwork) {
    MockNetworkInterface mockNetwork;

    EXPECT_CALL(mockNetwork, sendMessage(QString("Hello"))).Times(1);

    mockNetwork.sendMessage("Hello");
}

TEST(ClientMockTest, SimulatesSuccessfulServerResponse) {
    MockNetworkInterface mockNetwork;

    EXPECT_CALL(mockNetwork, receiveServerResponse())
        .WillOnce(testing::Return(QString("{\"type\":\"status\",\"message\":\"Connected successfully\"}")));

    QString response = mockNetwork.receiveServerResponse();

    EXPECT_TRUE(response.contains("Connected successfully"));
}

TEST(ClientMockTest, SimulatesServerErrorResponse) {
    MockNetworkInterface mockNetwork;

    EXPECT_CALL(mockNetwork, receiveServerResponse())
        .WillOnce(testing::Return(QString("{\"type\":\"error\",\"message\":\"Username already exists\"}")));

    QString response = mockNetwork.receiveServerResponse();

    EXPECT_TRUE(response.contains("Username already exists"));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}