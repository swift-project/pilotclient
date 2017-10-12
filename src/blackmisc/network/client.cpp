/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/client.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/compare.h"
#include "blackmisc/icon.h"

#include <QPixmap>
#include <QString>
#include <QStringList>
#include <tuple>

namespace BlackMisc
{
    namespace Network
    {

        QString CClient::convertToQString(bool i18n) const
        {
            QString s = this->m_user.toQString(i18n);
            s.append(" capabilites: ").append(this->getCapabilitiesAsString());
            s.append(" model: ").append(this->m_modelString);
            if (!this->m_server.isEmpty()) { s.append(" server:").append(this->m_server); }
            return s;
        }

        CClient::CClient(const Aviation::CCallsign &callsign, const QString &modelString) :
            m_user(CUser(callsign)), m_modelString(modelString.trimmed()) {}

        bool CClient::isValid() const
        {
            return this->m_user.hasValidCallsign();
        }

        void CClient::setCapability(bool hasCapability, CClient::Capabilities capability)
        {
            this->m_capabilities.addValue(static_cast<int>(capability), hasCapability);
        }

        void CClient::setCapabilities(const CPropertyIndexVariantMap &capabilities)
        {
            this->m_capabilities = capabilities;
        }

        QString CClient::getCapabilitiesAsString() const
        {
            QStringList sl;
            if (this->hasCapability(FsdAtisCanBeReceived)) sl << "ATIS";
            if (this->hasCapability(FsdWithInterimPositions)) sl << "interim pos.";
            if (this->hasCapability(FsdWithIcaoCodes)) sl << "ICAO";
            if (this->hasCapability(FsdWithAircraftConfig)) sl << "aircraft config";
            if (sl.isEmpty()) return "";
            return sl.join(", ");
        }

        bool CClient::hasCapability(CClient::Capabilities capability) const
        {
            if (this->m_capabilities.contains(capability))
            {
                return this->m_capabilities.value(capability).toBool();
            }
            else
            {
                return false;
            }
        }

        void CClient::setUserCallsign(const Aviation::CCallsign &callsign)
        {
            this->m_user.setCallsign(callsign);
        }

        CVariant CClient::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCapabilities:
                return CVariant::fromValue(this->m_capabilities);
            case IndexCapabilitiesString:
                return CVariant(this->getCapabilitiesAsString());
            case IndexCallsign:
                return this->getCallsign().propertyByIndex(index.copyFrontRemoved());
            case IndexUser:
                return this->getUser().propertyByIndex(index.copyFrontRemoved());
            case IndexModelString:
                return CVariant(this->m_modelString);
            case IndexServer:
                return CVariant(this->m_server);
            case IndexVoiceCapabilities:
                return this->m_voiceCapabilities.propertyByIndex(index.copyFrontRemoved());
            case IndexVoiceCapabilitiesPixmap:
                return CVariant::from(this->m_voiceCapabilities.toPixmap());
            case IndexVoiceCapabilitiesIcon:
                return CVariant::fromValue(this->m_voiceCapabilities.toIcon());
            case IndexVoiceCapabilitiesString:
                return CVariant(this->m_voiceCapabilities.toQString(true));
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CClient::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CClient>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCapabilities:
                this->m_capabilities = variant.value<CPropertyIndexVariantMap>();
                break;
            case IndexModelString:
                this->m_modelString = variant.toQString();
                break;
            case IndexServer:
                this->m_server = variant.toQString();
                break;
            case IndexUser:
                this->m_user.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexCallsign:
                this->m_user.setCallsign(variant.value<BlackMisc::Aviation::CCallsign>());
                break;
            case IndexVoiceCapabilities:
                this->m_voiceCapabilities.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }
    } // namespace
} // namespace
