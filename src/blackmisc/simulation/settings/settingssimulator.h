/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SETTINGS_SETTINGSSIMULATOR_H
#define BLACKMISC_SIMULATION_SETTINGS_SETTINGSSIMULATOR_H

#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/network/textmessage.h"
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
        class CSimulatedAircraft;

        namespace Settings
        {
            //! Settings for simulator
            //! Driver independent part also used in loaders (such as directories)
            class BLACKMISC_EXPORT CSettingsSimulator :
                public BlackMisc::CValueObject<CSettingsSimulator>
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexSimulatorDirectory = BlackMisc::CPropertyIndex::GlobalIndexCSimulatorSettings,
                    IndexModelDirectory,
                    IndexModelExcludeDirectoryPatterns
                };

                //! Default constructor
                CSettingsSimulator();

                //! Destructor.
                ~CSettingsSimulator() {}

                //! Set simulator directory
                void setSimulatorDirectory(const QString &simulatorDirectory);

                //! Simulator directory
                const QString &getSimulatorDirectory() const;

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
                BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

            private:
                QString     m_simulatorDirectory;       //! Simulator directory
                QStringList m_modelDirectories;         //!< Model directory
                QStringList m_excludeDirectoryPatterns; //!< Exclude model directory

                BLACK_METACLASS(
                    CSettingsSimulator,
                    BLACK_METAMEMBER(simulatorDirectory),
                    BLACK_METAMEMBER(modelDirectories),
                    BLACK_METAMEMBER(excludeDirectoryPatterns)
                );
            };

            //! Trait for simulator settings
            struct SettingsSimulatorFsx : public BlackMisc::CSettingTrait<CSettingsSimulator>
            {
                //! Key in data cache
                static const char *key() { return "settingssimulatorfsx"; }
            };

            //! Trait for simulator settings
            struct SettingsSimulatorFs9 : public BlackMisc::CSettingTrait<CSettingsSimulator>
            {
                //! Key in data cache
                static const char *key() { return "settingssimulatorfs9"; }
            };

            //! Trait for simulator settings
            struct SettingsSimulatorP3D : public BlackMisc::CSettingTrait<CSettingsSimulator>
            {
                //! Key in data cache
                static const char *key() { return "settingssimulatorp3d"; }
            };

            //! Trait for simulator settings
            struct SettingsSimulatorXP : public BlackMisc::CSettingTrait<CSettingsSimulator>
            {
                //! Key in data cache
                static const char *key() { return "settingssimulatorxplane"; }
            };

            //! Bundle of settings for all simulators
            class BLACKMISC_EXPORT CMultiSimulatorSimulatorSettings : public QObject
            {
                Q_OBJECT

            public:
                //! Construtor
                CMultiSimulatorSimulatorSettings(QObject *parent = nullptr);

                //! Settings per simulator
                CSettingsSimulator getSettings(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Set settings per simulator
                BlackMisc::CStatusMessage setSettings(const BlackMisc::Simulation::Settings::CSettingsSimulator &settings, const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Set settings per simulator
                BlackMisc::CStatusMessage setAndSaveSettings(const BlackMisc::Simulation::Settings::CSettingsSimulator &settings, const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Set settings per simulator
                BlackMisc::CStatusMessage saveSettings(const BlackMisc::Simulation::CSimulatorInfo &simulator);

                //! Simulator directory or default model path per simulator
                QString getSimulatorDirectoryOrDefault(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Default simulator path per simulator
                QString getDefaultSimulatorDirectory(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Model directory or default model path per simulator
                QStringList getModelDirectoriesOrDefault(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! First model directoy
                QString getFirstModelDirectoryOrDefault(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Default model path per simulator
                QStringList getDefaultModelDirectories(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Model exclude paths per simulator
                QStringList getModelExcludeDirectoryPatternsOrDefault(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Default model exclude paths per simulator
                QStringList getDefaultModelExcludeDirectoryPatterns(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Reset to defaults
                void resetToDefaults(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            private:
                BlackMisc::CSetting<BlackMisc::Simulation::Settings::SettingsSimulatorFsx> m_simSettingsFsx {this}; //!< FSX cache
                BlackMisc::CSetting<BlackMisc::Simulation::Settings::SettingsSimulatorFs9> m_simSettingsFs9 {this}; //!< FS9 cache
                BlackMisc::CSetting<BlackMisc::Simulation::Settings::SettingsSimulatorP3D> m_simSettingsP3D {this}; //!< P3D cache
                BlackMisc::CSetting<BlackMisc::Simulation::Settings::SettingsSimulatorXP>  m_simSettingsXP  {this}; //!< XP cache
            };

            //! Settings regarding message handling.
            //! Driver independent part, related to network
            class BLACKMISC_EXPORT CSettingsSimulatorMessages :
                public BlackMisc::CValueObject<CSettingsSimulatorMessages>
            {
            public:
                //! Properties by index
                enum ColumnIndex
                {
                    IndexTechnicalLogSeverity = BlackMisc::CPropertyIndex::GlobalIndexCSimulatorMessageSettings,
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
                    TextMessagesAll  = TextMessagesUnicom | TextMessagesCom1 | TextMessagesCom2 | TextMessagePrivate
                };
                Q_DECLARE_FLAGS(TextMessageType, TextMessageTypeFlag)

                //! Default constructor
                CSettingsSimulatorMessages();

                //! Destructor.
                ~CSettingsSimulatorMessages() {}

                //! Log severity
                void setTechnicalLogSeverity(BlackMisc::CStatusMessage::StatusSeverity severity);

                //! Globally enable / disable
                void setGloballyEnabled(bool enabled) { this->m_globallyEnabled = enabled; }

                //! Globally enabled?
                bool isGloballyEnabled() const { return this->m_globallyEnabled; }

                //! No technical messages
                void disableTechnicalMessages();

                //! Relay (technical) error messages
                bool isRelayedErrorsMessages() const;

                //! Relay (technical) warning messages
                bool isRelayedWarningMessages() const;

                //! Relay (technical) info messages
                bool isRelayedInfoMessages() const;

                //! Relay any message
                bool isRelayedTechnicalMessages() const;

                //! Relay the following message types
                void setRelayedTextMessages(BlackMisc::Simulation::Settings::CSettingsSimulatorMessages::TextMessageType messageType);

                //! Relay supervisor messages
                bool isRelayedSupervisorTextMessages() const;

                //! Relay private messages
                bool isRelayedPrivateTextMessages() const;

                //! Relay UNICOM messages
                bool isRelayedUnicomTextMessages() const;

                //! Relay given text message
                bool isRelayedTextMessage(const BlackMisc::Network::CTextMessage &msg, const BlackMisc::Simulation::CSimulatedAircraft &aircraft) const;

                //! Relay COM1 text message
                bool isRelayedCom1TextMessages() const;

                //! Relay COM2 text message
                bool isRelayedCom2TextMessages() const;

                //! Relayed text messages
                CSettingsSimulatorMessages::TextMessageType getRelayedTextMessageTypes() const;

                //! \copydoc BlackMisc::Mixin::String::toQString
                QString convertToQString(bool i18n = false) const;

                //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
                BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

                //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
                void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

            private:
                int m_technicalLogLevel = BlackMisc::CStatusMessage::SeverityError; //!< Simulator directory
                int m_messageType = static_cast<int>(TextMessagePrivate | TextMessageSupervisor);
                bool m_globallyEnabled = true; //!< messsage relay enabled to simulator

                BLACK_METACLASS(
                    CSettingsSimulatorMessages,
                    BLACK_METAMEMBER(technicalLogLevel),
                    BLACK_METAMEMBER(messageType)
                );
            };

            //! Trait for simulator message settings
            struct SettingsSimulatorMessages : public BlackMisc::CSettingTrait<CSettingsSimulatorMessages>
            {
                //! Key in data cache
                static const char *key() { return "settingssimulatormessages"; }
            };
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSettingsSimulator)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::Settings::CSettingsSimulator>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::Settings::CSettingsSimulator>)
Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSettingsSimulatorMessages)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::Settings::CSettingsSimulatorMessages>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::Settings::CSettingsSimulatorMessages>)
Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSettingsSimulatorMessages::TextMessageTypeFlag)
Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSettingsSimulatorMessages::TextMessageType)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::Settings::CSettingsSimulatorMessages::TextMessageType)

#endif // guard
