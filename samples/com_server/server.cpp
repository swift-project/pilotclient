#include "blackmisc/debug.h"
#include "server.h"

using namespace BlackMisc;

Server::Server(QObject *parent) : QObject(parent)
{
    QHostAddress local = QHostAddress(QHostAddress::LocalHost);

    server.Host(local, 6809);

    connect(&server, SIGNAL(doMessageReceived(QString &, QByteArray&)), this, SLOT(onData(QString &, QByteArray&)));

    CMessageSystem myMessageSystem;

	bInfo << "Com Server running. \n";
}

Server::~Server()
{
}

void Server::onData(QString &messageID, QByteArray &message)
{
    BlackMisc::IMessage* test = BlackMisc::CMessageFactory::getInstance().create(messageID);
    QDataStream stream(&message, QIODevice::ReadOnly);

    bAssert (test);
    *test << stream;

    CMessageDispatcher::getInstance().append(test);
    CMessageDispatcher::getInstance().dispatch();

}

void TestMessageHandler::onTestMessage(const TestMessage *testmessage)
{
   bDebug << "Message ID: " << testmessage->getID() << " with text: " << testmessage->getTestString();
}
