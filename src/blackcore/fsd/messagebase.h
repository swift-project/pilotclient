// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_FSD_MESSAGEBASE_H
#define BLACKCORE_FSD_MESSAGEBASE_H

#include "blackcore/blackcoreexport.h"

#include <QString>
#include <QStringBuilder>
#include <QStringList>
#include <QDebug>

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

namespace BlackCore::Fsd
{
    //! FSD message base class
    class BLACKCORE_EXPORT MessageBase
    {
    public:
        //! Default Constructor
        MessageBase() {}

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
} // ns

#endif // guard
