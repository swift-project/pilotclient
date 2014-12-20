/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CLIENT_H
#define BLACKMISC_CLIENT_H

#include "nwuser.h"
#include "nwaircraftmodel.h"
#include "nwvoicecapabilities.h"
#include "propertyindex.h"
#include "propertyindexvariantmap.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Another client software.
         */
        class CClient : public CValueObjectStdTuple<CClient>
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
                FsdAtisCanBeReceived
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
            bool isAtc() const { return getCallsign().isAtcCallsign(); }

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

            //! Server
            const QString &getServer() const { return this->m_server; }

            //! Server
            void setServer(const QString &server) { this->m_server = server;}

            //! Model
            const CAircraftModel &getAircraftModel() const { return this->m_model; }

            //! Set model
            void setAircraftModel(const CAircraftModel &model) { this->m_model = model; }

            //! \copydoc CValueObject::toIcon()
            virtual CIcon toIcon() const override { return this->m_user.toIcon(); }

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CClient)
            CUser          m_user;
            CAircraftModel m_model;
            CPropertyIndexVariantMap m_capabilities;
            QString                  m_server;
            CVoiceCapabilities       m_voiceCapabilities;

        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CClient, (
                                   o.m_user,
                                   o.m_model,
                                   attr(o.m_capabilities, flags<DisabledForComparison>()),
                                   o.m_server,
                                   o.m_voiceCapabilities
                               ))
Q_DECLARE_METATYPE(BlackMisc::Network::CClient)

#endif // guard
