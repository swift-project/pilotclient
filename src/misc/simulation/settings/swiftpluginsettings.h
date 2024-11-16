// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SETTINGS_SWIFTPLUGINSETTINGS_H
#define SWIFT_MISC_SIMULATION_SETTINGS_SWIFTPLUGINSETTINGS_H

#include "misc/propertyindexref.h"
#include "misc/settingscache.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation::Settings, CSwiftPluginSettings)

namespace swift::misc::simulation::settings
{
    //! Settings for models
    class SWIFT_MISC_EXPORT CSwiftPluginSettings : public swift::misc::CValueObject<CSwiftPluginSettings>
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

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

    private:
        CSimulatorInfo m_emulatedSimulator { CSimulatorInfo::P3D }; // simulator with default value
        CAircraftModel m_ownModel;
        CAircraftModel m_defaultModel;
        bool m_logFunctionCalls = true;

        SWIFT_METACLASS(
            CSwiftPluginSettings,
            SWIFT_METAMEMBER(emulatedSimulator),
            SWIFT_METAMEMBER(ownModel),
            SWIFT_METAMEMBER(defaultModel),
            SWIFT_METAMEMBER(logFunctionCalls));
    };

    //! Trait for swift plugin settings
    struct TSwiftPlugin : public swift::misc::TSettingTrait<CSwiftPluginSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "settingsswiftplugin"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("swift plugin");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const CSwiftPluginSettings &value, QString &) { return value.getEmulatedSimulator().isSingleSimulator(); }
    };
} // namespace swift::misc::simulation::settings

Q_DECLARE_METATYPE(swift::misc::simulation::settings::CSwiftPluginSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::settings::CSwiftPluginSettings>)

#endif // guard
