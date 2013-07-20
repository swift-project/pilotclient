#include "blackmisc/debug.h"
#include "server.h"

using namespace BlackMisc;

Server::Server(QObject *parent) : QObject(parent), server(IContext::getInstance())
{
    QHostAddress local = QHostAddress(QHostAddress::LocalHost);

    server.Host(local, 6809);

    connect(&server, static_cast<void (BlackMisc::CComServer::*)(QString &, QByteArray &)>(&BlackMisc::CComServer::doMessageReceived),
            this,    &Server::onData);

    CMessageSystem myMessageSystem;

	bAppInfo << "Com Server running. \n";
}

Server::~Server()
{
}

void Server::onData(QString &messageID, QByteArray &message)
{
    BlackMisc::IMessage* test = BlackMisc::CMessageFactory::getInstance().create(messageID);
    QDataStream stream(&message, QIODevice::ReadOnly);

    Q_ASSERT(test);
    *test << stream;

    CMessageDispatcher::getInstance().append(test);
    CMessageDispatcher::getInstance().dispatch();

}

void TestMessageHandler::onTestMessage(const TestMessage *testmessage)
{
   bAppDebug << "Message ID: " << testmessage->getID() << " with text: " << testmessage->getTestString();
}
