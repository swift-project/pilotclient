/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/simulation/fscommon/fsdirectories.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/comparefunctions.h"

#include <QJsonValue>
#include <QtGlobal>
#include <QStringBuilder>
#include <algorithm>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Simulation::XPlane;

namespace BlackMisc
{
    namespace Simulation
    {
        CSimulatorInfo::CSimulatorInfo()
        { }

        CSimulatorInfo::CSimulatorInfo(const QString &identifierString) : m_simulator(identifierToSimulator(identifierString))
        { }

        CSimulatorInfo::CSimulatorInfo(const QStringList &simulators)
        {
            const QString identifier = simulators.join(' ');
            m_simulator = identifierToSimulator(identifier);
        }

        CSimulatorInfo::CSimulatorInfo(Simulator simulator) : m_simulator(static_cast<int>(simulator))
        { }

        CSimulatorInfo::CSimulatorInfo(bool fsx, bool fs9, bool xp, bool p3d, bool fg) :
            m_simulator(boolToFlag(fsx, fs9, xp, p3d, fg))
        { }

        CSimulatorInfo::CSimulatorInfo(int flagsAsInt) :
            m_simulator(flagsAsInt)
        { }

        bool CSimulatorInfo::isUnspecified() const
        {
            return m_simulator < 1;
        }

        bool CSimulatorInfo::isFSX() const
        {
            return getSimulator().testFlag(FSX);
        }

        bool CSimulatorInfo::isFS9() const
        {
            return getSimulator().testFlag(FS9);
        }

        bool CSimulatorInfo::isXPlane() const
        {
            return getSimulator().testFlag(XPLANE);
        }

        bool CSimulatorInfo::isP3D() const
        {
            return getSimulator().testFlag(P3D);
        }

        bool CSimulatorInfo::isFG() const
        {
            return getSimulator().testFlag(FG);
        }

        bool CSimulatorInfo::isMSFS() const
        {
            return getSimulator().testFlag(MSFS);
        }

        bool CSimulatorInfo::isAnySimulator() const
        {
            return isFSX() || isFS9() || isXPlane() || isP3D() || isFG();
        }

        bool CSimulatorInfo::isSingleSimulator() const
        {
            return this->numberSimulators() == 1;
        }

        bool CSimulatorInfo::isNoSimulator() const
        {
            return m_simulator == 0;
        }

        bool CSimulatorInfo::isMultipleSimulators() const
        {
            return this->numberSimulators() > 1;
        }

        bool CSimulatorInfo::isAllSimulators() const
        {
            return isFSX() && isFS9() && isXPlane() && isP3D() && isFG();
        }

        bool CSimulatorInfo::isMicrosoftSimulator() const
        {
            return isFSX() || isFS9();
        }

        bool CSimulatorInfo::isMicrosoftOrPrepare3DSimulator() const
        {
            return isMicrosoftSimulator() || isP3D();
        }

        bool CSimulatorInfo::isFsxP3DFamily() const
        {
            return isFSX() || isP3D();
        }

        int CSimulatorInfo::numberSimulators() const
        {
            int c = isFS9() ? 1 : 0;
            if (isFSX())    { c++; }
            if (isXPlane()) { c++; }
            if (isP3D())    { c++; }
            if (isFG())     { c++; }
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
            const QString str =
                (s.testFlag(FSX)    ? QStringLiteral("FSX ") : QString()) %
                (s.testFlag(FS9)    ? QStringLiteral("FS9 ") : QString()) %
                (s.testFlag(P3D)    ? QStringLiteral("P3D ") : QString()) %
                (s.testFlag(XPLANE) ? QStringLiteral("XPlane ") : QString()) %
                (s.testFlag(FG)     ? QStringLiteral("FG ")  : QString());
            return str.trimmed();
        }

        CIcons::IconIndex CSimulatorInfo::toIcon() const
        {
            if (this->isSingleSimulator())
            {
                switch (this->getSimulator())
                {
                case FSX: return CIcons::SimulatorFSX16;
                case FS9: return CIcons::SimulatorFS916;
                case P3D: return CIcons::SimulatorP3D16;
                case XPLANE: return CIcons::SimulatorXPlane16;
                case FG: return CIcons::SimulatorXPlane16;
                default: break;
                }
            }
            return CValueObject::toIcon();
        }

        CSimulatorInfo CSimulatorInfo::add(const CSimulatorInfo &other)
        {
            // anything to add?
            if (other.isUnspecified())   { return None; }
            if (this->matchesAll(other)) { return None; }

            this->setSimulator(this->getSimulator() | other.getSimulator());
            const CSimulatorInfo delta(this->getSimulator() & other.getSimulator());
            return delta;
        }

        QSet<CSimulatorInfo> CSimulatorInfo::asSingleSimulatorSet() const
        {
            QSet<CSimulatorInfo> set;
            if (m_simulator & FSX) { set.insert(CSimulatorInfo(FSX)); }
            if (m_simulator & FS9) { set.insert(CSimulatorInfo(FS9)); }
            if (m_simulator & P3D) { set.insert(CSimulatorInfo(P3D)); }
            if (m_simulator & FG)  { set.insert(CSimulatorInfo(FG)); }
            if (m_simulator & XPLANE) { set.insert(CSimulatorInfo(XPLANE)); }
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
            if (this->isMicrosoftOrPrepare3DSimulator() && this->isXPlane()) { return m.validationError(u"Cannot combine XPlane and FS simulators"); }
            if (this->isMicrosoftOrPrepare3DSimulator() && this->isFG()) { return m.validationError(u"Cannot combine FG and FS simulators"); }
            if (this->isXPlane() && this->isFG()) { return m.validationError(u"Cannot combine FG and XPlane simulators"); }
            return m.info(u"Simulators OK for model");
        }

        CSimulatorInfo::Simulator CSimulatorInfo::boolToFlag(bool fsx, bool fs9, bool xp, bool p3d, bool fg)
        {
            Simulator s = fsx ? FSX : None;
            if (fs9) { s |= FS9; }
            if (xp)  { s |= XPLANE; }
            if (p3d) { s |= P3D; }
            if (fg)  { s |= FG; }
            return s;
        }

        CSimulatorInfo::Simulator CSimulatorInfo::identifierToSimulator(const QString &identifier)
        {
            const QString i(identifier.toLower().trimmed().remove(' ').remove('-'));
            if (i.isEmpty()) { return None; }

            Simulator s = None;
            if (i.contains("fsx")   || i.contains("fs10")) { s |= FSX; }
            if (i.contains("fs9")   || i.contains("2004")) { s |= FS9; }
            if (i.contains("plane") || i.contains("xp"))   { s |= XPLANE; }
            if (i.contains("gear")  || stringCompare(QStringLiteral("fg"), identifier, Qt::CaseInsensitive)) { s |= FG; }
            if (i.contains("3d")    || i.contains("prepar") || i.contains("martin") || i.contains("lm") || i.contains("lock"))
            {
                s |= P3D;
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
            static const QStringList sims = []
            {
                QStringList s;
                for (const CSimulatorInfo &i : CSimulatorInfo::allSimulatorsSet())
                {
                    s.push_back(i.toQString(false));
                }
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

        const CSimulatorInfo CSimulatorInfo::getLocallyInstalledSimulators()
        {
            CSimulatorInfo sim;
            bool fs9 = false;
            bool fsx = false;
            bool p3d = false;
            bool fg  = false;

            if (CBuildConfig::isRunningOnWindowsNtPlatform())
            {
                fs9 =
                    !CFsDirectories::fs9AircraftDir().isEmpty() &&
                    !CFsDirectories::fs9Dir().isEmpty();
                fsx =
                    !CFsDirectories::fsxSimObjectsDir().isEmpty() &&
                    !CFsDirectories::fsxDir().isEmpty();
                p3d =
                    !CFsDirectories::p3dDir().isEmpty() &&
                    !CFsDirectories::p3dSimObjectsDir().isEmpty();
            }

            const bool xp = !CXPlaneUtil::xplaneRootDir().isEmpty();

            sim.setSimulator(CSimulatorInfo::boolToFlag(fsx, fs9, xp, p3d, fg));
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
            if (locallyInstalled.isFSX()) { return CSimulatorInfo::fsx(); }
            if (locallyInstalled.isFS9()) { return CSimulatorInfo::fs9(); }

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
            const QJsonValue jfsx = json.value(prefix % u"simfsx");
            const QJsonValue jfs9 = json.value(prefix % u"simfs9");
            const QJsonValue jxp  = json.value(prefix % u"simxplane");
            const QJsonValue jp3d = json.value(prefix % u"simp3d");
            const QJsonValue jfg  = json.value(prefix % u"simfg");

            // we handle bool JSON values and bool as string
            const bool fsx = jfsx.isBool() ? jfsx.toBool() : CDatastoreUtility::dbBoolStringToBool(jfsx.toString());
            const bool fs9 = jfs9.isBool() ? jfs9.toBool() : CDatastoreUtility::dbBoolStringToBool(jfs9.toString());
            const bool xp  = jxp.isBool()  ? jxp.toBool()  : CDatastoreUtility::dbBoolStringToBool(jxp.toString());
            const bool p3d = jp3d.isBool() ? jp3d.toBool() : CDatastoreUtility::dbBoolStringToBool(jp3d.toString());
            const bool fg  = jfg.isBool()  ? jfg.toBool()  : CDatastoreUtility::dbBoolStringToBool(jfg.toString());

            const CSimulatorInfo simInfo(fsx, fs9, xp, p3d, fg);
            return simInfo;
        }

        CCountPerSimulator::CCountPerSimulator()
        {
            m_counts.reserve(CSimulatorInfo::NumberOfSimulators + 1);
            for (int i = 0; i < CSimulatorInfo::NumberOfSimulators + 1; i++)
            {
                m_counts.push_back(0);
            }
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
            return this->getCount(CSimulatorInfo::fsx()) + this->getCount(CSimulatorInfo::p3d()) + this->getCount(CSimulatorInfo::fs9());
        }

        int CCountPerSimulator::getCountForFsxFamilySimulators() const
        {
            return this->getCount(CSimulatorInfo::fsx()) + this->getCount(CSimulatorInfo::p3d());
        }

        int CCountPerSimulator::getMaximum() const
        {
            return *std::min_element(m_counts.begin(), m_counts.end());
        }

        int CCountPerSimulator::getMinimum() const
        {
            return *std::max_element(m_counts.begin(), m_counts.end());
        }

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
            for (int i = 0; i < m_counts.size(); i++)
            {
                counts.insertMulti(m_counts[i], simulator(i));
            }
            return counts;
        }

        QString CCountPerSimulator::toQString() const
        {
            return u"FSX: "     % QString::number(m_counts[0]) %
                   u" P3D: "    % QString::number(m_counts[1]) %
                   u" FS9: "    % QString::number(m_counts[2]) %
                   u" XPlane: " % QString::number(m_counts[3]) %
                   u" FG: "     % QString::number(m_counts[4]);
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
                m_counts[5]++;
                return;
            }
            if (simulator.isFSX())    { m_counts[0]++; }
            if (simulator.isP3D())    { m_counts[1]++; }
            if (simulator.isFS9())    { m_counts[2]++; }
            if (simulator.isXPlane()) { m_counts[3]++; }
            if (simulator.isFG())     { m_counts[4]++; }
        }

        int CCountPerSimulator::internalIndex(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            switch (simulator.getSimulator())
            {
            case CSimulatorInfo::FSX:    return 0;
            case CSimulatorInfo::P3D:    return 1;
            case CSimulatorInfo::FS9:    return 2;
            case CSimulatorInfo::XPLANE: return 3;
            case CSimulatorInfo::FG:     return 4;
            default: return CSimulatorInfo::NumberOfSimulators; // unknown
            }
        }

        CSimulatorInfo CCountPerSimulator::simulator(int internalIndex)
        {
            switch (internalIndex)
            {
            case 0: return CSimulatorInfo(CSimulatorInfo::FSX);
            case 1: return CSimulatorInfo(CSimulatorInfo::P3D);
            case 2: return CSimulatorInfo(CSimulatorInfo::FS9);
            case 3: return CSimulatorInfo(CSimulatorInfo::XPLANE);
            case 4: return CSimulatorInfo(CSimulatorInfo::FG);
            default: return CSimulatorInfo(CSimulatorInfo::None);
            }
        }
    } // ns
} // ns
