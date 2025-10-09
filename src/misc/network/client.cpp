// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/client.h"

#include <QPixmap>
#include <QString>
#include <QStringBuilder>
#include <QStringList>

#include "misc/aviation/callsign.h"
#include "misc/iconlist.h"
#include "misc/mixin/mixincompare.h"
#include "misc/propertyindexvariantmap.h" // needed for mixin::Index::apply

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::network, CClient)

namespace swift::misc::network
{
    QString CClient::convertToQString(bool i18n) const
    {
        return m_user.toQString(i18n) % QStringLiteral(" capabilites: ") % this->getCapabilitiesAsString() %
               QStringLiteral(" model: ") % m_modelString %
               (m_server.isEmpty() ? QString() : QStringLiteral("server: ") % m_server);
    }

    CClient::CClient(const aviation::CCallsign &callsign, const QString &modelString)
        : m_user(CUser(callsign)), m_modelString(modelString.trimmed())
    {}

    bool CClient::isValid() const { return m_user.hasCallsign(); }

    void CClient::addCapability(CClient::Capability capability)
    {
        Capabilities cap = this->getCapabilities();
        cap |= capability;
        this->setCapabilities(cap);
    }

    void CClient::removeCapability(CClient::Capability capability)
    {
        Capabilities cap = this->getCapabilities();
        cap &= ~capability;
        this->setCapabilities(cap);
    }

    void CClient::setCapabilities(const Capabilities &capabilities) { m_capabilities = capabilities; }

    QString CClient::getCapabilitiesAsString() const
    {
        QStringList sl;
        const Capabilities cap = this->getCapabilities();
        if (cap.testFlag(FsdAtisCanBeReceived)) sl << "ATIS";
        if (cap.testFlag(FsdWithInterimPositions)) sl << "interim pos.";
        if (cap.testFlag(FsdWithIcaoCodes)) sl << "ICAO";
        if (cap.testFlag(FsdWithAircraftConfig)) sl << "aircraft config";
        if (cap.testFlag(FsdWithGroundFlag)) sl << "gnd.flag";
        if (cap.testFlag(FsdModelString)) sl << "modelstring";
        if (cap.testFlag(FsdWithVisualPositions)) sl << "visual pos.";
        if (sl.isEmpty()) { return {}; }
        return sl.join(", ");
    }

    bool CClient::hasCapability(Capability capability) const { return this->getCapabilities().testFlag(capability); }

    bool CClient::hasAircraftPartsCapability() const { return this->hasCapability(FsdWithAircraftConfig); }

    bool CClient::hasGndFlagCapability() const { return this->hasCapability(FsdWithGroundFlag); }

    bool CClient::setUserCallsign(const aviation::CCallsign &callsign) { return m_user.setCallsign(callsign); }

    void CClient::setQueriedModelString(const QString &modelString)
    {
        m_modelString = modelString.trimmed();
        if (!modelString.isEmpty()) { this->addCapability(CClient::FsdModelString); }
    }

    CIcons::IconIndex CClient::toIcon() const
    {
        if (!m_swift) { return m_user.toIcon(); }

        return CIcons::Swift16;
    }

    QVariant CClient::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexCapabilities: return QVariant::fromValue(m_capabilities);
        case IndexCapabilitiesString: return QVariant(this->getCapabilitiesAsString());
        case IndexCallsign: return this->getCallsign().propertyByIndex(index.copyFrontRemoved());
        case IndexUser: return this->getUser().propertyByIndex(index.copyFrontRemoved());
        case IndexModelString: return QVariant(m_modelString);
        case IndexServer: return QVariant(m_server);
        case IndexVoiceCapabilities: return m_voiceCapabilities.propertyByIndex(index.copyFrontRemoved());
        case IndexVoiceCapabilitiesPixmap: return QVariant::fromValue(CIcon(m_voiceCapabilities.toIcon()).toPixmap());
        case IndexVoiceCapabilitiesIcon: return QVariant::fromValue(CIcon(m_voiceCapabilities.toIcon()));
        case IndexVoiceCapabilitiesString: return QVariant(m_voiceCapabilities.toQString(true));
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CClient::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CClient>();
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexCapabilities: m_capabilities = variant.toInt(); break;
        case IndexModelString: m_modelString = variant.toString(); break;
        case IndexServer: m_server = variant.toString(); break;
        case IndexUser: m_user.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexCallsign: m_user.setCallsign(variant.value<swift::misc::aviation::CCallsign>()); break;
        case IndexVoiceCapabilities: m_voiceCapabilities.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // namespace swift::misc::network
