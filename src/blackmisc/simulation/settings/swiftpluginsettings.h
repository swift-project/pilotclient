/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SETTINGS_SWIFTPLUGINSETTINGS_H
#define BLACKMISC_SIMULATION_SETTINGS_SWIFTPLUGINSETTINGS_H

#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            //! Settings for models
            class BLACKMISC_EXPORT CSwiftPluginSettings : public BlackMisc::CValueObject<CSwiftPluginSettings>
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexEmulatedSimulator = CPropertyIndexRef::GlobalIndexCSwiftPluignSettings,
                    IndexOwnModel,
                    IndexDefaultModel,
                    IndexLoggingFunctionCalls
                };

                //! Default constructor
                CSwiftPluginSettings();

                //! Emulated simualtor
                CSimulatorInfo getEmulatedSimulator() const { return m_emulatedSimulator; }

                //! Emulated simualtor
                void setEmulatedSimulator(const CSimulatorInfo &simulator);

                //! Get own model
                const CAircraftModel &getOwnModel() const { return m_ownModel; }

                //! Set own model
                void setOwnModel(const CAircraftModel &ownModel) { m_ownModel = ownModel; }

                //! Get default model
                const CAircraftModel &getDefaultModel() const { return m_defaultModel; }

                //! Set default model
                void setDefaultModel(const CAircraftModel &defaultModel) { m_defaultModel = defaultModel; }

                //! Log function calls?
                bool isLoggingFunctionCalls() const { return m_logFunctionCalls; }

                //! Log function calls?
                void setLoggingFunctionCalls(bool log) { m_logFunctionCalls = log; }

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                QVariant propertyByIndex(CPropertyIndexRef index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            private:
                CSimulatorInfo m_emulatedSimulator { CSimulatorInfo::P3D }; // simulator with default value
                CAircraftModel m_ownModel;
                CAircraftModel m_defaultModel;
                bool m_logFunctionCalls = true;

                BLACK_METACLASS(
                    CSwiftPluginSettings,
                    BLACK_METAMEMBER(emulatedSimulator),
                    BLACK_METAMEMBER(ownModel),
                    BLACK_METAMEMBER(defaultModel),
                    BLACK_METAMEMBER(logFunctionCalls)
                );
            };

            //! Trait for swift plugin settings
            struct TSwiftPlugin : public BlackMisc::TSettingTrait<CSwiftPluginSettings>
            {
                //! \copydoc BlackMisc::TSettingTrait::key
                static const char *key() { return "settingsswiftplugin"; }

                //! \copydoc BlackMisc::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("swift plugin"); return name; }

                //! \copydoc BlackMisc::TSettingTrait::isValid
                static bool isValid(const CSwiftPluginSettings &value, QString &) { return value.getEmulatedSimulator().isSingleSimulator(); }
            };
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSwiftPluginSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::Settings::CSwiftPluginSettings>)

#endif // guard
