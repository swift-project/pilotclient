// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_CLIENT_H
#define SWIFT_MISC_NETWORK_CLIENT_H

#include "misc/network/user.h"
#include "misc/network/voicecapabilities.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/aviation/callsign.h"
#include "misc/swiftmiscexport.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/valueobject.h"
#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CClient)

namespace swift::misc::network
{
    //! Another client software.
    class SWIFT_MISC_EXPORT CClient : public CValueObject<CClient>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexCapabilities = CPropertyIndexRef::GlobalIndexCClient,
            IndexCapabilitiesString,
            IndexModelString,
            IndexServer,
            IndexUser,
            IndexCallsign,
            IndexVoiceCapabilities,
            IndexVoiceCapabilitiesString,
            IndexVoiceCapabilitiesPixmap,
            IndexVoiceCapabilitiesIcon
        };

        //! The Capabilities enum
        enum Capability
        {
            None = 0,
            FsdWithInterimPositions = 1 << 0, //!< fast position updates
            FsdWithIcaoCodes = 1 << 1, //!< basically means it is a pilot client handling ICAO code packages
            FsdAtisCanBeReceived = 1 << 2, //!< ATIS
            FsdWithAircraftConfig = 1 << 3, //!< Aircraft parts
            FsdWithGroundFlag = 1 << 4, //!< supports gnd. flag (in position)
            FsdModelString = 1 << 5, //!< model string can be queried
            FsdWithVisualPositions = 1 << 6 //!< visual position updates
        };
        Q_DECLARE_FLAGS(Capabilities, Capability)

        //! Default constructor.
        CClient() = default;

        //! Construct by callsign and optional model string
        CClient(const aviation::CCallsign &callsign, const QString &modelString = {});

        //! Constructor.
        CClient(const CUser &user) : m_user(user) {}

        //! Callsign used with other client
        const aviation::CCallsign &getCallsign() const { return m_user.getCallsign(); }

        //! ATC client
        bool isAtc() const { return getCallsign().isAtcAlikeCallsign(); }

        //! Is valid
        bool isValid() const;

        //! Get capabilities
        Capabilities getCapabilities() const { return static_cast<Capabilities>(m_capabilities); }

        //! Set capabilities
        void setCapabilities(const Capabilities &capabilities);

        //! Get capabilities
        QString getCapabilitiesAsString() const;

        //! Has capability?
        bool hasCapability(Capability capability) const;

        //! Supports aircraft parts?
        bool hasAircraftPartsCapability() const;

        //! Supports gnd.flag?
        bool hasGndFlagCapability() const;

        //! Add capability
        void addCapability(Capability capability);

        //! Remove capability
        void removeCapability(Capability capability);

        //! Get voice capabilities
        const CVoiceCapabilities &getVoiceCapabilities() const { return m_voiceCapabilities; }

        //! Has known voice capabilities?
        bool hasSpecifiedVoiceCapabilities() const { return !m_voiceCapabilities.isUnknown(); }

        //! Set voice capabilities
        void setVoiceCapabilities(const CVoiceCapabilities &voiceCapabilities) { m_voiceCapabilities = voiceCapabilities; }

        //! Set voice capabilities
        void setVoiceCapabilities(const QString &flightPlanRemarks) { m_voiceCapabilities = CVoiceCapabilities(flightPlanRemarks); }

        //! User
        const CUser &getUser() const { return m_user; }

        //! User
        void setUser(const CUser &user) { m_user = user; }

        //! User's callsign
        bool setUserCallsign(const swift::misc::aviation::CCallsign &callsign);

        //! Server
        const QString &getServer() const { return m_server; }

        //! Server
        void setServer(const QString &server) { m_server = server; }

        //! Another swift client?
        bool isSwiftClient() const { return m_swift; }

        //! Mark as other swift client
        void setSwiftClient(bool isSwift) { m_swift = isSwift; }

        //! Model
        const QString &getQueriedModelString() const { return m_modelString; }

        //! \copydoc simulation::CAircraftModel::hasQueriedModelString
        bool hasQueriedModelString() const { return !m_modelString.isEmpty(); }

        //! Set model
        void setQueriedModelString(const QString &modelString);

        //! \copydoc swift::misc::mixin::Icon::toIcon()
        CIcons::IconIndex toIcon() const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        CUser m_user;
        int m_capabilities = static_cast<int>(None);
        bool m_swift = false; // another swift client
        QString m_modelString;
        QString m_server;
        CVoiceCapabilities m_voiceCapabilities;

        BLACK_METACLASS(
            CClient,
            BLACK_METAMEMBER(user),
            BLACK_METAMEMBER(swift),
            BLACK_METAMEMBER(modelString),
            BLACK_METAMEMBER(capabilities),
            BLACK_METAMEMBER(server),
            BLACK_METAMEMBER(voiceCapabilities)
        );
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::network::CClient)
Q_DECLARE_METATYPE(swift::misc::network::CClient::Capability)
Q_DECLARE_METATYPE(swift::misc::network::CClient::Capabilities)

#endif // guard
