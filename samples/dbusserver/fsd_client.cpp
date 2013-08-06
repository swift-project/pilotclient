/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QDebug>

#include "fsd_client.h"

CFsdClient::CFsdClient(QObject *parent) :
    QObject(parent)
{
}

void CFsdClient::connectTo(const QString &host)
{
    qDebug() << "Client requests to connect to " << host;
}
