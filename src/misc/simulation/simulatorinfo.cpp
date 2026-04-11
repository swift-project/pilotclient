// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/simulatorinfo.h"

#include <algorithm>

#include <QJsonValue>
#include <QStringBuilder>
#include <QtGlobal>

#include "config/buildconfig.h"
#include "misc/comparefunctions.h"
#include "misc/db/datastoreutility.h"
#include "misc/iconlist.h"
#include "misc/simulation/fscommon/fsdirectories.h"
#include "misc/simulation/xplane/xplaneutil.h"
#include "misc/stringutils.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::db;
using namespace swift::misc::simulation::fscommon;
using namespace swift::misc::simulation::xplane;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation, CSimulatorInfo)

namespace swift::misc::simulation
{
    CSimulatorInfo::CSimulatorInfo(const QString &identifierString)
        : m_simulator(identifierToSimulator(identifierString))
    {}

    CSimulatorInfo::CSimulatorInfo(const QStringList &simulators)
    {
        const QString identifier = simulators.join(' ');
        m_simulator = identifierToSimulator(identifier);
    }

    CSimulatorInfo::CSimulatorInfo(Simulator simulator) : m_simulator(static_cast<int>(simulator)) {}

    CSimulatorInfo::CSimulatorInfo(bool xp, bool p3d, bool fg, bool msfs, bool msfs2024)
        : m_simulator(boolToFlag(xp, p3d, fg, msfs, msfs2024))
    {}

    CSimulatorInfo::CSimulatorInfo(int flagsAsInt) : m_simulator(flagsAsInt) {}

    bool CSimulatorInfo::isUnspecified() const { return m_simulator < 1; }

    bool CSimulatorInfo::isXPlane() const { return getSimulator().testFlag(XPLANE); }

    bool CSimulatorInfo::isP3D() const { return getSimulator().testFlag(P3D); }

    bool CSimulatorInfo::isFG() const { return getSimulator().testFlag(FG); }

    bool CSimulatorInfo::isMSFS() const { return getSimulator().testFlag(MSFS); }

    bool CSimulatorInfo::isMSFS2024() const { return getSimulator().testFlag(MSFS2024); }

    bool CSimulatorInfo::isAnySimulator() const { return isXPlane() || isP3D() || isFG() || isMSFS() || isMSFS2024(); }

    bool CSimulatorInfo::isSingleSimulator() const { return this->numberSimulators() == 1; }

    bool CSimulatorInfo::isNoSimulator() const { return m_simulator == 0; }

    bool CSimulatorInfo::isMultipleSimulators() const { return this->numberSimulators() > 1; }

    bool CSimulatorInfo::isAllSimulators() const { return isXPlane() && isP3D() && isFG() && isMSFS() && isMSFS2024(); }

    bool CSimulatorInfo::isMicrosoftSimulator() const { return isMSFS() || isMSFS2024(); }

    bool CSimulatorInfo::isMicrosoftOrPrepare3DSimulator() const { return isMicrosoftSimulator() || isP3D(); }

    bool CSimulatorInfo::isFsxP3DFamily() const { return isP3D() || isMSFS() || isMSFS2024(); }

    int CSimulatorInfo::numberSimulators() const
    {
        int c = isXPlane() ? 1 : 0;
        if (isP3D()) { c++; }
        if (isFG()) { c++; }
        if (isMSFS()) { c++; }
        if (isMSFS2024()) { c++; }
        return c;
    }

    bool CSimulatorInfo::matchesAll(const CSimulatorInfo &otherInfo) const
    {
        return (m_simulator & otherInfo.m_simulator) == otherInfo.m_simulator;
    }

    bool CSimulatorInfo::matchesAny(const CSimulatorInfo &otherInfo) const
    {
        return (m_simulator & otherInfo.m_simulator) > 0;
    }

    bool CSimulatorInfo::matchesAnyOrNone(const CSimulatorInfo &otherInfo) const
    {
        if (this->isNoSimulator()) { return true; }
        return this->matchesAny(otherInfo);
    }

    int CSimulatorInfo::comparePropertyByIndex(CPropertyIndexRef index, const CSimulatorInfo &compareValue) const
    {
        Q_UNUSED(index)
        return Compare::compare(m_simulator, compareValue.m_simulator);
    }

    QString CSimulatorInfo::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        const Simulator s = getSimulator();
        const QString str = (s.testFlag(P3D) ? QStringLiteral("P3D ") : QString()) %
                            (s.testFlag(XPLANE) ? QStringLiteral("XPlane ") : QString()) %
                            (s.testFlag(FG) ? QStringLiteral("FG ") : QString()) %
                            (s.testFlag(MSFS) ? QStringLiteral("MSFS ") : QString()) %
                            (s.testFlag(MSFS2024) ? QStringLiteral("MSFS2024 ") : QString());
        return str.trimmed();
    }

    CIcons::IconIndex CSimulatorInfo::toIcon() const { return CIcons::StandardIconEmpty; }

    CSimulatorInfo CSimulatorInfo::add(const CSimulatorInfo &other)
    {
        // anything to add?
        if (other.isUnspecified()) { return None; }
        if (this->matchesAll(other)) { return None; }

        this->setSimulator(this->getSimulator() | other.getSimulator());
        const CSimulatorInfo delta(this->getSimulator() & other.getSimulator());
        return delta;
    }

    QSet<CSimulatorInfo> CSimulatorInfo::asSingleSimulatorSet() const
    {
        QSet<CSimulatorInfo> set;
        if (m_simulator & P3D) { set.insert(CSimulatorInfo(P3D)); }
        if (m_simulator & FG) { set.insert(CSimulatorInfo(FG)); }
        if (m_simulator & XPLANE) { set.insert(CSimulatorInfo(XPLANE)); }
        if (m_simulator & MSFS) { set.insert(CSimulatorInfo(MSFS)); }
        if (m_simulator & MSFS2024) { set.insert(CSimulatorInfo(MSFS2024)); }
        return set;
    }

    void CSimulatorInfo::invertSimulators()
    {
        m_simulator = (m_simulator ^ static_cast<int>(All)) & static_cast<int>(All);
    }

    CStatusMessage CSimulatorInfo::validateSimulatorsForModel() const
    {
        CStatusMessage m(this);
        if (!this->isAnySimulator()) { return m.validationError(u"No simulator"); }
        if (this->isMicrosoftOrPrepare3DSimulator() && this->isXPlane())
        {
            return m.validationError(u"Cannot combine XPlane and FS simulators");
        }
        if (this->isMicrosoftOrPrepare3DSimulator() && this->isFG())
        {
            return m.validationError(u"Cannot combine FG and FS simulators");
        }
        if (this->isXPlane() && this->isFG()) { return m.validationError(u"Cannot combine FG and XPlane simulators"); }
        return m.info(u"Simulators OK for model");
    }

    CSimulatorInfo::Simulator CSimulatorInfo::boolToFlag(bool xp, bool p3d, bool fg, bool msfs, bool msfs2024)
    {
        Simulator s = xp ? XPLANE : None;
        if (p3d) { s |= P3D; }
        if (fg) { s |= FG; }
        if (msfs) { s |= MSFS; }
        if (msfs2024) { s |= MSFS2024; }
        return s;
    }

    CSimulatorInfo::Simulator CSimulatorInfo::identifierToSimulator(const QString &identifier)
    {
        const QString i(identifier.toLower().trimmed().remove(' ').remove('-'));
        if (i.isEmpty()) { return None; }

        Simulator s = None;
        if (i.contains("plane") || i.contains("xp")) { s |= XPLANE; }
        if (i.contains("gear") || stringCompare(QStringLiteral("fg"), identifier, Qt::CaseInsensitive)) { s |= FG; }
        if (i.contains("3d") || i.contains("prepar") || i.contains("martin") || i.contains("lm") || i.contains("lock"))
        {
            s |= P3D;
        }
        if (i.contains("msfs2024"))
        {
            s |= MSFS2024;
            return s;
        }

        if (i.contains("msfs")) { s |= MSFS; }
        return s;
    }

    const CSimulatorInfo &CSimulatorInfo::allSimulators()
    {
        static const CSimulatorInfo s(All);
        return s;
    }

    const QStringList &CSimulatorInfo::allSimulatorStrings()
    {
        static const QStringList sims = [] {
            QStringList s;
            for (const CSimulatorInfo &i : CSimulatorInfo::allSimulatorsSet()) { s.push_back(i.toQString(false)); }
            s.sort(Qt::CaseInsensitive);
            return s;
        }();
        return sims;
    }

    const QSet<CSimulatorInfo> &CSimulatorInfo::allSimulatorsSet()
    {
        static const QSet<CSimulatorInfo> all(allSimulators().asSingleSimulatorSet());
        return all;
    }

    const CSimulatorInfo &CSimulatorInfo::allFsFamilySimulators()
    {
        static const CSimulatorInfo s(CSimulatorInfo::AllFsFamily);
        return s;
    }

    CSimulatorInfo CSimulatorInfo::getLocallyInstalledSimulators()
    {
        CSimulatorInfo sim;
        bool p3d = false;
        bool fg = false;
        bool msfs = false;
        bool msfs2024 = false;

        if (CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            p3d = !CFsDirectories::p3dDir().isEmpty() && !CFsDirectories::p3dSimObjectsDir().isEmpty();
            msfs = !CFsDirectories::msfsDir().isEmpty() && !CFsDirectories::msfsPackagesDir().isEmpty();
            msfs2024 = !CFsDirectories::msfs2024Dir().isEmpty() && !CFsDirectories::msfs2024PackagesDir().isEmpty();
        }

        const bool xp = !CXPlaneUtil::xplaneRootDir().isEmpty();

        sim.setSimulator(CSimulatorInfo::boolToFlag(xp, p3d, fg, msfs, msfs2024));
        return sim;
    }

    //! \cond PRIVATE
    CSimulatorInfo guessDefaultSimulatorImpl()
    {
        static const CSimulatorInfo locallyInstalled(CSimulatorInfo::getLocallyInstalledSimulators());
        if (CBuildConfig::isRunningOnLinuxPlatform() || CBuildConfig::isRunningOnMacOSPlatform())
        {
            return CSimulatorInfo::xplane();
        }
        if (locallyInstalled.isP3D()) { return CSimulatorInfo::p3d(); }

        // fallback
        return CSimulatorInfo::p3d();
    }
    //! \endcond

    const CSimulatorInfo &CSimulatorInfo::guessDefaultSimulator()
    {
        static const CSimulatorInfo sim(guessDefaultSimulatorImpl());
        return sim;
    }

    CSimulatorInfo CSimulatorInfo::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        const QJsonValue jxp = json.value(prefix % u"simxplane");
        const QJsonValue jp3d = json.value(prefix % u"simp3d");
        const QJsonValue jfg = json.value(prefix % u"simfg");
        const QJsonValue jmsfs = json.value(prefix % u"simmsfs");
        const QJsonValue jmsfs2024 = json.value(prefix % u"simmsfs2024");

        // we handle bool JSON values and bool as string
        const bool xp = jxp.isBool() ? jxp.toBool() : CDatastoreUtility::dbBoolStringToBool(jxp.toString());
        const bool p3d = jp3d.isBool() ? jp3d.toBool() : CDatastoreUtility::dbBoolStringToBool(jp3d.toString());
        const bool fg = jfg.isBool() ? jfg.toBool() : CDatastoreUtility::dbBoolStringToBool(jfg.toString());
        const bool msfs = jmsfs.isBool() ? jmsfs.toBool() : CDatastoreUtility::dbBoolStringToBool(jmsfs.toString());
        const bool msfs2024 =
            jmsfs2024.isBool() ? jmsfs2024.toBool() : CDatastoreUtility::dbBoolStringToBool(jmsfs2024.toString());

        const CSimulatorInfo simInfo(xp, p3d, fg, msfs, msfs2024);
        return simInfo;
    }

    CCountPerSimulator::CCountPerSimulator()
    {
        m_counts.reserve(CSimulatorInfo::NumberOfSimulators + 1);
        for (int i = 0; i < CSimulatorInfo::NumberOfSimulators + 1; i++) { m_counts.push_back(0); }
    }

    int CCountPerSimulator::getCount(const CSimulatorInfo &simulator) const
    {
        return m_counts[internalIndex(simulator)];
    }

    int CCountPerSimulator::getCountForUnknownSimulators() const
    {
        return m_counts[CSimulatorInfo::NumberOfSimulators];
    }

    int CCountPerSimulator::getCountForFsFamilySimulators() const
    {
        return this->getCount(CSimulatorInfo::p3d()) + this->getCount(CSimulatorInfo::msfs()) +
               this->getCount(CSimulatorInfo::msfs2024());
    }

    int CCountPerSimulator::getCountForFsxFamilySimulators() const
    {
        return this->getCount(CSimulatorInfo::p3d()) + this->getCount(CSimulatorInfo::msfs()) +
               this->getCount(CSimulatorInfo::msfs2024());
    }

    int CCountPerSimulator::getMaximum() const { return *std::min_element(m_counts.begin(), m_counts.end()); }

    int CCountPerSimulator::getMinimum() const { return *std::max_element(m_counts.begin(), m_counts.end()); }

    int CCountPerSimulator::simulatorsRepresented() const
    {
        int c = 0;
        for (int i = 0; i < m_counts.size() - 1; i++)
        {
            if (m_counts[i] > 0) { c++; }
        }
        return c;
    }

    QMultiMap<int, CSimulatorInfo> CCountPerSimulator::countPerSimulator() const
    {
        QMultiMap<int, CSimulatorInfo> counts;
        for (int i = 0; i < m_counts.size(); i++) { counts.insert(m_counts[i], simulator(i)); }
        return counts;
    }

    QString CCountPerSimulator::toQString() const
    {
        return u" P3D: " % QString::number(m_counts[1]) % u" XPlane: " % QString::number(m_counts[3]) % u" FG: " %
               QString::number(m_counts[4]) % u" MSFS: " % QString::number(m_counts[5]) % u" MSFS2024: " %
               QString::number(m_counts[6]);
    }

    void CCountPerSimulator::setCount(int count, const CSimulatorInfo &simulator)
    {
        m_counts[internalIndex(simulator)] = count;
    }

    void CCountPerSimulator::increaseSimulatorCounts(const CSimulatorInfo &simulator)
    {
        if (simulator.isNoSimulator() || simulator.isUnspecified())
        {
            // unknown count
            m_counts[6]++;
            return;
        }
        if (simulator.isP3D()) { m_counts[1]++; }
        if (simulator.isXPlane()) { m_counts[3]++; }
        if (simulator.isFG()) { m_counts[4]++; }
        if (simulator.isMSFS()) { m_counts[5]++; }
        if (simulator.isMSFS2024()) { m_counts[6]++; }
    }

    int CCountPerSimulator::internalIndex(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        switch (simulator.getSimulator())
        {
        // 0 => FSX
        case CSimulatorInfo::P3D: return 1;
        // 2 => FS9
        case CSimulatorInfo::XPLANE: return 3;
        case CSimulatorInfo::FG: return 4;
        case CSimulatorInfo::MSFS: return 5;
        case CSimulatorInfo::MSFS2024: return 6;
        default: return CSimulatorInfo::NumberOfSimulators; // unknown
        }
    }

    CSimulatorInfo CCountPerSimulator::simulator(int internalIndex)
    {
        switch (internalIndex)
        {
        case 1: return { CSimulatorInfo::P3D };
        case 3: return { CSimulatorInfo::XPLANE };
        case 4: return { CSimulatorInfo::FG };
        case 5: return { CSimulatorInfo::MSFS };
        case 6: return { CSimulatorInfo::MSFS2024 };
        default: return { CSimulatorInfo::None };
        }
    }
} // namespace swift::misc::simulation
