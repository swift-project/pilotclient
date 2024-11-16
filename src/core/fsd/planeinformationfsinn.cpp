// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/fsd/planeinformationfsinn.h"

#include "misc/logmessage.h"

namespace swift::core::fsd
{
    PlaneInformationFsinn::PlaneInformationFsinn() : MessageBase()
    {}

    PlaneInformationFsinn::PlaneInformationFsinn(const QString &sender,
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
    {}

    QStringList PlaneInformationFsinn::toTokens() const
    {
        auto tokens = QStringList {};
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back("FSIPI");
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

    PlaneInformationFsinn PlaneInformationFsinn::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() != 12)
        {
            swift::misc::CLogMessage(static_cast<PlaneInformationFsinn *>(nullptr)).debug(u"Wrong number of arguments.");
            return {};
        };

        return PlaneInformationFsinn(tokens[0], tokens[1], tokens[4], tokens[5], tokens[10], tokens[11]);
    }
} // namespace swift::core::fsd
