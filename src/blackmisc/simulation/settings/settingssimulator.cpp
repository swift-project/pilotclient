/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingssimulator.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/stringutils.h"


using namespace BlackMisc;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Simulation::XPlane;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            CSettingsSimulator::CSettingsSimulator()
            { }

            void CSettingsSimulator::setSimulatorDirectory(const QString &simulatorDirectory)
            {
                this->m_simulatorDirectory = simulatorDirectory.trimmed();
            }

            const QString &CSettingsSimulator::getSimulatorDirectory() const
            {
                return this->m_simulatorDirectory;
            }

            void CSettingsSimulator::setModelDirectories(const QStringList &modelDirectories)
            {
                this->m_modelDirectories = modelDirectories;
            }

            void CSettingsSimulator::setModelDirectory(const QString &modelDirectory)
            {
                this->m_modelDirectories = QStringList({ modelDirectory });
            }

            const QStringList &CSettingsSimulator::getModelDirectories() const
            {
                return this->m_modelDirectories;
            }

            void CSettingsSimulator::setModelExcludeDirectories(const QStringList &excludeDirectories)
            {
                this->m_excludeDirectoryPatterns = excludeDirectories;
            }

            const QStringList &CSettingsSimulator::getModelExcludeDirectoryPatterns() const
            {
                return m_excludeDirectoryPatterns;
            }

            void CSettingsSimulator::resetPaths()
            {
                this->m_excludeDirectoryPatterns.clear();
                this->m_modelDirectories.clear();
                this->m_simulatorDirectory.clear();
            }

            QString CSettingsSimulator::convertToQString(bool i18n) const
            {
                return convertToQString(", ", i18n);
            }

            QString CSettingsSimulator::convertToQString(const QString &separator, bool i18n) const
            {
                Q_UNUSED(i18n);
                QString s("model directories: ");
                s.append(this->m_modelDirectories.join(','));
                s.append(separator);
                s.append("exclude directories: ");
                s.append(this->m_excludeDirectoryPatterns.join(','));
                return s;
            }

            CVariant CSettingsSimulator::propertyByIndex(const CPropertyIndex &index) const
            {
                if (index.isMyself()) { return CVariant::from(*this); }
                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexSimulatorDirectory:
                    return CVariant::fromValue(this->m_simulatorDirectory);
                case IndexModelDirectory:
                    return CVariant::fromValue(this->m_modelDirectories);
                case IndexModelExcludeDirectoryPatterns:
                    return CVariant::fromValue(this->m_excludeDirectoryPatterns);
                default:
                    return CValueObject::propertyByIndex(index);
                }
            }

            void CSettingsSimulator::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
            {
                if (index.isMyself()) { (*this) = variant.to<CSettingsSimulator>(); return; }

                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexSimulatorDirectory:
                    this->setSimulatorDirectory(variant.toQString());
                    break;
                case IndexModelDirectory:
                    this->setSimulatorDirectory(variant.toQString());
                    break;
                case IndexModelExcludeDirectoryPatterns:
                    this->m_excludeDirectoryPatterns = variant.value<QStringList>();
                    break;
                default:
                    CValueObject::setPropertyByIndex(index, variant);
                    break;
                }
            }

            CMultiSimulatorSimulatorSettings::CMultiSimulatorSimulatorSettings(QObject *parent) : QObject(parent)
            {
                // void
            }

            CSettingsSimulator CMultiSimulatorSimulatorSettings::getSettings(const CSimulatorInfo &simulator) const
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_simSettingsFs9.get();
                case CSimulatorInfo::FSX: return this->m_simSettingsFsx.get();
                case CSimulatorInfo::P3D: return this->m_simSettingsP3D.get();
                case CSimulatorInfo::XPLANE: return this->m_simSettingsXP.get();
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    break;
                }
                return CSettingsSimulator();
            }

            CStatusMessage CMultiSimulatorSimulatorSettings::setSettings(const CSettingsSimulator &settings, const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_simSettingsFs9.set(settings);
                case CSimulatorInfo::FSX: return this->m_simSettingsFsx.set(settings);
                case CSimulatorInfo::P3D: return this->m_simSettingsP3D.set(settings);
                case CSimulatorInfo::XPLANE: return this->m_simSettingsXP.set(settings);
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    break;
                }
                return CStatusMessage({ CLogCategory::settings() }, CStatusMessage::SeverityError, "wrong simulator");
            }

            CStatusMessage CMultiSimulatorSimulatorSettings::setAndSaveSettings(const CSettingsSimulator &settings, const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_simSettingsFs9.setAndSave(settings);
                case CSimulatorInfo::FSX: return this->m_simSettingsFsx.setAndSave(settings);
                case CSimulatorInfo::P3D: return this->m_simSettingsP3D.setAndSave(settings);
                case CSimulatorInfo::XPLANE: return this->m_simSettingsXP.setAndSave(settings);
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    break;
                }
                return CStatusMessage({ CLogCategory::settings() }, CStatusMessage::SeverityError, "wrong simulator");
            }

            CStatusMessage CMultiSimulatorSimulatorSettings::saveSettings(const CSimulatorInfo &simulator)
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return this->m_simSettingsFs9.save();
                case CSimulatorInfo::FSX: return this->m_simSettingsFsx.save();
                case CSimulatorInfo::P3D: return this->m_simSettingsP3D.save();
                case CSimulatorInfo::XPLANE: return this->m_simSettingsXP.save();
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    break;
                }
                return CStatusMessage({ CLogCategory::settings() }, CStatusMessage::SeverityError, "wrong simulator");
            }

            QString CMultiSimulatorSimulatorSettings::getSimulatorDirectoryOrDefault(const CSimulatorInfo &simulator) const
            {
                const CSettingsSimulator s = this->getSettings(simulator);
                if (s.getSimulatorDirectory().isEmpty())
                {
                    return this->getDefaultSimulatorDirectory(simulator);
                }
                return s.getSimulatorDirectory();
            }

            QString CMultiSimulatorSimulatorSettings::getDefaultSimulatorDirectory(const CSimulatorInfo &simulator) const
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return CFsCommonUtil::fs9Dir();
                case CSimulatorInfo::FSX: return CFsCommonUtil::fsxDir();
                case CSimulatorInfo::P3D: return CFsCommonUtil::p3dDir();
                case CSimulatorInfo::XPLANE: return CXPlaneUtil::xplaneRootDir(); //! check XP
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    break;
                }
                return "";
            }

            QStringList CMultiSimulatorSimulatorSettings::getModelDirectoriesOrDefault(const CSimulatorInfo &simulator) const
            {
                const CSettingsSimulator s = this->getSettings(simulator);
                if (s.getModelDirectories().isEmpty())
                {
                    return this->getDefaultModelDirectories(simulator);
                }
                return s.getModelDirectories();
            }

            QString CMultiSimulatorSimulatorSettings::getFirstModelDirectoryOrDefault(const CSimulatorInfo &simulator) const
            {
                const QStringList models(getModelDirectoriesOrDefault(simulator));
                if (models.isEmpty()) { return ""; }
                return models.first();
            }

            QStringList CMultiSimulatorSimulatorSettings::getDefaultModelDirectories(const CSimulatorInfo &simulator) const
            {
                static const QStringList e;
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return CFsCommonUtil::fs9AircraftDir().isEmpty()   ? e : QStringList({ CFsCommonUtil::fs9AircraftDir() });
                case CSimulatorInfo::FSX: return CFsCommonUtil::fsxSimObjectsDir().isEmpty() ? e : QStringList({ CFsCommonUtil::fsxSimObjectsDir() });
                case CSimulatorInfo::P3D: return CFsCommonUtil::p3dSimObjectsDir().isEmpty() ? e : QStringList({ CFsCommonUtil::p3dSimObjectsDir()});
                case CSimulatorInfo::XPLANE: return CXPlaneUtil::xplaneModelDirectories();
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    break;
                }
                return QStringList();
            }

            QStringList CMultiSimulatorSimulatorSettings::getModelExcludeDirectoryPatternsOrDefault(const CSimulatorInfo &simulator) const
            {
                const CSettingsSimulator s = this->getSettings(simulator);
                QStringList exclude(s.getModelExcludeDirectoryPatterns());
                if (!exclude.isEmpty()) { return exclude; }
                exclude = this->getDefaultModelExcludeDirectoryPatterns(simulator);
                return exclude;
            }

            QStringList CMultiSimulatorSimulatorSettings::getDefaultModelExcludeDirectoryPatterns(const CSimulatorInfo &simulator) const
            {
                Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                switch (simulator.getSimulator())
                {
                case CSimulatorInfo::FS9: return CFsCommonUtil::fs9AircraftObjectsExcludeDirectoryPatterns();
                case CSimulatorInfo::FSX: return CFsCommonUtil::fsxSimObjectsExcludeDirectoryPatterns();
                case CSimulatorInfo::P3D: return CFsCommonUtil::p3dSimObjectsExcludeDirectoryPatterns();
                case CSimulatorInfo::XPLANE: return CXPlaneUtil::xplaneModelExcludeDirectoryPatterns();
                default:
                    Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
                    break;
                }
                return QStringList();
            }

            void CMultiSimulatorSimulatorSettings::resetToDefaults(const CSimulatorInfo &simulator)
            {
                CSettingsSimulator s = this->getSettings(simulator);
                s.resetPaths();
                this->setAndSaveSettings(s, simulator);
            }
        } // ns
    } // ns
} // ns
