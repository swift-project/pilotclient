#include <QDebug>

#include "fsdclient.h"

CFsdClient::CFsdClient(QObject *parent) :
    QObject(parent)
{
}

void CFsdClient::connectTo(const QString &host)
{
    qDebug() << "Client requests to connect to " << host;
}
