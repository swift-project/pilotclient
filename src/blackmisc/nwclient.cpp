/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "nwclient.h"
#include "icon.h"
#include "avcallsign.h"

#include <QString>

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Convert to string
         */
        QString CClient::convertToQString(bool i18n) const
        {
            QString s = this->m_user.toQString(i18n);
            s.append(" capabilites: ").append(this->getCapabilitiesAsString());
            s.append(" model: ").append(this->m_model.toQString(i18n));
            if (!this->m_server.isEmpty())
            {
                s.append(" server:").append(this->m_server);
            }
            return s;
        }

        /*
         * Compare
         */
        int CClient::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CClient &>(otherBase);
            return compare(TupleConverter<CClient>::toTuple(*this), TupleConverter<CClient>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CClient::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CClient>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CClient::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CClient>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CClient::getValueHash() const
        {
            return qHash(TupleConverter<CClient>::toTuple(*this));
        }

        /*
         * Equal?
         */
        bool CClient::operator ==(const CClient &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CClient>::toTuple(*this) == TupleConverter<CClient>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CClient::operator !=(const CClient &other) const
        {
            return !((*this) == other);
        }

        /*
         * metaTypeId
         */
        int CClient::getMetaTypeId() const
        {
            return qMetaTypeId<CClient>();
        }

        /*
         * is a
         */
        bool CClient::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CClient>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Register metadata
         */
        void CClient::registerMetadata()
        {
            qRegisterMetaType<CClient>();
            qDBusRegisterMetaType<CClient>();
        }

        /*
         * Members
         */
        const QStringList &CClient::jsonMembers()
        {
            return TupleConverter<CClient>::jsonMembers();
        }

        /*
         * To JSON
         */
        QJsonObject CClient::toJson() const
        {
            return BlackMisc::serializeJson(TupleConverter<CClient>::toMetaTuple(*this));
        }

        /*
         * From Json
         */
        void CClient::convertFromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, TupleConverter<CClient>::toMetaTuple(*this));
        }

        /*
         * Capability
         */
        void CClient::setCapability(bool hasCapability, CClient::Capabilities capability)
        {
            this->m_capabilities.addValue(static_cast<int>(capability), hasCapability);
        }

        /*
         * Capabilities
         */
        void CClient::setCapabilities(const CPropertyIndexVariantMap &capabilities)
        {
            this->m_capabilities = capabilities;
        }

        /*
         * Capabilities
         */
        QString CClient::getCapabilitiesAsString() const
        {
            QStringList sl;
            if (this->hasCapability(FsdAtisCanBeReceived)) sl << "ATIS";
            if (this->hasCapability(FsdWithInterimPositions)) sl << "interim pos.";
            if (this->hasCapability(FsdWithModelDescription)) sl << "model";
            if (sl.isEmpty()) return "";
            return sl.join(", ");
        }

        /*
         * Capability
         */
        bool CClient::hasCapability(CClient::Capabilities capability) const
        {
            if (this->m_capabilities.contains(capability))
                return this->m_capabilities.value(capability).toBool();
            else
                return false;
        }

        /*
         * Property by index
         */
        QVariant CClient::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCapabilities:
                return this->m_capabilities.toQVariant();
            case IndexCapabilitiesString:
                return QVariant(this->getCapabilitiesAsString());
            case IndexCallsign:
                return this->getCallsign().propertyByIndex(index.copyFrontRemoved());
            case IndexUser:
                return this->getUser().propertyByIndex(index.copyFrontRemoved());
            case IndexModel:
                return this->m_model.propertyByIndex(index.copyFrontRemoved());
            case IndexServer:
                return QVariant(this->m_server);
            case IndexVoiceCapabilities:
                return this->m_voiceCapabilities.propertyByIndex(index.copyFrontRemoved());
            case IndexVoiceCapabilitiesPixmap:
                return QVariant(this->m_voiceCapabilities.toPixmap());
            case IndexVoiceCapabilitiesIcon:
                return QVariant(this->m_voiceCapabilities.toIcon().toQVariant());
            case IndexVoiceCapabilitiesString:
                return QVariant(this->m_voiceCapabilities.toQString(true));
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        /*
         * Set property as index
         */
        void CClient::setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCapabilities:
                this->m_capabilities = variant.value<CPropertyIndexVariantMap>();
                break;
            case IndexModel:
                this->m_model.setPropertyByIndex(variant, index.copyFrontRemoved());;
                break;
            case IndexServer:
                this->m_server = variant.toString();
                break;
            case IndexUser:
                this->m_user.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexCallsign:
                this->m_user.setCallsign(variant.value<BlackMisc::Aviation::CCallsign>());
                break;
            case IndexVoiceCapabilities:
                this->m_voiceCapabilities.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }
    } // namespace
} // namespace
