/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SETTINGS_SIMULATORSETTINGS_H
#define BLACKMISC_SIMULATION_SETTINGS_SIMULATORSETTINGS_H

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/weather/weatherscenario.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"

#include <QStringList>
#include <QObject>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            //! Settings for simulator
            //! Driver independent part also used in loaders (such as directories)
            class BLACKMISC_EXPORT CSimulatorSettings :
                public CValueObject<CSimulatorSettings>
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexSimulatorDirectory = CPropertyIndex::GlobalIndexCSimulatorSettings,
                    IndexModelDirectory,
                    IndexModelExcludeDirectoryPatterns
                };

                //! Default constructor
                CSimulatorSettings();

                //! Set simulator directory
                void setSimulatorDirectory(const QString &simulatorDirectory);

                //! Simulator directory
                const QString &getSimulatorDirectory() const;

                //! Setting has simulator setting
                bool hasSimulatorDirectory() const;

                //! Set model directories
                void setModelDirectories(const QStringList &modelDirectories);

                //! Set single model directory
                void setModelDirectory(const QString &modelDirectory);

                //! Model directory
                const QStringList &getModelDirectories() const;

                //! Set exclude directories
                void setModelExcludeDirectories(const QStringList &excludeDirectories);

                //! Margins for given dock widget
                const QStringList &getModelExcludeDirectoryPatterns() const;

                //! Reset the paths
                void resetPaths();

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

                //! To string
                QString convertToQString(const QString &separator, bool i18n = false) const;

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                CVariant propertyByIndex(const CPropertyIndex &index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            private:
                QString     m_simulatorDirectory;         //! Simulator directory
                QStringList m_modelDirectories;           //!< Model directory
                QStringList m_excludeDirectoryPatterns;   //!< Exclude model directory

                BLACK_METACLASS(
                    CSimulatorSettings,
                    BLACK_METAMEMBER(simulatorDirectory),
                    BLACK_METAMEMBER(modelDirectories),
                    BLACK_METAMEMBER(excludeDirectoryPatterns)
                );
            };

            //! Trait for simulator settings
            struct TSimulatorFsx : public TSettingTrait<CSimulatorSettings>
            {
                //! \copydoc BlackCore::TSettingTrait::key
                static const char *key() { return "settingssimulatorfsx"; }

                //! \copydoc BlackCore::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("FSX"); return name; }
            };

            //! Trait for simulator settings
            struct TSimulatorFs9 : public TSettingTrait<CSimulatorSettings>
            {
                //! \copydoc BlackCore::TSettingTrait::key
                static const char *key() { return "settingssimulatorfs9"; }

                //! \copydoc BlackCore::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("FS9"); return name; }
            };

            //! Trait for simulator settings
            struct TSimulatorP3D : public TSettingTrait<CSimulatorSettings>
            {
                //! \copydoc BlackCore::TSettingTrait::key
                static const char *key() { return "settingssimulatorp3d"; }

                //! \copydoc BlackCore::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("P3D"); return name; }
            };

            //! Trait for simulator settings
            struct TSimulatorXP : public TSettingTrait<CSimulatorSettings>
            {
                //! \copydoc BlackCore::TSettingTrait::key
                static const char *key() { return "settingssimulatorxplane"; }

                //! \copydoc BlackCore::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("XPlane"); return name; }
            };

            //! Bundle of settings for all simulators
            class BLACKMISC_EXPORT CMultiSimulatorSettings : public QObject
            {
                Q_OBJECT

            public:
                //! Construtor
                CMultiSimulatorSettings(QObject *parent = nullptr);

                //! Settings per simulator
                CSimulatorSettings getSettings(const CSimulatorInfo &simulator) const;

                //! Set settings per simulator
                CStatusMessage setSettings(const CSimulatorSettings &settings, const CSimulatorInfo &simulator);

                //! Set settings per simulator
                CStatusMessage setAndSaveSettings(const CSimulatorSettings &settings, const CSimulatorInfo &simulator);

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

                //! Default model path per simulator
                QStringList getDefaultModelDirectories(const CSimulatorInfo &simulator) const;

                //! Model exclude patterns or empty if default
                QStringList getModelExcludeDirectoryPatternsIfNotDefault(const CSimulatorInfo &simulator) const;

                //! Model exclude patterns per simulator
                QStringList getModelExcludeDirectoryPatternsOrDefault(const CSimulatorInfo &simulator) const;

                //! Default model exclude patterns per simulator
                QStringList getDefaultModelExcludeDirectoryPatterns(const CSimulatorInfo &simulator) const;

                //! Reset to defaults
                void resetToDefaults(const CSimulatorInfo &simulator);

                //! Default simulator path per simulator
                static QString getDefaultSimulatorDirectory(const CSimulatorInfo &simulator);

            private:
                CSetting<Settings::TSimulatorFsx> m_simSettingsFsx {this}; //!< FSX cache
                CSetting<Settings::TSimulatorFs9> m_simSettingsFs9 {this}; //!< FS9 cache
                CSetting<Settings::TSimulatorP3D> m_simSettingsP3D {this}; //!< P3D cache
                CSetting<Settings::TSimulatorXP>  m_simSettingsXP  {this}; //!< XP cache
            };

            //! Settings regarding message handling.
            //! Driver independent part, related to network
            class BLACKMISC_EXPORT CSimulatorMessagesSettings :
                public CValueObject<CSimulatorMessagesSettings>
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexTechnicalLogSeverity = CPropertyIndex::GlobalIndexCSimulatorMessageSettings,
                    IndexTextMessageRelay,
                    IndexGloballyEnabled
                };

                //! Enabled matching mode flags
                enum TextMessageTypeFlag
                {
                    NoTextMessages        = 0,
                    TextMessagesUnicom    = 1 << 0,
                    TextMessagesCom1      = 1 << 1,
                    TextMessagesCom2      = 1 << 2,
                    TextMessagePrivate    = 1 << 3,
                    TextMessageSupervisor = 1 << 4,
                    TextMessagesAll       = TextMessagesUnicom | TextMessagesCom1 | TextMessagesCom2 | TextMessagePrivate
                };
                Q_DECLARE_FLAGS(TextMessageType, TextMessageTypeFlag)

                //! Default constructor
                CSimulatorMessagesSettings();

                //! Log severity
                void setTechnicalLogSeverity(CStatusMessage::StatusSeverity severity);

                //! Globally enable / disable
                void setGloballyEnabled(bool enabled) { m_globallyEnabled = enabled; }

                //! Globally enabled?
                bool isGloballyEnabled() const { return m_globallyEnabled; }

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
                bool relayThisTextMessage(const Network::CTextMessage &msg, const CSimulatedAircraft &aircraft) const;

                //! Relay this particular message
                bool relayThisStatusMessage(const CStatusMessage &message) const;

                //! Relayed text messages
                CSimulatorMessagesSettings::TextMessageType getRelayedTextMessageTypes() const;

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                BlackMisc::CVariant propertyByIndex(const CPropertyIndex &index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            private:
                int m_technicalLogLevel = CStatusMessage::SeverityError; //!< Simulator directory
                int m_messageType = static_cast<int>(TextMessagePrivate | TextMessageSupervisor);
                bool m_globallyEnabled = true; //!< messsage relay enabled to simulator

                BLACK_METACLASS(
                    CSimulatorMessagesSettings,
                    BLACK_METAMEMBER(technicalLogLevel),
                    BLACK_METAMEMBER(messageType)
                );
            };

            //! Trait for simulator message settings
            struct TSimulatorMessages : public TSettingTrait<CSimulatorMessagesSettings>
            {
                //! \copydoc BlackMisc::TSettingTrait::key
                static const char *key() { return "settingssimulatormessages"; }

                //! \copydoc BlackCore::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("Simulator messages"); return name; }
            };

            //! Selected weather scenario
            struct TSelectedWeatherScenario : public TSettingTrait<Weather::CWeatherScenario>
            {
                //! \copydoc BlackMisc::TSettingTrait::key
                static const char *key() { return "simulator/selectedweatherscenario"; }

                //! \copydoc BlackCore::TSettingTrait::humanReadable
                static const QString &humanReadable() { static const QString name("Weather scenario"); return name; }

                //! \copydoc BlackMisc::TSettingTrait::defaultValue
                static const Weather::CWeatherScenario &defaultValue()
                {
                    static const Weather::CWeatherScenario scenario {};
                    return scenario;
                }
            };
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSimulatorSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::Settings::CSimulatorSettings>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::Settings::CSimulatorSettings>)
Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSimulatorMessagesSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::Settings::CSimulatorMessagesSettings>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::Settings::CSimulatorMessagesSettings>)
Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSimulatorMessagesSettings::TextMessageTypeFlag)
Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSimulatorMessagesSettings::TextMessageType)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::Settings::CSimulatorMessagesSettings::TextMessageType)

#endif // guard
