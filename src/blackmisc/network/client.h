/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_CLIENT_H
#define BLACKMISC_NETWORK_CLIENT_H

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/icon.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

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
                IndexCapabilities = CPropertyIndex::GlobalIndexCClient,
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
            enum Capabilities
            {
                FsdWithInterimPositions = CPropertyIndex::GlobalIndexClientCapabilities, //!< interim positions
                FsdWithIcaoCodes,       //!< basically means it is a pilot client handling ICAO code pacakages
                FsdAtisCanBeReceived,   //!< ATIS
                FsdWithAircraftConfig   //!< Aircraft parts
            };

            //! Default constructor.
            CClient() = default;

            //! Construct by callsign and optional model string
            CClient(const BlackMisc::Aviation::CCallsign &callsign, const QString &modelString = {});

            //! Constructor.
            CClient(const CUser &user) : m_user(user) {}

            //! Callsign used with other client
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return m_user.getCallsign(); }

            //! ATC client
            bool isAtc() const { return getCallsign().isAtcAlikeCallsign(); }

            //! Is valid
            bool isValid() const;

            //! Get capabilities
            CPropertyIndexVariantMap getCapabilities() const { return m_capabilities; }

            //! Set capability
            void setCapability(bool hasCapability, Capabilities capability);

            //! Set capabilities
            void setCapabilities(const CPropertyIndexVariantMap &capabilities);

            //! Get capabilities
            QString getCapabilitiesAsString() const;

            //! Has capability?
            bool hasCapability(Capabilities capability) const;

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
            void setUserCallsign(const BlackMisc::Aviation::CCallsign &callsign);

            //! Server
            const QString &getServer() const { return m_server; }

            //! Server
            void setServer(const QString &server) { m_server = server;}

            //! Model
            const QString &getQueriedModelString() const { return m_modelString; }

            //! \copydoc Simulation::CAircraftModel::hasQueriedModelString
            bool hasQueriedModelString() const { return !m_modelString.isEmpty(); }

            //! Set model
            void setQueriedModelString(const QString &modelString) { m_modelString = modelString.trimmed(); }

            //! \copydoc BlackMisc::Mixin::Icon::toIcon()
            CIcon toIcon() const { return m_user.toIcon(); }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            CUser m_user;
            CPropertyIndexVariantMap m_capabilities;
            QString m_modelString;
            QString m_server;
            CVoiceCapabilities m_voiceCapabilities;

            BLACK_METACLASS(
                CClient,
                BLACK_METAMEMBER(user),
                BLACK_METAMEMBER(modelString),
                BLACK_METAMEMBER(capabilities, 0, DisabledForComparison | DisabledForJson),
                BLACK_METAMEMBER(server),
                BLACK_METAMEMBER(voiceCapabilities)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CClient)

#endif // guard
