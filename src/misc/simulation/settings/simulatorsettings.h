// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SETTINGS_SIMULATORSETTINGS_H
#define SWIFT_MISC_SIMULATION_SETTINGS_SIMULATORSETTINGS_H

#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/network/textmessage.h"
#include "misc/pq/length.h"
#include "misc/settingscache.h"
#include "misc/statusmessage.h"
#include "misc/propertyindexref.h"
#include "misc/directoryutils.h"
#include "misc/swiftmiscexport.h"

#include <QStringList>
#include <QObject>

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation::Settings, CSimulatorSettings)
SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation::Settings, CFsxP3DSettings)
SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation::Settings, CSimulatorMessagesSettings)

namespace swift::misc::simulation::settings
{
    //! Settings for simulator
    //! Driver independent parts (such as directories), also used in model loaders
    class SWIFT_MISC_EXPORT CSimulatorSettings : public CValueObject<CSimulatorSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexSimulatorDirectory = CPropertyIndexRef::GlobalIndexCSimulatorSettings,
            IndexModelDirectories,
            IndexModelExcludeDirectoryPatterns,
            IndexComIntegration, //!< COM unit integration
            IndexCGSource,
            IndexRecordOwnAircraftGnd,
            IndexRecordOwnAircraftGndRadius
        };

        //! Where we get the CG (aka vertical offset) from
        enum CGSource
        {
            CGFromSimulatorFirst,
            CGFromDBFirst,
            CGFromSimulatorOnly,
            CGFromDBOnly
        };

        //! CG source as string
        static const QString &cgSourceAsString(CGSource source);

        //! Default constructor
        CSimulatorSettings();

        //! Constructor
        CSimulatorSettings(const QString &simDir) : m_simulatorDirectory(simDir) {}

        //! Set simulator directory
        void setSimulatorDirectory(const QString &simulatorDirectory);

        //! Simulator directory
        const QString &getSimulatorDirectory() const;

        //! Setting has simulator setting
        bool hasSimulatorDirectory() const { return !this->getSimulatorDirectory().isEmpty(); }

        //! Set model directories
        void setModelDirectories(const QStringList &modelDirectories);

        //! Clear the model directories
        void clearModelDirectories();

        //! Set single model directory
        void setModelDirectory(const QString &modelDirectory);

        //! Add (if not exists) model directory
        bool addModelDirectory(const QString &modelDirectory);

        //! Model directory
        const QStringList &getModelDirectories() const;

        //! Having model directories?
        bool hasModelDirectories() const { return !this->getModelDirectories().isEmpty(); }

        //! Set exclude directories
        void setModelExcludeDirectories(const QStringList &excludeDirectories);

        //! Model exclude directoy patterns
        const QStringList &getModelExcludeDirectoryPatterns() const;

        //! Having model exclude directoy patterns?
        bool hasModelExcludeDirectoryPatterns() const { return !this->getModelExcludeDirectoryPatterns().isEmpty(); }

        //! COM unit integration
        bool isComIntegrated() const { return m_comIntegration; }

        //! COM unit integration
        bool setComIntegrated(bool integrated);

        //! CG source
        CGSource getCGSource() const { return static_cast<CGSource>(m_cgSource); }

        //! CG source
        bool setCGSource(CGSource source);

        //! Record GND values (of own aircraft)
        bool isRecordOwnAircraftGnd() const { return m_recordGnd; }

        //! Record GND values (of own aircraft)
        bool setRecordOwnAircraftGnd(bool record);

        //! Record GND values with radius
        swift::misc::physical_quantities::CLength getRecordedGndRadius() const { return m_recordedGndRadius; }

        //! Record GND values with radius
        bool setRecordedGndRadius(const swift::misc::physical_quantities::CLength &radius);

        //! Reset the paths
        void resetPaths();

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

    private:
        QString m_simulatorDirectory; //!< Simulator directory
        QStringList m_modelDirectories; //!< Model directory
        QStringList m_excludeDirectoryPatterns; //!< Exclude model directory
        bool m_comIntegration = false; //!< COM integration
        bool m_recordGnd = false; //!< Record GND values (of own aircraft)
        int m_cgSource = static_cast<int>(CGFromSimulatorFirst); //!< CG source
        physical_quantities::CLength m_recordedGndRadius { 250.0, physical_quantities::CLengthUnit::m() };

        SWIFT_METACLASS(
            CSimulatorSettings,
            SWIFT_METAMEMBER(simulatorDirectory),
            SWIFT_METAMEMBER(modelDirectories),
            SWIFT_METAMEMBER(excludeDirectoryPatterns),
            SWIFT_METAMEMBER(comIntegration),
            SWIFT_METAMEMBER(cgSource),
            SWIFT_METAMEMBER(recordGnd),
            SWIFT_METAMEMBER(recordedGndRadius));
    };

    //! Some P3D/FSX settings
    class SWIFT_MISC_EXPORT CFsxP3DSettings : public CValueObject<CFsxP3DSettings>
    {
    public:
        //! Default constructor
        CFsxP3DSettings() {}

        //! Constructor
        CFsxP3DSettings(bool simulatedObject, bool sbOffsets) : m_useSimulatedObjectAdding(simulatedObject),
                                                                m_useSbOffsets(sbOffsets)
        {}

        //! Use simulated object adding
        void setAddingAsSimulatedObjectEnabled(bool enable) { m_useSimulatedObjectAdding = enable; }

        //! Use simulated object adding
        bool isAddingAsSimulatedObjectEnabled() const { return m_useSimulatedObjectAdding; }

        //! Use SB offsets?
        void setSbOffsetsEnabled(bool enable) { m_useSbOffsets = enable; }

        //! Are SB offsets enabled
        bool isSbOffsetsEnabled() const { return m_useSbOffsets; }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

    private:
        bool m_useSimulatedObjectAdding = false; //!< COM integration
        bool m_useSbOffsets = true; //!< use the SB offset

        SWIFT_METACLASS(
            CFsxP3DSettings,
            SWIFT_METAMEMBER(useSimulatedObjectAdding),
            SWIFT_METAMEMBER(useSbOffsets));
    };

    //! Allows to have specific utility functions for each simulator
    class SWIFT_MISC_EXPORT CSpecializedSimulatorSettings
    {
    public:
        //! Get the generic settings
        const CSimulatorSettings &getGenericSettings() const { return m_genericSettings; }

        //! Ctor
        CSpecializedSimulatorSettings(const CSimulatorSettings &settings, const CSimulatorInfo &simulator) : m_genericSettings(settings), m_simulator(simulator) {}

        //! Ctor
        CSpecializedSimulatorSettings(const QString &simulatorDir, const CSimulatorInfo &simulator) : m_genericSettings(CSimulatorSettings(simulatorDir)), m_simulator(simulator) {}

        //! The generic settings
        const CSimulatorSettings &getSimulatorSettings() const { return m_genericSettings; }

        //! Default simulator path per simulator
        const QString &getDefaultSimulatorDirectory() const;

        //! Simulator directory or empty if default dir
        const QString &getSimulatorDirectoryIfNotDefault() const;

        //! Simulator directory or default path
        const QString &getSimulatorDirectoryOrDefault() const;

        //! Model directories or default
        QStringList getModelDirectoriesOrDefault() const;

        //! Model directories
        QStringList getModelDirectoriesFromSimulatorDirectoy() const;

        //! Model directories, then from simulator directory, then default
        QStringList getModelDirectoriesFromSimulatorDirectoryOrDefault() const;

        //! Model directories or empty if default
        const QStringList &getModelDirectoriesIfNotDefault() const;

        //! Default model exclude patterns
        const QStringList &getDefaultModelExcludeDirectoryPatterns() const;

        //! First model directoy
        QString getFirstModelDirectoryOrDefault() const;

        //! Model exclude patterns or empty if default
        const QStringList &getDefaultModelDirectories() const;

        //! Model exclude patterns or empty if default
        const QStringList &getModelExcludeDirectoryPatternsIfNotDefault() const;

        //! Model exclude patterns or empty if default
        const QStringList &getModelExcludeDirectoryPatternsOrDefault() const;

        //! \copydoc CSimulatorSettings::clearModelDirectories
        void clearModelDirectories() { m_genericSettings.clearModelDirectories(); }

        //! \copydoc CSimulatorSettings::addModelDirectory
        bool addModelDirectory(const QString &modelDirectory) { return m_genericSettings.addModelDirectory(modelDirectory); }

        //! \copydoc CSimulatorSettings::setSimulatorDirectory
        void setSimulatorDirectory(const QString &simDir) { m_genericSettings.setSimulatorDirectory(simDir); }

        //! Default model path per simulator
        static const QStringList &defaultModelDirectories(const CSimulatorInfo &simulator);

        //! Default simulator path per simulator
        static const QString &defaultSimulatorDirectory(const CSimulatorInfo &simulator);

        //! Default model exclude patterns per simulator
        static const QStringList &defaultModelExcludeDirectoryPatterns(const CSimulatorInfo &simulator);

    protected:
        CSimulatorSettings m_genericSettings; //!< the generic settings
        CSimulatorInfo m_simulator; //!< represented simulator
    };

    //! XPlane specific settings
    class SWIFT_MISC_EXPORT CXPlaneSimulatorSettings : public CSpecializedSimulatorSettings
    {
    public:
        //! Constructor
        CXPlaneSimulatorSettings(const CSimulatorSettings &settings) : CSpecializedSimulatorSettings(settings, CSimulatorInfo(CSimulatorInfo::XPLANE))
        {}

        //! Constructor
        CXPlaneSimulatorSettings(const CSpecializedSimulatorSettings &settings) : CXPlaneSimulatorSettings(settings.getGenericSettings())
        {}

        //! Plugin directory or default plugin dir
        QString getPluginDirOrDefault() const;
    };

    //! Trait for simulator settings
    struct TSimulatorFsx : public TSettingTrait<CSimulatorSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "settingssimulatorfsx"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("FSX settings");
            return name;
        }
    };

    //! Trait for simulator settings
    struct TSimulatorFs9 : public TSettingTrait<CSimulatorSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "settingssimulatorfs9"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("FS9 settings");
            return name;
        }
    };

    //! Trait for simulator settings
    struct TSimulatorP3D : public TSettingTrait<CSimulatorSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "settingssimulatorp3d"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("P3D settings");
            return name;
        }
    };

    //! Trait for simulator settings
    struct TSimulatorMsfs : public TSettingTrait<CSimulatorSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "settingssimulatormsfs"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("MSFS settings");
            return name;
        }
    };

    //! Selected P3D version (64bit)
    struct TP3DVersion : public TSettingTrait<QString>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "simulator/p3dversion"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("P3D version");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const QString &defaultValue()
        {
            static const QString version("4.3");
            return version;
        }
    };

    //! Some details for FSX
    struct TFsxDetailsSettings : public TSettingTrait<CFsxP3DSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "simulator/fsxdetailsettings"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("FSX details");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const CFsxP3DSettings &defaultValue()
        {
            static const CFsxP3DSettings d(true, true);
            return d;
        }
    };

    //! Some details for P3D
    struct TP3DDetailsSettings : public TSettingTrait<CFsxP3DSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "simulator/p3ddetailsettings"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("P3D details");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const CFsxP3DSettings &defaultValue()
        {
            static const CFsxP3DSettings d(false, true);
            return d;
        }
    };

    //! Trait for simulator settings
    struct SWIFT_MISC_EXPORT TSimulatorXP : public TSettingTrait<CSimulatorSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "settingssimulatorxplane"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("XPlane settings");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const CSimulatorSettings &value, QString &reason);
    };

    //! Trait for simulator settings
    struct TSimulatorFG : public TSettingTrait<CSimulatorSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "settingssimulatorfg"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("FG settings");
            return name;
        }
    };

    //! Bundle of detail settings
    class SWIFT_MISC_EXPORT CMultiSimulatorDetailsSettings : public QObject
    {
    public:
        //! Settings per simulator
        CFsxP3DSettings getSettings(const CSimulatorInfo &sim) const;

        //! Set settings per simulator
        CStatusMessage setSettings(const CFsxP3DSettings &settings, const CSimulatorInfo &simulator);

        //! Set settings per simulator
        CStatusMessage setAndSaveSettings(const CFsxP3DSettings &settings, const CSimulatorInfo &simulator);

    private:
        CSetting<TFsxDetailsSettings> m_simFsx { this }; //!< FSX settings
        CSetting<TP3DDetailsSettings> m_simP3D { this }; //!< P3D settings
    };

    //! Bundle of settings for all simulators
    class SWIFT_MISC_EXPORT CMultiSimulatorSettings : public QObject
    {
        Q_OBJECT

    public:
        //! Construtor
        CMultiSimulatorSettings(QObject *parent = nullptr);

        //! Settings per simulator
        CSimulatorSettings getSettings(const CSimulatorInfo &simulator) const;

        //! Specialized simulator settings
        CSpecializedSimulatorSettings getSpecializedSettings(const CSimulatorInfo &simulator) const;

        //! Set settings per simulator
        CStatusMessage setSettings(const CSimulatorSettings &settings, const CSimulatorInfo &simulator);

        //! Set model directory per simulator
        CStatusMessage addModelDirectory(const QString &modelDirectory, const CSimulatorInfo &simulator);

        //! Clear the model directory
        CStatusMessage clearModelDirectories(const CSimulatorInfo &simulator);

        //! Set settings per simulator, but do NOT save yet, but validate
        //! \remark can be simulator specific
        CStatusMessageList setAndValidateSettings(const CSimulatorSettings &settings, const CSimulatorInfo &simulator);

        //! Set settings per simulator
        CStatusMessage setAndSaveSettings(const CSimulatorSettings &settings, const CSimulatorInfo &simulator);

        //! Set settings per simulator
        CStatusMessage setAndSaveSettings(const CSpecializedSimulatorSettings &settings, const CSimulatorInfo &simulator);

        //! Set settings per simulator
        CStatusMessage saveSettings(const CSimulatorInfo &simulator);

        //! Simulator directory or empty if default dir
        QString getSimulatorDirectoryIfNotDefault(const CSimulatorInfo &simulator) const;

        //! Simulator directory or default model path per simulator
        QString getSimulatorDirectoryOrDefault(const CSimulatorInfo &simulator) const;

        //! Model directory or or empty if default
        QStringList getModelDirectoriesIfNotDefault(const CSimulatorInfo &simulator) const;

        //! Model directory or default model path per simulator
        QStringList getModelDirectoriesOrDefault(const CSimulatorInfo &simulator) const;

        //! First model directoy
        QString getFirstModelDirectoryOrDefault(const CSimulatorInfo &simulator) const;

        //! Model exclude patterns or empty if default
        QStringList getModelExcludeDirectoryPatternsIfNotDefault(const CSimulatorInfo &simulator) const;

        //! Model exclude patterns per simulator
        QStringList getModelExcludeDirectoryPatternsOrDefault(const CSimulatorInfo &simulator) const;

        //! Reset to defaults
        void resetToDefaults(const CSimulatorInfo &simulator);

        //! Default model path per simulator
        const QStringList &defaultModelDirectories(const CSimulatorInfo &simulator) const;

    signals:
        //! Simulator settings have been changed
        void settingsChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

    private:
        CSetting<TSimulatorFsx> m_simSettingsFsx { this, &CMultiSimulatorSettings::onFsxSettingsChanged }; //!< FSX settings
        CSetting<TSimulatorFs9> m_simSettingsFs9 { this, &CMultiSimulatorSettings::onFs9SettingsChanged }; //!< FS9 settings
        CSetting<TSimulatorP3D> m_simSettingsP3D { this, &CMultiSimulatorSettings::onP3DSettingsChanged }; //!< P3D settings
        CSetting<TSimulatorMsfs> m_simSettingsMsfs { this, &CMultiSimulatorSettings::onMsfsSettingsChanged }; //!< MSFS settings
        CSetting<TSimulatorXP> m_simSettingsXP { this, &CMultiSimulatorSettings::onXPSettingsChanged }; //!< XP settings
        CSetting<TSimulatorFG> m_simSettingsFG { this, &CMultiSimulatorSettings::onFGSettingsChanged }; //!< FG settings

        //! @{
        //! Settings changed, this will only detect if settings are changed elsewhere
        void onFsxSettingsChanged();
        void onFs9SettingsChanged();
        void onP3DSettingsChanged();
        void onMsfsSettingsChanged();
        void onXPSettingsChanged();
        void onFGSettingsChanged();
        //! @}

        //! Emit the signal, allows breakpoint
        void emitSettingsChanged(const CSimulatorInfo &simInfo);
    };

    //! Settings regarding message handling.
    //! Driver independent part, related to network
    class SWIFT_MISC_EXPORT CSimulatorMessagesSettings : public CValueObject<CSimulatorMessagesSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexTechnicalLogSeverity = CPropertyIndexRef::GlobalIndexCSimulatorMessageSettings,
            IndexRelayTextMessage,
            IndexRelayGloballyEnabled
        };

        //! Enabled matching mode flags
        enum TextMessageTypeFlag
        {
            NoTextMessages = 0,
            TextMessagesUnicom = 1 << 0,
            TextMessagesCom1 = 1 << 1,
            TextMessagesCom2 = 1 << 2,
            TextMessagePrivate = 1 << 3,
            TextMessageSupervisor = 1 << 4,
            TextMessagesAll = TextMessagesUnicom | TextMessagesCom1 | TextMessagesCom2 | TextMessagePrivate
        };
        Q_DECLARE_FLAGS(TextMessageType, TextMessageTypeFlag)

        //! Default constructor
        CSimulatorMessagesSettings() {}

        //! Log severity
        void setTechnicalLogSeverity(CStatusMessage::StatusSeverity severity);

        //! Globally enable / disable
        void setRelayGloballyEnabled(bool enabled) { m_relayGloballyEnabled = enabled; }

        //! Globally enabled?
        bool isRelayGloballyEnabled() const { return m_relayGloballyEnabled; }

        //! No technical messages
        void disableTechnicalMessages();

        //! Relay (technical) error messages
        bool isRelayErrorsMessages() const;

        //! Relay (technical) warning messages
        bool isRelayWarningMessages() const;

        //! Relay (technical) info messages
        bool isRelayInfoMessages() const;

        //! Relay any message
        bool isRelayTechnicalMessages() const;

        //! Relay the following message types
        void setRelayTextMessages(CSimulatorMessagesSettings::TextMessageType messageType);

        //! Relay supervisor messages
        bool isRelaySupervisorTextMessages() const;

        //! Relay private messages
        bool isRelayPrivateTextMessages() const;

        //! Relay UNICOM messages
        bool isRelayUnicomTextMessages() const;

        //! Relay COM1 text message
        bool isRelayCom1TextMessages() const;

        //! Relay COM2 text message
        bool isRelayCom2TextMessages() const;

        //! Relay given text message
        bool relayThisTextMessage(const network::CTextMessage &msg, const CSimulatedAircraft &aircraft) const;

        //! Relay this particular message
        bool relayThisStatusMessage(const CStatusMessage &message) const;

        //! Relayed text messages
        CSimulatorMessagesSettings::TextMessageType getRelayedTextMessageTypes() const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

    private:
        int m_technicalLogLevel = CStatusMessage::SeverityError; //!< log level
        int m_messageType = static_cast<int>(TextMessagePrivate | TextMessageSupervisor);
        bool m_relayGloballyEnabled = true; //!< messsage relay enabled to simulator

        SWIFT_METACLASS(
            CSimulatorMessagesSettings,
            SWIFT_METAMEMBER(technicalLogLevel),
            SWIFT_METAMEMBER(messageType));
    };

    //! Trait for simulator message settings
    struct TSimulatorMessages : public TSettingTrait<CSimulatorMessagesSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "settingssimulatormessages"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Simulator messages");
            return name;
        }
    };

} // ns

Q_DECLARE_METATYPE(swift::misc::simulation::settings::CSimulatorSettings)
Q_DECLARE_METATYPE(swift::misc::simulation::settings::CSimulatorSettings::CGSource)
Q_DECLARE_METATYPE(swift::misc::simulation::settings::CFsxP3DSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::settings::CSimulatorSettings>)
Q_DECLARE_METATYPE(swift::misc::simulation::settings::CSimulatorMessagesSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::settings::CSimulatorMessagesSettings>)
Q_DECLARE_METATYPE(swift::misc::simulation::settings::CSimulatorMessagesSettings::TextMessageTypeFlag)
Q_DECLARE_METATYPE(swift::misc::simulation::settings::CSimulatorMessagesSettings::TextMessageType)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::misc::simulation::settings::CSimulatorMessagesSettings::TextMessageType)

#endif // guard
