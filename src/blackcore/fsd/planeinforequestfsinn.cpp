/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/fsd/planeinforequestfsinn.h"

#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    PlaneInfoRequestFsinn::PlaneInfoRequestFsinn() : MessageBase()
    { }

    PlaneInfoRequestFsinn::PlaneInfoRequestFsinn(const QString &sender,
                                                    const QString &receiver,
                                                    const QString &airlineIcao,
                                                    const QString &aircraftIcao,
                                                    const QString &aircraftIcaoCombinedType,
                                                    const QString &sendMModelString)
        : MessageBase(sender, receiver),
            m_airlineIcao(airlineIcao),
            m_aircraftIcao(aircraftIcao),
            m_aircraftIcaoCombinedType(aircraftIcaoCombinedType),
            m_sendMModelString(sendMModelString)
    { }

    QStringList PlaneInfoRequestFsinn::toTokens() const
    {
        auto tokens = QStringList {};
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back("FSIPIR");
        tokens.push_back("0");
        tokens.push_back(m_airlineIcao);
        tokens.push_back(m_aircraftIcao);
        tokens.push_back({});
        tokens.push_back({});
        tokens.push_back({});
        tokens.push_back({});
        tokens.push_back(m_aircraftIcaoCombinedType);
        tokens.push_back(m_sendMModelString);
        return tokens;
    }

    PlaneInfoRequestFsinn PlaneInfoRequestFsinn::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() != 12)
        {
            BlackMisc::CLogMessage(static_cast<PlaneInfoRequestFsinn *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };
        return PlaneInfoRequestFsinn(tokens[0], tokens[1], tokens[4], tokens[5], tokens[10], tokens[11]);
    }
}
