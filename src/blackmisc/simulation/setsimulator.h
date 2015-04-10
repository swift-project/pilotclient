/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SETTINGS_SIMULATOR_H
#define BLACKMISC_SETTINGS_SIMULATOR_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/pq/time.h"
#include "simulatorplugininfo.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            //! Value object encapsulating information of simulator related settings.
            //! \deprecated will be removed with new MS settings
            class BLACKMISC_EXPORT CSettingsSimulator : public BlackMisc::CValueObject<CSettingsSimulator>
            {
            public:
                //! Default constructor.
                CSettingsSimulator();

                //! Path
                static const QString &ValueSelectedDriver()
                {
                    static const QString value("selecteddriver");
                    return value;
                }

                //! Path
                static const QString &ValueSyncTimeOffset()
                {
                    static const QString value("synctimeoffset");
                    return value;
                }

                //! Path
                static const QString &ValueSyncTime()
                {
                    static const QString value("synctime");
                    return value;
                }

                //! Selected driver
                const BlackMisc::Simulation::CSimulatorPluginInfo &getSelectedPlugin() const { return this->m_selectedPlugin; }

                //! Selected driver
                void setSelectedPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &plugin) { this->m_selectedPlugin = plugin; }

                //! Time synchronization offset time
                const BlackMisc::PhysicalQuantities::CTime &getSyncTimeOffset() const { return this->m_timeSyncOffset;}

                //! Set time synchronization offset time
                void setSyncTimeOffset(const BlackMisc::PhysicalQuantities::CTime &offset) { this->m_timeSyncOffset = offset; this->m_timeSyncOffset.switchUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin());}

                //! Time syncronization enabled?
                bool isTimeSyncEnabled() const { return this->m_timeSync;}

                //! Set time synchronization
                void setTimeSyncEnabled(bool enabled) { this->m_timeSync = enabled; }

                //! Init with meaningful default values
                void initDefaultValues();

                //! \copydoc BlackCore::IContextSettings::value
                virtual BlackMisc::CStatusMessage value(const QString &path, const QString &command, const BlackMisc::CVariant &value, bool &changedFlag);

            protected:
                //! \copydoc CValueObject::convertToQString
                virtual QString convertToQString(bool i18n = false) const override;

            private:
                BLACK_ENABLE_TUPLE_CONVERSION(CSettingsSimulator)
                BlackMisc::Simulation::CSimulatorPluginInfo m_selectedPlugin;
                bool m_timeSync = false;
                BlackMisc::PhysicalQuantities::CTime m_timeSyncOffset;
            };

        } // namespace
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSettingsSimulator)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Simulation::Settings::CSettingsSimulator, (o.m_selectedPlugin, o.m_timeSync, o.m_timeSyncOffset))

#endif // guard
