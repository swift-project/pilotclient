// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_MESSAGEBASE_H
#define SWIFT_CORE_FSD_MESSAGEBASE_H

#include <QDebug>
#include <QString>
#include <QStringBuilder>
#include <QStringList>

#include "core/swiftcoreexport.h"

//! Message type
//! \remark FSD Server docu https://fsd-doc.norrisng.ca/
enum class MessageType
{
    Unknown,
    AddAtc,
    AddPilot,
    AtcDataUpdate,
    AuthChallenge,
    AuthResponse,
    ClientIdentification,
    ClientQuery,
    ClientResponse,
    DeleteATC,
    DeletePilot,
    EuroscopeSimData, // Euroscope only
    FlightPlan,
    ProController,
    FsdIdentification,
    KillRequest,
    PilotDataUpdate,
    VisualPilotDataUpdate,
    VisualPilotDataPeriodic,
    VisualPilotDataStopped,
    VisualPilotDataToggle,
    Ping,
    Pong,
    ServerError,
    ServerHeartbeat,
    RegistrationInfo, // IVAO only
    TextMessage,
    PilotClientCom,
    RevBClientParts, // IVAO only
    RevBPilotDescription, // -PD IVAO only not handled in swift
    Rehost,
    Mute,
};

namespace swift::core::fsd
{
    //! FSD message base class
    class SWIFT_CORE_EXPORT MessageBase
    {
    public:
        //! Default Constructor
        MessageBase() = default;

        //! Constructor
        MessageBase(const QString &sender);

        //! Constructor
        MessageBase(const QString &sender, const QString &receiver);

        //! Set callsign
        void setCallsign(const QString &sender) { m_sender = sender; }

        //! get message sender
        QString sender() const { return m_sender; }

        //! Set message receiver
        void setReceiver(const QString &receiver) { m_receiver = receiver; }

        //! Get message receiver
        QString receiver() const { return m_receiver; }

        //! Is message valid?
        bool isValid() const { return m_isValid; }

        //! set message valid
        void setValid(bool isValid) { m_isValid = isValid; }

    protected:
        QString m_sender; //!< message sender
        QString m_receiver; //!< message receiver

        bool m_isValid = true; //!< is valid?
    };

    //! String which will be send
    template <class T>
    QString messageToFSDString(const T &message)
    {
        if (!message.isValid()) return {};
        return message.pdu() % message.toTokens().join(':') % QStringLiteral("\r\n");
    }
} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_MESSAGEBASE_H
