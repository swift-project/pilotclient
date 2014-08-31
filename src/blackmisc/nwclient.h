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
#include "indexvariantmap.h"

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Another client software.
         */
        class CClient : public BlackMisc::CValueObject
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
            CClient() {}

            //! Construct by callsign
            CClient(const BlackMisc::Aviation::CCallsign &callsign) : m_user(CUser(callsign)) {}

            //! Constructor.
            CClient(const CUser &user) : m_user(user) {}

            //! Equal operator ==
            bool operator ==(const CClient &other) const;

            //! Unequal operator !=
            bool operator !=(const CClient &other) const;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Callsign used with other client
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return this->m_user.getCallsign(); }

            //! Get capabilities
            CIndexVariantMap getCapabilities() const { return this->m_capabilities; }

            //! Set capability
            void setCapability(bool hasCapability, Capabilities capability);

            //! Set capabilities
            void setCapabilities(const CIndexVariantMap &capabilities);

            //! Get capabilities
            QString getCapabilitiesAsString() const;

            //! Has capability?
            bool hasCapability(Capabilities capability) const;

            //! Get voice capabilities
            const CVoiceCapabilities &getVoiceCapabilities() const { return m_voiceCapabilities;}

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

            //! \copydoc CValueObject::propertyByIndex(int)
            virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex(const QVariant, int)
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Register metadata
            static void registerMetadata();

            //! JSON member names
            static const QStringList &jsonMembers();

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CClient)
            CUser m_user;
            CAircraftModel m_model;
            CIndexVariantMap m_capabilities;
            QString m_server;
            CVoiceCapabilities m_voiceCapabilities;

        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CClient, (o.m_user, o.m_model, o.m_capabilities, o.m_server, o.m_voiceCapabilities))
Q_DECLARE_METATYPE(BlackMisc::Network::CClient)

#endif // guard
