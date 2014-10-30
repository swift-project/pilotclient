/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "setsimulator.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Settings;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSim
{
    namespace Settings
    {
        /*
         * Constructor
         */
        CSettingsSimulator::CSettingsSimulator()
        {
            this->initDefaultValues();
        }

        /*
         * Convert to string
         */
        QString CSettingsSimulator::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s("Sel.driver:");
            s.append(" ").append(m_selectedPlugin.toQString(i18n));
            return s;
        }

        /*
         * Default values
         */
        void CSettingsSimulator::initDefaultValues()
        {
            this->m_selectedPlugin = CSimulatorInfo::FSX();
            this->m_timeSyncOffset = CTime(0, CTimeUnit::hrmin());
            this->m_timeSync = false;
        }

        /*
         * Value
         */
        BlackMisc::CStatusMessage CSettingsSimulator::value(const QString &path, const QString &command, const CVariant &value, bool &changedFlag)
        {
            // TODO: This needs to be refactored to a smarter way to delegate commands
            changedFlag = false;
            if (path == CSettingsSimulator::ValueSelectedDriver())
            {
                if (command == CSettingUtilities::CmdAdd() || command == CSettingUtilities::CmdUpdate())
                {
                    CSimulatorInfo v = value.value<CSimulatorInfo>();
                    changedFlag = (v != this->m_selectedPlugin);
                    this->m_selectedPlugin = v;
                    return CLogMessage(CLogCategory::settingsUpdate()).info("selected driver%1 changed") << (changedFlag ? "" : " not");
                }
                return CLogMessage(CLogCategory::validation()).error("wrong command: %1") << command;
            }
            else if (path == CSettingsSimulator::ValueSyncTime())
            {
                if (command == CSettingUtilities::CmdAdd() || command == CSettingUtilities::CmdUpdate())
                {
                    bool v = value.value<bool>();
                    changedFlag = (v != this->m_timeSync);
                    this->m_timeSync = v;
                    return CLogMessage(CLogCategory::settingsUpdate()).info("time synchronization%1 changed") << (changedFlag ? "" : " not");
                }
                return CLogMessage(CLogCategory::validation()).error("wrong command: %1") << command;
            }
            else if (path == CSettingsSimulator::ValueSyncTimeOffset())
            {
                if (command == CSettingUtilities::CmdAdd() || command == CSettingUtilities::CmdUpdate())
                {
                    CTime v = value.value<CTime>();
                    changedFlag = (v != this->m_timeSyncOffset);
                    this->m_timeSyncOffset = v;
                    return CLogMessage(CLogCategory::settingsUpdate()).info("time synchronization offset%1 changed") << (changedFlag ? "" : " not");
                }
                return CLogMessage(CLogCategory::validation()).error("wrong command: %1") << command;
            }
            else
            {
                return CLogMessage(CLogCategory::validation()).error("wrong path: %1") << path;
            }
        }
    } // namespace
} // namespace
