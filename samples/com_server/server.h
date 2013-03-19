#include "blackmisc/message_system.h"
#include "blackmisc/com_server.h"

class TestMessageHandler : public BlackMisc::CMessageHandler
{
public:
    TestMessageHandler()
    {
        registerMessageFunction(this, &TestMessageHandler::onTestMessage);
    }
private:

    void onTestMessage(const BlackMisc::TestMessage *testmessage);

};

class Server : public QObject
{
    Q_OBJECT
    
public:
    explicit Server(QObject *parent = NULL);
    ~Server();

protected slots:
    
    void onData(QString &messageID, QByteArray& message);

private:
    BlackMisc::CComServer server;
    TestMessageHandler myHandler;
};
