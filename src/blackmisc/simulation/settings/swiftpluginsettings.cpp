/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "swiftpluginsettings.h"
#include "blackmisc/stringutils.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            CSwiftPluginSettings::CSwiftPluginSettings()
            { }

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

            CVariant CSwiftPluginSettings::propertyByIndex(const CPropertyIndex &index) const
            {
                if (index.isMyself()) { return CVariant::from(*this); }
                const ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexEmulatedSimulator:
                    return this->m_emulatedSimulator.propertyByIndex(index.copyFrontRemoved());
                case IndexOwnModel:
                    return CVariant::from(m_ownModel);
                case IndexDefaultModel:
                    return CVariant::from(m_defaultModel);
                case IndexLoggingFunctionCalls:
                    return CVariant::from(m_logFunctionCalls);
                default:
                    return CValueObject::propertyByIndex(index);
                }
            }

            void CSwiftPluginSettings::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
            {
                if (index.isMyself()) { (*this) = variant.to<CSwiftPluginSettings>(); return; }
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
    } // ns
} // ns
