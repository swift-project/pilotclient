// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/audio/notificationsounds.h"

#include <QString>
#include <QStringList>

namespace swift::misc::audio
{

    const QString &CNotificationSounds::flagToString(CNotificationSounds::NotificationFlag notification)
    {
        static const QString unknown("unknown");
        static const QString error("error");
        static const QString login("login");
        static const QString logoff("logoff");
        static const QString freqMsg("frequency msg.");
        static const QString privateMsg("private msg.");
        static const QString supMsg("supervisor msg.");
        static const QString unicomMsg("unicom msg.");
        static const QString mentioned("cs mentioned");
        static const QString pttUp("PTT click up");
        static const QString pttDown("PTT click down");
        static const QString afvclick("AFV click");
        static const QString afvblock("AFV blocked");

        switch (notification)
        {
        case NotificationError: return error;
        case NotificationLogin: return login;
        case NotificationLogoff: return logoff;
        case NotificationTextMessageFrequency: return freqMsg;
        case NotificationTextMessagePrivate: return privateMsg;
        case NotificationTextMessageSupervisor: return supMsg;
        case NotificationTextMessageUnicom: return unicomMsg;
        case NotificationTextCallsignMentioned: return mentioned;
        case PTTClickKeyDown: return pttDown;
        case PTTClickKeyUp: return pttUp;
        case AFVClicked: return afvclick;
        case AFVBlocked: return afvblock;

        default: break;
        }
        return unknown;
    }

    QString CNotificationSounds::toString(Notification notification)
    {
        QStringList n;
        if (notification.testFlag(NotificationError)) n << flagToString(NotificationError);
        if (notification.testFlag(NotificationLogin)) n << flagToString(NotificationLogin);
        if (notification.testFlag(NotificationLogoff)) n << flagToString(NotificationLogoff);
        if (notification.testFlag(NotificationTextMessageFrequency))
            n << flagToString(NotificationTextMessageFrequency);
        if (notification.testFlag(NotificationTextMessagePrivate)) n << flagToString(NotificationTextMessagePrivate);
        if (notification.testFlag(NotificationTextMessageSupervisor))
            n << flagToString(NotificationTextMessageSupervisor);
        if (notification.testFlag(NotificationTextMessageUnicom)) n << flagToString(NotificationTextMessageUnicom);
        if (notification.testFlag(NotificationTextCallsignMentioned))
            n << flagToString(NotificationTextCallsignMentioned);

        if (notification.testFlag(PTTClickKeyUp)) n << flagToString(PTTClickKeyUp);
        if (notification.testFlag(PTTClickKeyDown)) n << flagToString(PTTClickKeyDown);

        if (notification.testFlag(AFVClicked)) n << flagToString(AFVClicked);
        if (notification.testFlag(AFVBlocked)) n << flagToString(AFVBlocked);
        return n.join(", ");
    }
} // namespace swift::misc::audio
