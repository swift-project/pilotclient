// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/settings/swiftpluginsettings.h"
#include "misc/stringutils.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation::settings, CSwiftPluginSettings)

namespace swift::misc::simulation::settings
{
    CSwiftPluginSettings::CSwiftPluginSettings()
    {}

    void CSwiftPluginSettings::setEmulatedSimulator(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");
        m_emulatedSimulator = simulator;
    }

    QString CSwiftPluginSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        static const QString ms("Emulated simulator: %1, default model: '%2', log.function calls: %3");
        return ms.arg(this->m_emulatedSimulator.toQString(), m_defaultModel.getModelStringAndDbKey(), boolToYesNo(m_logFunctionCalls));
    }

    QVariant CSwiftPluginSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexEmulatedSimulator:
            return this->m_emulatedSimulator.propertyByIndex(index.copyFrontRemoved());
        case IndexOwnModel:
            return QVariant::fromValue(m_ownModel);
        case IndexDefaultModel:
            return QVariant::fromValue(m_defaultModel);
        case IndexLoggingFunctionCalls:
            return QVariant::fromValue(m_logFunctionCalls);
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CSwiftPluginSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CSwiftPluginSettings>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexEmulatedSimulator:
            this->m_emulatedSimulator.setPropertyByIndex(index.copyFrontRemoved(), variant);
            break;
        case IndexOwnModel:
            this->m_ownModel.setPropertyByIndex(index.copyFrontRemoved(), variant);
            break;
        case IndexDefaultModel:
            this->m_defaultModel.setPropertyByIndex(index.copyFrontRemoved(), variant);
            break;
        case IndexLoggingFunctionCalls:
            this->m_logFunctionCalls = variant.toBool();
            break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }
} // ns
