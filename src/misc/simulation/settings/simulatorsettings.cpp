// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/settings/simulatorsettings.h"

#include <QStringBuilder>

#include "config/buildconfig.h"
#include "misc/simulation/flightgear/flightgearutil.h"
#include "misc/simulation/fscommon/fsdirectories.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/xplane/xplaneutil.h"
#include "misc/stringutils.h"
#include "misc/verify.h"

using namespace swift::config;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation::fscommon;
using namespace swift::misc::simulation::xplane;
using namespace swift::misc::simulation::flightgear;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation::settings, CSimulatorSettings)
SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation::settings, CFsxP3DSettings)
SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation::settings, CSimulatorMessagesSettings)

namespace swift::misc::simulation::settings
{
    const QString &CSimulatorSettings::cgSourceAsString(CSimulatorSettings::CGSource source)
    {
        static const QString sf("simulator 1st");
        static const QString dbf("DB 1st");
        static const QString so("simulator only");
        static const QString dbo("DB only");

        switch (source)
        {
        case CGFromSimulatorFirst: return sf;
        case CGFromDBFirst: return dbf;
        case CGFromSimulatorOnly: return so;
        case CGFromDBOnly: return dbo;
        default: break;
        }
        static const QString u("unknown");
        return u;
    }

    CSimulatorSettings::CSimulatorSettings() {}

    void CSimulatorSettings::setSimulatorDirectory(const QString &simulatorDirectory)
    {
        m_simulatorDirectory = simulatorDirectory.trimmed();
    }

    const QString &CSimulatorSettings::getSimulatorDirectory() const { return m_simulatorDirectory; }

    void CSimulatorSettings::setModelDirectories(const QStringList &modelDirectories)
    {
        m_modelDirectories = modelDirectories;
        m_modelDirectories.removeAll({}); // empty values
        m_modelDirectories.removeDuplicates(); // duplicates
    }

    void CSimulatorSettings::clearModelDirectories() { m_modelDirectories.clear(); }

    void CSimulatorSettings::setModelDirectory(const QString &modelDirectory)
    {
        m_modelDirectories = QStringList({ modelDirectory });
    }

    bool CSimulatorSettings::addModelDirectory(const QString &modelDirectory)
    {
        const Qt::CaseSensitivity cs =
            CBuildConfig::isRunningOnWindowsNtPlatform() ? Qt::CaseInsensitive : Qt::CaseSensitive;
        if (m_modelDirectories.contains(modelDirectory, cs)) { return false; }
        m_modelDirectories.push_back(modelDirectory);
        m_modelDirectories.removeAll({});
        m_modelDirectories.removeDuplicates();
        return true;
    }

    const QStringList &CSimulatorSettings::getModelDirectories() const { return m_modelDirectories; }

    void CSimulatorSettings::setModelExcludeDirectories(const QStringList &excludeDirectories)
    {
        m_excludeDirectoryPatterns = excludeDirectories;
        m_excludeDirectoryPatterns.removeAll({});
        m_excludeDirectoryPatterns.removeDuplicates();
    }

    const QStringList &CSimulatorSettings::getModelExcludeDirectoryPatterns() const
    {
        return m_excludeDirectoryPatterns;
    }

    bool CSimulatorSettings::setComIntegrated(bool integrated)
    {
        if (integrated == m_comIntegration) { return false; }
        m_comIntegration = integrated;
        return true;
    }

    bool CSimulatorSettings::setCGSource(CSimulatorSettings::CGSource source)
    {
        const int s = static_cast<int>(source);
        if (m_cgSource == s) { return false; }
        m_cgSource = s;
        return true;
    }

    bool CSimulatorSettings::setRecordOwnAircraftGnd(bool record)
    {
        if (record == m_recordGnd) { return false; }
        m_recordGnd = record;
        return true;
    }

    bool CSimulatorSettings::setRecordedGndRadius(const CLength &radius)
    {
        if (radius == m_recordedGndRadius) { return false; }
        m_recordedGndRadius = radius;
        return true;
    }

    void CSimulatorSettings::resetPaths()
    {
        m_excludeDirectoryPatterns.clear();
        m_modelDirectories.clear();
        m_simulatorDirectory.clear();
    }

    QString CSimulatorSettings::convertToQString(bool i18n) const { return convertToQString(", ", i18n); }

    QString CSimulatorSettings::convertToQString(const QString &separator, bool i18n) const
    {
        Q_UNUSED(i18n)
        return u"model directories: " % m_modelDirectories.join(',') % separator % u"exclude directories: " %
               m_excludeDirectoryPatterns.join(',');
    }

    QVariant CSimulatorSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexSimulatorDirectory: return QVariant::fromValue(m_simulatorDirectory);
        case IndexModelDirectories: return QVariant::fromValue(m_modelDirectories);
        case IndexModelExcludeDirectoryPatterns: return QVariant::fromValue(m_excludeDirectoryPatterns);
        case IndexComIntegration: return QVariant::fromValue(m_comIntegration);
        case IndexRecordOwnAircraftGnd: return QVariant::fromValue(m_recordGnd);
        case IndexCGSource: return QVariant::fromValue(m_cgSource);
        case IndexRecordOwnAircraftGndRadius: return m_recordedGndRadius.propertyByIndex(index.copyFrontRemoved());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CSimulatorSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CSimulatorSettings>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexSimulatorDirectory: this->setSimulatorDirectory(variant.toString()); break;
        case IndexModelDirectories: this->setSimulatorDirectory(variant.toString()); break;
        case IndexComIntegration: this->setComIntegrated(variant.toBool()); break;
        case IndexRecordOwnAircraftGnd: this->setRecordOwnAircraftGnd(variant.toBool()); break;
        case IndexModelExcludeDirectoryPatterns: m_excludeDirectoryPatterns = variant.value<QStringList>(); break;
        case IndexCGSource: m_cgSource = variant.toInt(); break;
        case IndexRecordOwnAircraftGndRadius:
            m_recordedGndRadius.setPropertyByIndex(index.copyFrontRemoved(), variant);
            break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    CMultiSimulatorSettings::CMultiSimulatorSettings(QObject *parent) : QObject(parent)
    {
        // void
    }

    CSimulatorSettings CMultiSimulatorSettings::getSettings(const CSimulatorInfo &simulator) const
    {
        if (!simulator.isSingleSimulator())
        {
            // mostly happening with emulated driver, VERIFY for better debugging
            SWIFT_VERIFY_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
            return CSimulatorSettings();
        }
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FG: return m_simSettingsFG.get();
        case CSimulatorInfo::FS9: return m_simSettingsFs9.get();
        case CSimulatorInfo::FSX: return m_simSettingsFsx.get();
        case CSimulatorInfo::P3D: return m_simSettingsP3D.get();
        case CSimulatorInfo::MSFS: return m_simSettingsMsfs.get();
        case CSimulatorInfo::XPLANE: return m_simSettingsXP.get();

        default: Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator"); break;
        }
        return CSimulatorSettings();
    }

    CSpecializedSimulatorSettings CMultiSimulatorSettings::getSpecializedSettings(const CSimulatorInfo &simulator) const
    {
        return CSpecializedSimulatorSettings(this->getSettings(simulator), simulator);
    }

    CStatusMessage CMultiSimulatorSettings::setSettings(const CSimulatorSettings &settings,
                                                        const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FG: return m_simSettingsFG.set(settings);
        case CSimulatorInfo::FS9: return m_simSettingsFs9.set(settings);
        case CSimulatorInfo::FSX: return m_simSettingsFsx.set(settings);
        case CSimulatorInfo::P3D: return m_simSettingsP3D.set(settings);
        case CSimulatorInfo::MSFS: return m_simSettingsMsfs.set(settings);
        case CSimulatorInfo::XPLANE: return m_simSettingsXP.set(settings);
        default: Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator"); break;
        }
        return CStatusMessage({ CLogCategories::settings() }, CStatusMessage::SeverityError, u"wrong simulator");
    }

    CStatusMessage CMultiSimulatorSettings::addModelDirectory(const QString &modelDirectory,
                                                              const CSimulatorInfo &simulator)
    {
        CSimulatorSettings s = this->getSettings(simulator);
        if (!s.addModelDirectory(modelDirectory))
        {
            return CStatusMessage({ CLogCategories::settings() }, CStatusMessage::SeverityInfo,
                                  u"directory already existing");
        }
        return this->setSettings(s, simulator);
    }

    CStatusMessage CMultiSimulatorSettings::clearModelDirectories(const CSimulatorInfo &simulator)
    {
        CSimulatorSettings s = this->getSettings(simulator);
        s.clearModelDirectories();
        return this->setSettings(s, simulator);
    }

    CStatusMessageList CMultiSimulatorSettings::setAndValidateSettings(const CSimulatorSettings &settings,
                                                                       const CSimulatorInfo &simulator)
    {
        this->setSettings(settings, simulator);
        CStatusMessageList msgs;
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FG: break;
        case CSimulatorInfo::FS9: break;
        case CSimulatorInfo::FSX: break;
        case CSimulatorInfo::P3D: break;
        case CSimulatorInfo::MSFS: break;
        case CSimulatorInfo::XPLANE:
        {
            if (settings.hasModelDirectories())
            {
                const QString simDir = this->getSimulatorDirectoryOrDefault(simulator);
                msgs = CXPlaneUtil::validateModelDirectories(simDir, settings.getModelDirectories());
            }
        }
        break;
        default: Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator"); break;
        }
        return msgs;
    }

    CStatusMessage CMultiSimulatorSettings::setAndSaveSettings(const CSimulatorSettings &settings,
                                                               const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FG: return m_simSettingsFG.setAndSave(settings);
        case CSimulatorInfo::FS9: return m_simSettingsFs9.setAndSave(settings);
        case CSimulatorInfo::FSX: return m_simSettingsFsx.setAndSave(settings);
        case CSimulatorInfo::P3D: return m_simSettingsP3D.setAndSave(settings);
        case CSimulatorInfo::MSFS: return m_simSettingsMsfs.setAndSave(settings);
        case CSimulatorInfo::XPLANE: return m_simSettingsXP.setAndSave(settings);
        default: Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator"); break;
        }
        return CStatusMessage({ CLogCategories::settings() }, CStatusMessage::SeverityError, u"wrong simulator");
    }

    CStatusMessage CMultiSimulatorSettings::setAndSaveSettings(const CSpecializedSimulatorSettings &settings,
                                                               const CSimulatorInfo &simulator)
    {
        return this->setAndSaveSettings(settings.getGenericSettings(), simulator);
    }

    CStatusMessage CMultiSimulatorSettings::saveSettings(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FG: return m_simSettingsFG.save();
        case CSimulatorInfo::FS9: return m_simSettingsFs9.save();
        case CSimulatorInfo::FSX: return m_simSettingsFsx.save();
        case CSimulatorInfo::P3D: return m_simSettingsP3D.save();
        case CSimulatorInfo::MSFS: return m_simSettingsMsfs.save();
        case CSimulatorInfo::XPLANE: return m_simSettingsXP.save();
        default: Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator"); break;
        }
        return CStatusMessage({ CLogCategories::settings() }, CStatusMessage::SeverityError, u"wrong simulator");
    }

    QString CMultiSimulatorSettings::getSimulatorDirectoryIfNotDefault(const CSimulatorInfo &simulator) const
    {
        const CSpecializedSimulatorSettings s = this->getSpecializedSettings(simulator);
        return s.getSimulatorDirectoryIfNotDefault();
    }

    QString CMultiSimulatorSettings::getSimulatorDirectoryOrDefault(const CSimulatorInfo &simulator) const
    {
        const CSpecializedSimulatorSettings s = this->getSpecializedSettings(simulator);
        return s.getSimulatorDirectoryOrDefault();
    }

    QStringList CMultiSimulatorSettings::getModelDirectoriesIfNotDefault(const CSimulatorInfo &simulator) const
    {
        const CSpecializedSimulatorSettings s = this->getSpecializedSettings(simulator);
        return s.getModelDirectoriesIfNotDefault();
    }

    QStringList CMultiSimulatorSettings::getModelDirectoriesOrDefault(const CSimulatorInfo &simulator) const
    {
        const CSpecializedSimulatorSettings s = this->getSpecializedSettings(simulator);
        return s.getModelDirectoriesOrDefault();
    }

    QString CMultiSimulatorSettings::getFirstModelDirectoryOrDefault(const CSimulatorInfo &simulator) const
    {
        const CSpecializedSimulatorSettings s = this->getSpecializedSettings(simulator);
        return s.getFirstModelDirectoryOrDefault();
    }

    QStringList
    CMultiSimulatorSettings::getModelExcludeDirectoryPatternsIfNotDefault(const CSimulatorInfo &simulator) const
    {
        const CSpecializedSimulatorSettings s = this->getSpecializedSettings(simulator);
        return s.getModelExcludeDirectoryPatternsIfNotDefault();
    }

    QStringList
    CMultiSimulatorSettings::getModelExcludeDirectoryPatternsOrDefault(const CSimulatorInfo &simulator) const
    {
        const CSpecializedSimulatorSettings s = this->getSpecializedSettings(simulator);
        return s.getModelExcludeDirectoryPatternsOrDefault();
    }

    void CMultiSimulatorSettings::resetToDefaults(const CSimulatorInfo &simulator)
    {
        CSimulatorSettings s = this->getSettings(simulator);
        s.resetPaths();
        this->setAndSaveSettings(s, simulator);
    }

    const QStringList &CMultiSimulatorSettings::defaultModelDirectories(const CSimulatorInfo &simulator) const
    {
        return CSpecializedSimulatorSettings::defaultModelDirectories(simulator);
    }

    void CMultiSimulatorSettings::onFsxSettingsChanged() { this->emitSettingsChanged(CSimulatorInfo::fsx()); }

    void CMultiSimulatorSettings::onP3DSettingsChanged() { this->emitSettingsChanged(CSimulatorInfo::p3d()); }

    void CMultiSimulatorSettings::onFs9SettingsChanged() { this->emitSettingsChanged(CSimulatorInfo::fs9()); }

    void CMultiSimulatorSettings::onMsfsSettingsChanged() { this->emitSettingsChanged(CSimulatorInfo::msfs()); }

    void CMultiSimulatorSettings::onXPSettingsChanged() { this->emitSettingsChanged(CSimulatorInfo::xplane()); }

    void CMultiSimulatorSettings::onFGSettingsChanged() { this->emitSettingsChanged(CSimulatorInfo::fg()); }

    void CMultiSimulatorSettings::emitSettingsChanged(const CSimulatorInfo &simInfo)
    {
        emit this->settingsChanged(simInfo);
    }

    void CSimulatorMessagesSettings::setTechnicalLogSeverity(CStatusMessage::StatusSeverity severity)
    {
        m_technicalLogLevel = static_cast<int>(severity);
    }

    void CSimulatorMessagesSettings::disableTechnicalMessages() { m_technicalLogLevel = -1; }

    bool CSimulatorMessagesSettings::isRelayErrorsMessages() const
    {
        if (m_technicalLogLevel < 0) { return false; }
        return (m_technicalLogLevel <= CStatusMessage::SeverityError);
    }

    bool CSimulatorMessagesSettings::isRelayWarningMessages() const
    {
        if (m_technicalLogLevel < 0) { return false; }
        return (m_technicalLogLevel <= CStatusMessage::SeverityWarning);
    }

    bool CSimulatorMessagesSettings::isRelayInfoMessages() const
    {
        if (m_technicalLogLevel < 0) { return false; }
        return (m_technicalLogLevel <= CStatusMessage::SeverityInfo);
    }

    bool CSimulatorMessagesSettings::isRelayTechnicalMessages() const { return (m_technicalLogLevel >= 0); }

    void CSimulatorMessagesSettings::setRelayTextMessages(CSimulatorMessagesSettings::TextMessageType messageType)
    {
        m_messageType = static_cast<int>(messageType);
    }

    bool CSimulatorMessagesSettings::isRelaySupervisorTextMessages() const
    {
        return this->getRelayedTextMessageTypes().testFlag(TextMessageSupervisor);
    }

    bool CSimulatorMessagesSettings::isRelayPrivateTextMessages() const
    {
        return this->getRelayedTextMessageTypes().testFlag(TextMessagePrivate);
    }

    bool CSimulatorMessagesSettings::isRelayUnicomTextMessages() const
    {
        return this->getRelayedTextMessageTypes().testFlag(TextMessagesUnicom);
    }

    bool CSimulatorMessagesSettings::isRelayCom1TextMessages() const
    {
        return this->getRelayedTextMessageTypes().testFlag(TextMessagesCom1);
    }

    bool CSimulatorMessagesSettings::isRelayCom2TextMessages() const
    {
        return this->getRelayedTextMessageTypes().testFlag(TextMessagesCom2);
    }

    bool CSimulatorMessagesSettings::relayThisStatusMessage(const CStatusMessage &message) const
    {
        if (message.isEmpty()) { return false; }
        if (!this->isRelayGloballyEnabled()) { return false; }
        if (!this->isRelayTechnicalMessages()) { return false; }
        const int s = static_cast<int>(message.getSeverity());
        return (s >= m_technicalLogLevel);
    }

    bool
    CSimulatorMessagesSettings::relayThisTextMessage(const network::CTextMessage &msg,
                                                     const swift::misc::simulation::CSimulatedAircraft &aircraft) const
    {
        if (msg.isEmpty()) { return false; }
        if (!this->isRelayGloballyEnabled()) { return false; }
        if (m_messageType == NoTextMessages) { return false; }

        const TextMessageType mt = static_cast<TextMessageType>(m_messageType);
        if (msg.isPrivateMessage() && mt.testFlag(TextMessagePrivate)) { return true; }
        if (msg.isSupervisorMessage() && (mt.testFlag(TextMessagePrivate) || mt.testFlag(TextMessageSupervisor)))
        {
            return true;
        }
        if (msg.isSendToUnicom() && mt.testFlag(TextMessagesUnicom)) { return true; }

        if (msg.isRadioMessage())
        {
            const CFrequency f(msg.getFrequency());
            if (mt.testFlag(TextMessagesCom1))
            {
                if (aircraft.getCom1System().isActiveFrequencySameFrequency(f)) { return true; }
            }
            if (mt.testFlag(TextMessagesCom2))
            {
                if (aircraft.getCom2System().isActiveFrequencySameFrequency(f)) { return true; }
            }
        }
        return false;
    }

    CSimulatorMessagesSettings::TextMessageType CSimulatorMessagesSettings::getRelayedTextMessageTypes() const
    {
        return static_cast<CSimulatorMessagesSettings::TextMessageType>(m_messageType);
    }

    QString CSimulatorMessagesSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        static const QString s("Enabled %1, text messages: %2, severity: %3");
        QString severity;
        if (this->isRelayTechnicalMessages()) { severity = QStringLiteral("No tech. msgs"); }
        else
        {
            severity =
                CStatusMessage::severityToString(static_cast<CStatusMessage::StatusSeverity>(m_technicalLogLevel));
        }
        return s.arg(boolToOnOff(m_relayGloballyEnabled)).arg(m_messageType).arg(severity);
    }

    QVariant CSimulatorMessagesSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexTechnicalLogSeverity: return QVariant::fromValue(m_technicalLogLevel);
        case IndexRelayTextMessage: return QVariant::fromValue(m_messageType);
        case IndexRelayGloballyEnabled: return QVariant::fromValue(m_relayGloballyEnabled);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CSimulatorMessagesSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CSimulatorMessagesSettings>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexTechnicalLogSeverity:
            this->setTechnicalLogSeverity(static_cast<CStatusMessage::StatusSeverity>(variant.toInt()));
            break;
        case IndexRelayTextMessage:
            this->setRelayTextMessages(static_cast<CSimulatorMessagesSettings::TextMessageType>(variant.toInt()));
            break;
        case IndexRelayGloballyEnabled: this->setRelayGloballyEnabled(variant.toBool()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    const QString &CSpecializedSimulatorSettings::getDefaultSimulatorDirectory() const
    {
        return CSpecializedSimulatorSettings::defaultSimulatorDirectory(m_simulator);
    }

    const QString &CSpecializedSimulatorSettings::getSimulatorDirectoryIfNotDefault() const
    {
        static const QString empty;
        if (!m_genericSettings.hasSimulatorDirectory()) { return empty; }
        if (m_genericSettings.getSimulatorDirectory() ==
            CSpecializedSimulatorSettings::defaultSimulatorDirectory(m_simulator))
        {
            return empty;
        }
        return m_genericSettings.getSimulatorDirectory();
    }

    const QString &CSpecializedSimulatorSettings::getSimulatorDirectoryOrDefault() const
    {
        return (m_genericSettings.hasSimulatorDirectory()) ?
                   m_genericSettings.getSimulatorDirectory() :
                   CSpecializedSimulatorSettings::defaultSimulatorDirectory(m_simulator);
    }

    QStringList CSpecializedSimulatorSettings::getModelDirectoriesOrDefault() const
    {
        return m_genericSettings.hasModelDirectories() ? m_genericSettings.getModelDirectories() :
                                                         this->getModelDirectoriesFromSimulatorDirectoryOrDefault();
    }

    QStringList CSpecializedSimulatorSettings::getModelDirectoriesFromSimulatorDirectoy() const
    {
        if (!m_genericSettings.hasSimulatorDirectory()) { return QStringList(); }
        const QString s(m_genericSettings.getSimulatorDirectory());
        QStringList dirs;
        switch (m_simulator.getSimulator())
        {
        case CSimulatorInfo::FG: dirs = QStringList(CFlightgearUtil::modelDirectoriesFromSimDir(s)); break;
        case CSimulatorInfo::FS9: dirs = QStringList({ CFsDirectories::fs9AircraftDirFromSimDir(s) }); break;
        case CSimulatorInfo::FSX:
            dirs = CFsDirectories::fsxSimObjectsDirPlusAddOnXmlSimObjectsPaths(
                CFsDirectories::fsxSimObjectsDirFromSimDir(s));
            break;
        case CSimulatorInfo::P3D:
        {
            const QString versionHint = CFsDirectories::guessP3DVersion(s);
            dirs = CFsDirectories::p3dSimObjectsDirPlusAddOnXmlSimObjectsPaths(
                CFsDirectories::p3dSimObjectsDirFromSimDir(s), versionHint);
        }
        break;
        case CSimulatorInfo::XPLANE: dirs = QStringList({ CXPlaneUtil::modelDirectoriesFromSimDir(s) }); break;
        default: break;
        }

        dirs.removeAll({}); // remove empty
        dirs.removeDuplicates();
        return dirs;
    }

    QStringList CSpecializedSimulatorSettings::getModelDirectoriesFromSimulatorDirectoryOrDefault() const
    {
        if (!m_genericSettings.hasSimulatorDirectory())
        {
            return CSpecializedSimulatorSettings::defaultModelDirectories(m_simulator);
        }
        return this->getModelDirectoriesFromSimulatorDirectoy();
    }

    const QStringList &CSpecializedSimulatorSettings::getModelDirectoriesIfNotDefault() const
    {
        static const QStringList empty;
        if (!m_genericSettings.hasModelDirectories()) { return empty; }
        const QStringList defaultDirectories = CSpecializedSimulatorSettings::defaultModelDirectories(m_simulator);
        if (CFileUtils::sameDirectories(m_genericSettings.getModelDirectories(), defaultDirectories)) { return empty; }
        return m_genericSettings.getModelDirectories();
    }

    const QStringList &CSpecializedSimulatorSettings::getDefaultModelExcludeDirectoryPatterns() const
    {
        return CSpecializedSimulatorSettings::defaultModelExcludeDirectoryPatterns(m_simulator);
    }

    QString CSpecializedSimulatorSettings::getFirstModelDirectoryOrDefault() const
    {
        static const QString empty;
        if (this->getModelDirectoriesOrDefault().isEmpty()) { return empty; }
        return this->getModelDirectoriesOrDefault().constFirst();
    }

    const QStringList &CSpecializedSimulatorSettings::getDefaultModelDirectories() const
    {
        return CSpecializedSimulatorSettings::defaultModelDirectories(m_simulator);
    }

    const QStringList &CSpecializedSimulatorSettings::getModelExcludeDirectoryPatternsIfNotDefault() const
    {
        static const QStringList empty;
        if (!m_genericSettings.hasModelExcludeDirectoryPatterns()) { return empty; }
        if (m_genericSettings.getModelExcludeDirectoryPatterns() ==
            CSpecializedSimulatorSettings::defaultModelExcludeDirectoryPatterns(m_simulator))
        {
            return empty;
        }
        return m_genericSettings.getModelExcludeDirectoryPatterns();
    }

    const QStringList &CSpecializedSimulatorSettings::getModelExcludeDirectoryPatternsOrDefault() const
    {
        return m_genericSettings.hasModelExcludeDirectoryPatterns() ?
                   m_genericSettings.getModelExcludeDirectoryPatterns() :
                   CSpecializedSimulatorSettings::defaultModelExcludeDirectoryPatterns(m_simulator);
    }

    const QStringList &CSpecializedSimulatorSettings::defaultModelDirectories(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
        static const QStringList e;
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FG:
        {
            return CFlightgearUtil::flightgearModelDirectories();
        }
        case CSimulatorInfo::FS9:
        {
            if (CFsDirectories::fs9AircraftDir().isEmpty()) { return e; }
            static const QStringList md({ CFsDirectories::fs9AircraftDir() });
            return md;
        }
        case CSimulatorInfo::FSX:
        {
            if (CFsDirectories::fsxSimObjectsDir().isEmpty()) { return e; }
            static const QStringList md = CFsDirectories::fsxSimObjectsDirPlusAddOnXmlSimObjectsPaths();
            return md;
        }
        case CSimulatorInfo::P3D:
        {
            static const QString p3d = CFsDirectories::p3dSimObjectsDir();
            if (p3d.isEmpty()) { return e; }
            static const QString versionHint = CFsDirectories::guessP3DVersion(p3d);
            static const QStringList md = CFsDirectories::p3dSimObjectsDirPlusAddOnXmlSimObjectsPaths(p3d, versionHint);
            return md;
        }
        case CSimulatorInfo::MSFS:
        {
            static const QString msfs = CFsDirectories::msfsPackagesDir();
            if (msfs.isEmpty()) { return e; }
            static const QStringList md { msfs };
            return md;
        }
        case CSimulatorInfo::XPLANE:
        {
            return CXPlaneUtil::xplaneModelDirectories();
        }
        default: Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator"); break;
        }
        return e;
    }

    const QString &CSpecializedSimulatorSettings::defaultSimulatorDirectory(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
        static const QString empty;
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FG: return CFlightgearUtil::flightgearRootDir();
        case CSimulatorInfo::FS9: return CFsDirectories::fs9Dir();
        case CSimulatorInfo::FSX: return CFsDirectories::fsxDir();
        case CSimulatorInfo::P3D: return CFsDirectories::p3dDir();
        case CSimulatorInfo::MSFS: return CFsDirectories::msfsDir();
        case CSimulatorInfo::XPLANE: return CXPlaneUtil::xplaneRootDir();
        default: Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator"); break;
        }
        return empty;
    }

    const QStringList &
    CSpecializedSimulatorSettings::defaultModelExcludeDirectoryPatterns(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator");
        static const QStringList empty;
        switch (simulator.getSimulator())
        {
        case CSimulatorInfo::FG: return CFlightgearUtil::flightgearModelExcludeDirectoryPatterns();
        case CSimulatorInfo::FS9: return CFsDirectories::fs9AircraftObjectsExcludeDirectoryPatterns();
        case CSimulatorInfo::FSX: return CFsDirectories::fsxSimObjectsExcludeDirectoryPatterns();
        case CSimulatorInfo::P3D: return CFsDirectories::p3dSimObjectsExcludeDirectoryPatterns();
        case CSimulatorInfo::XPLANE: return CXPlaneUtil::xplaneModelExcludeDirectoryPatterns();
        case CSimulatorInfo::MSFS: return CFsDirectories::msfs20SimObjectsExcludeDirectoryPatterns();
        default: Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "No single simulator"); break;
        }
        return empty;
    }

    QString CXPlaneSimulatorSettings::getPluginDirOrDefault() const
    {
        return CFileUtils::appendFilePathsAndFixUnc(this->getSimulatorDirectoryOrDefault(),
                                                    CXPlaneUtil::xplanePluginPathName());
    }

    QString CFsxP3DSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        return u"SimulatedObject: " % boolToYesNo(m_useSimulatedObjectAdding) % u" SB offsets: " %
               boolToYesNo(m_useSbOffsets);
    }

    QVariant CFsxP3DSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        Q_UNUSED(index)
        return {};
    }

    void CFsxP3DSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        Q_UNUSED(index)
        Q_UNUSED(variant)
    }

    CFsxP3DSettings CMultiSimulatorDetailsSettings::getSettings(const CSimulatorInfo &sim) const
    {
        Q_ASSERT_X(sim.isFsxP3DFamily(), Q_FUNC_INFO, "Only for FSX/P3D");
        if (sim == CSimulatorInfo::p3d()) { return m_simP3D.get(); }
        return m_simFsx.get();
    }

    CStatusMessage CMultiSimulatorDetailsSettings::setSettings(const CFsxP3DSettings &settings,
                                                               const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isFsxP3DFamily(), Q_FUNC_INFO, "Only for FSX/P3D");
        if (simulator == CSimulatorInfo::p3d()) { return m_simP3D.set(settings); }
        return m_simFsx.set(settings);
    }

    CStatusMessage CMultiSimulatorDetailsSettings::setAndSaveSettings(const CFsxP3DSettings &settings,
                                                                      const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isFsxP3DFamily(), Q_FUNC_INFO, "Only for FSX/P3D");
        if (simulator == CSimulatorInfo::p3d()) { return m_simP3D.setAndSave(settings); }
        return m_simFsx.setAndSave(settings);
    }

    bool TSimulatorXP::isValid(const CSimulatorSettings &value, QString &reason)
    {
        const QString simDir = value.hasSimulatorDirectory() ?
                                   value.getSimulatorDirectory() :
                                   CSpecializedSimulatorSettings::defaultSimulatorDirectory(CSimulatorInfo::XPLANE);
        const CStatusMessageList msgs = CXPlaneUtil::validateModelDirectories(simDir, value.getModelDirectories());
        if (msgs.isSuccess()) { return true; }
        reason = msgs.getErrorMessages().toSingleMessage().toQString(true);
        return false;
    }

} // namespace swift::misc::simulation::settings
