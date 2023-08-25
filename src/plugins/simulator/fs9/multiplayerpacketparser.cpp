// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
#include "multiplayerpacketparser.h"

namespace BlackSimPlugin::Fs9::Private
{
    QByteArray readValue(const QByteArray &data, QString &val)
    {
        val = QString(data);

        // Up to know all packets with string data, had the string payload at the end
        // of the packet. Therefore there should not be any data after the string.
        // Remove the assert, if this will change in the future.
        QByteArray leftOver = data.mid(val.size() + 1);
        Q_ASSERT(leftOver.isEmpty());

        return leftOver;
    }

    QByteArray writeValue(QByteArray data, const QString &value)
    {
        data.append(qPrintable(value), value.size() + 1);
        return data;
    }
} // ns
