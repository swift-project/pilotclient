// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/fsd/atcdataupdate.h"
#include "blackcore/fsd/serializer.h"

#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore::Fsd
{
    AtcDataUpdate::AtcDataUpdate() : MessageBase()
    {}

    AtcDataUpdate::AtcDataUpdate(const QString &sender, int frequencykHz, CFacilityType facility, int visibleRange, AtcRating rating,
                                 double latitude, double longitude, int elevation) : MessageBase(sender, {}),
                                                                                     m_frequencykHz(frequencykHz),
                                                                                     m_facility(facility),
                                                                                     m_visibleRange(visibleRange),
                                                                                     m_rating(rating),
                                                                                     m_latitude(latitude),
                                                                                     m_longitude(longitude),
                                                                                     m_elevation(elevation)
    {}

    QStringList AtcDataUpdate::toTokens() const
    {
        QStringList tokens;
        tokens.push_back(m_sender);
        tokens.push_back(QString::number(m_frequencykHz - 100000));
        tokens.push_back(toQString(m_facility));
        tokens.push_back(QString::number(m_visibleRange));
        tokens.push_back(toQString(m_rating));
        tokens.push_back(QString::number(m_latitude, 'f', 5));
        tokens.push_back(QString::number(m_longitude, 'f', 5));
        tokens.push_back(QString::number(m_elevation));
        return tokens;
    }

    AtcDataUpdate AtcDataUpdate::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 7)
        {
            CLogMessage(static_cast<AtcDataUpdate *>(nullptr)).warning(u"Wrong number of arguments.");
            return {};
        }

        AtcDataUpdate packet(tokens[0], tokens[1].toInt() + 100000, fromQString<CFacilityType>(tokens[2]), tokens[3].toInt(), fromQString<AtcRating>(tokens[4]),
                             tokens[5].toDouble(), tokens[6].toDouble(), tokens[7].toInt());
        return packet;
    }
}
