/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_CLIENT_H
#define BLACKMISC_NETWORK_CLIENT_H

#include "user.h"
#include "voicecapabilities.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"
#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Network
    {
        //! Another client software.
        class BLACKMISC_EXPORT CClient : public CValueObject<CClient>
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
                FsdWithIcaoCodes        = 1 << 1, //!< basically means it is a pilot client handling ICAO code packages
                FsdAtisCanBeReceived    = 1 << 2, //!< ATIS
                FsdWithAircraftConfig   = 1 << 3, //!< Aircraft parts
                FsdWithGroundFlag       = 1 << 4, //!< supports gnd. flag (in position)
                FsdModelString          = 1 << 5, //!< model string can be queried
                FsdWithVisualPositions  = 1 << 6  //!< visual position updates
            };
            Q_DECLARE_FLAGS(Capabilities, Capability)

            //! Default constructor.
            CClient() = default;

            //! Construct by callsign and optional model string
            CClient(const Aviation::CCallsign &callsign, const QString &modelString = {});

            //! Constructor.
            CClient(const CUser &user) : m_user(user) {}

            //! Callsign used with other client
            const Aviation::CCallsign &getCallsign() const { return m_user.getCallsign(); }

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
            const CVoiceCapabilities &getVoiceCapabilities() const { return m_voiceCapabilities;}

            //! Has known voice capabilities?
            bool hasSpecifiedVoiceCapabilities() const { return !m_voiceCapabilities.isUnknown();}

            //! Set voice capabilities
            void setVoiceCapabilities(const CVoiceCapabilities &voiceCapabilities) { m_voiceCapabilities = voiceCapabilities;}

            //! Set voice capabilities
            void setVoiceCapabilities(const QString &flightPlanRemarks) { m_voiceCapabilities = CVoiceCapabilities(flightPlanRemarks);}

            //! User
            const CUser &getUser() const { return m_user; }

            //! User
            void setUser(const CUser &user) { m_user = user;}

            //! User's callsign
            bool setUserCallsign(const BlackMisc::Aviation::CCallsign &callsign);

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

            //! \copydoc Simulation::CAircraftModel::hasQueriedModelString
            bool hasQueriedModelString() const { return !m_modelString.isEmpty(); }

            //! Set model
            void setQueriedModelString(const QString &modelString);

            //! \copydoc BlackMisc::Mixin::Icon::toIcon()
            CIcons::IconIndex toIcon() const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
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
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CClient)
Q_DECLARE_METATYPE(BlackMisc::Network::CClient::Capability)
Q_DECLARE_METATYPE(BlackMisc::Network::CClient::Capabilities)

#endif // guard
