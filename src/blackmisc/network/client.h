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

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/network/user.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/propertyindexvariantmap.h"

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
                IndexCapabilities = BlackMisc::CPropertyIndex::GlobalIndexCClient,
                IndexCapabilitiesString,
                IndexModel,
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
                FsdWithInterimPositions = BlackMisc::CPropertyIndex::GlobalIndexAbuseMode,
                FsdWithModelDescription,
                FsdAtisCanBeReceived,
                FsdWithAircraftConfig
            };

            //! Default constructor.
            CClient() = default;

            //! Construct by callsign
            CClient(const BlackMisc::Aviation::CCallsign &callsign) : m_user(CUser(callsign)) {}

            //! Constructor.
            CClient(const CUser &user) : m_user(user) {}

            //! Callsign used with other client
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return this->m_user.getCallsign(); }

            //! ATC client
            bool isAtc() const { return getCallsign().isAtcAlikeCallsign(); }

            //! Get capabilities
            CPropertyIndexVariantMap getCapabilities() const { return this->m_capabilities; }

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
            const CUser &getUser() const { return this->m_user; }

            //! User
            void setUser(const CUser &user) { this->m_user = user;}

            //! User's callsign
            void setUserCallsign(const BlackMisc::Aviation::CCallsign &callsign);

            //! Server
            const QString &getServer() const { return this->m_server; }

            //! Server
            void setServer(const QString &server) { this->m_server = server;}

            //! Model
            const BlackMisc::Simulation::CAircraftModel &getAircraftModel() const { return this->m_model; }

            //! \copydoc Simulation::CAircraftModel::hasQueriedModelString
            bool hasQueriedModelString() const { return this->m_model.hasQueriedModelString(); }

            //! Set model
            void setAircraftModel(const BlackMisc::Simulation::CAircraftModel &model) { this->m_model = model; }

            //! \copydoc BlackMisc::Mixin::Icon::toIcon()
            CIcon toIcon() const { return this->m_user.toIcon(); }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            BlackMisc::Network::CUser              m_user;
            BlackMisc::Simulation::CAircraftModel  m_model;
            BlackMisc::CPropertyIndexVariantMap    m_capabilities;
            QString                                m_server;
            BlackMisc::Network::CVoiceCapabilities m_voiceCapabilities;

            BLACK_METACLASS(CClient,
                BLACK_METAMEMBER(user),
                BLACK_METAMEMBER(model),
                BLACK_METAMEMBER(capabilities, 0, DisabledForComparison | DisabledForJson),
                BLACK_METAMEMBER(server),
                BLACK_METAMEMBER(voiceCapabilities)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CClient)

#endif // guard
