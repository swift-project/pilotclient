// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/dbus.h"

QDBusArgument &operator<<(QDBusArgument &arg, const std::string &s)
{
    arg.beginStructure();
    arg << QString::fromStdString(s);
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, std::string &s)
{
    QString qs;
    arg.beginStructure();
    arg >> qs;
    arg.endStructure();
    s = qs.toStdString();
    return arg;
}
