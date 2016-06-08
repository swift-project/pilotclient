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

#include "blackmisc/settingscache.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
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

                //! Set model directory
                void setModelDirectory(const QString &modelDirectory);

                //! Model directory
                const QString &getModelDirectory() const;

                //! Set exclude directories
                void setModelExcludeDirectories(const QStringList &excludeDirectories);

                //! Margins for given dock widget
                const QStringList &getModelExcludeDirectoryPatterns() const;

                //! Relative exclude directory patterns
                QStringList getRelativeModelExcludeDirectoryPatterns(const QString &modelDirectory) const;

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
                QString     m_modelDirectory;           //!< Model directory
                QStringList m_excludeDirectoryPatterns; //!< Exclude model directory

                BLACK_METACLASS(
                    CSettingsSimulator,
                    BLACK_METAMEMBER(simulatorDirectory),
                    BLACK_METAMEMBER(modelDirectory),
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
                QString getModelDirectoryOrDefault(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Default model path per simulator
                QString getDefaultModelDirectory(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

                //! Model exclude paths per simulator
                QStringList getModelExcludeDirectoryPatternsOrDefault(const BlackMisc::Simulation::CSimulatorInfo &simulator, bool relative) const;

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
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CSettingsSimulator)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::Settings::CSettingsSimulator>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::Settings::CSettingsSimulator>)

#endif // guard
