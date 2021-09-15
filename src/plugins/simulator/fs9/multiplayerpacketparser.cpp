/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */
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
