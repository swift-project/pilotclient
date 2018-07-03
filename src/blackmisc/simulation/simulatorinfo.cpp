/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
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

        CSimulatorInfo::CSimulatorInfo(bool fsx, bool fs9, bool xp, bool p3d) :
            m_simulator(boolToFlag(fsx, fs9, xp, p3d))
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

        bool CSimulatorInfo::isAnySimulator() const
        {
            return isFSX() || isFS9() || isXPlane() || isP3D();
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
            return isFSX() && isFS9() && isXPlane() && isP3D();
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
            if (isFSX()) { c++; }
            if (isXPlane()) { c++; }
            if (isP3D()) { c++; }
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

        int CSimulatorInfo::comparePropertyByIndex(const CPropertyIndex &index, const CSimulatorInfo &compareValue) const
        {
            Q_UNUSED(index);
            return Compare::compare(m_simulator, compareValue.m_simulator);
        }

        QString CSimulatorInfo::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            const Simulator s = getSimulator();
            QString str;
            if (s.testFlag(FSX)) { str.append("FSX "); }
            if (s.testFlag(FS9)) { str.append("FS9 "); }
            if (s.testFlag(P3D)) { str.append("P3D "); }
            if (s.testFlag(XPLANE)) { str.append("XPlane"); }
            return str.trimmed();
        }

        void CSimulatorInfo::add(const CSimulatorInfo &other)
        {
            if (other.isUnspecified()) { return; }
            this->setSimulator(this->getSimulator() | other.getSimulator());
        }

        QSet<CSimulatorInfo> CSimulatorInfo::asSingleSimulatorSet() const
        {
            QSet<CSimulatorInfo> set;
            if (m_simulator & FSX) { set.insert(CSimulatorInfo(FSX)); }
            if (m_simulator & FS9) { set.insert(CSimulatorInfo(FS9)); }
            if (m_simulator & P3D) { set.insert(CSimulatorInfo(P3D)); }
            if (m_simulator & XPLANE) { set.insert(CSimulatorInfo(XPLANE)); }
            return set;
        }

        void CSimulatorInfo::invertSimulators()
        {
            m_simulator = (m_simulator ^ static_cast<int>(All)) & static_cast<int>(All);
        }

        const QString &CSimulatorInfo::toPluginIdentifier() const
        {
            static const QString e;
            if (!this->isSingleSimulator()) { return e; }
            const Simulator s = getSimulator();
            if (s.testFlag(FSX)) { return CSimulatorPluginInfo::fsxPluginIdentifier(); }
            if (s.testFlag(FS9)) { return CSimulatorPluginInfo::fs9PluginIdentifier(); }
            if (s.testFlag(P3D)) { return CSimulatorPluginInfo::p3dPluginIdentifier(); }
            if (s.testFlag(XPLANE)) { return CSimulatorPluginInfo::xplanePluginIdentifier(); }
            return e;
        }

        CSimulatorInfo::Simulator CSimulatorInfo::boolToFlag(bool fsx, bool fs9, bool xp, bool p3d)
        {
            Simulator s = fsx ? FSX : None;
            if (fs9) { s |= FS9; }
            if (xp)  { s |= XPLANE; }
            if (p3d) { s |= P3D; }
            return s;
        }

        CSimulatorInfo::Simulator CSimulatorInfo::identifierToSimulator(const QString &identifier)
        {
            const QString i(identifier.toLower().trimmed().remove(' ').remove('-'));
            if (i.isEmpty()) { return None; }

            Simulator s = None;
            if (i.contains("fsx") || i.contains("fs10")) { s |= FSX; }
            if (i.contains("fs9") || i.contains("2004")) { s |= FS9; }
            if (i.contains("plane")  || i.contains("xp")) { s |= XPLANE; }
            if (i.contains("3d")  || i.contains("prepar") || i.contains("martin") || i.contains("lm") || i.contains("lock"))
            {
                s |= P3D;
            }
            return s;
        }

        const CSimulatorInfo &CSimulatorInfo::allSimulators()
        {
            static const CSimulatorInfo s(All);
            return s;
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

            if (CBuildConfig::isRunningOnWindowsNtPlatform())
            {
                fs9 =
                    !CFsCommonUtil::fs9AircraftDir().isEmpty() &&
                    !CFsCommonUtil::fs9Dir().isEmpty();
                fsx =
                    !CFsCommonUtil::fsxSimObjectsDir().isEmpty() &&
                    !CFsCommonUtil::fsxDir().isEmpty();
                p3d =
                    !CFsCommonUtil::p3dDir().isEmpty() &&
                    !CFsCommonUtil::p3dSimObjectsDir().isEmpty();
            }

            const bool xp = !CXPlaneUtil::xplaneRootDir().isEmpty();

            sim.setSimulator(CSimulatorInfo::boolToFlag(fsx, fs9, xp, p3d));
            return sim;
        }

        //! \cond PRIVATE
        CSimulatorInfo guessDefaultSimulatorImpl()
        {
            static const CSimulatorInfo locallyInstalled(CSimulatorInfo::getLocallyInstalledSimulators());
            if (CBuildConfig::isRunningOnLinuxPlatform())
            {
                return CSimulatorInfo("XPLANE");
            }
            if (locallyInstalled.isP3D()) { return CSimulatorInfo("P3D"); }
            if (locallyInstalled.isFSX()) { return CSimulatorInfo("FSX"); }
            if (locallyInstalled.isFS9()) { return CSimulatorInfo("FS9"); }

            // fallback
            return CSimulatorInfo("P3D");
        }
        //! \endcond

        const CSimulatorInfo &CSimulatorInfo::guessDefaultSimulator()
        {
            static const CSimulatorInfo sim(guessDefaultSimulatorImpl());
            return sim;
        }

        CSimulatorInfo CSimulatorInfo::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            const QJsonValue jfsx = json.value(prefix + "simfsx");
            const QJsonValue jfs9 = json.value(prefix + "simfs9");
            const QJsonValue jxp  = json.value(prefix + "simxplane");
            const QJsonValue jp3d = json.value(prefix + "simp3d");

            // we handle bool JSON values and bool as string
            const bool fsx = jfsx.isBool() ? jfsx.toBool() : CDatastoreUtility::dbBoolStringToBool(jfsx.toString());
            const bool fs9 = jfs9.isBool() ? jfs9.toBool() : CDatastoreUtility::dbBoolStringToBool(jfs9.toString());
            const bool xp  = jxp.isBool()  ? jxp.toBool()  : CDatastoreUtility::dbBoolStringToBool(jxp.toString());
            const bool p3d = jp3d.isBool() ? jp3d.toBool() : CDatastoreUtility::dbBoolStringToBool(jp3d.toString());

            const CSimulatorInfo simInfo(fsx, fs9, xp, p3d);
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
            return "FSX: " % QString::number(m_counts[0]) %
                   " P3D: " % QString::number(m_counts[1]) %
                   " FS9: " % QString::number(m_counts[2]) %
                   " XPlane: " % QString::number(m_counts[3]);
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
                m_counts[4] = m_counts[4] + 1;
                return;
            }
            if (simulator.isFSX())    { m_counts[0]++; }
            if (simulator.isP3D())    { m_counts[1]++; }
            if (simulator.isFS9())    { m_counts[2]++; }
            if (simulator.isXPlane()) { m_counts[3]++;  }
        }

        int CCountPerSimulator::internalIndex(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            switch (simulator.getSimulator())
            {
            case CSimulatorInfo::FSX: return 0;
            case CSimulatorInfo::P3D: return 1;
            case CSimulatorInfo::FS9: return 2;
            case CSimulatorInfo::XPLANE: return 3;
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
            default: return CSimulatorInfo(CSimulatorInfo::None);
            }
        }
    } // ns
} // ns
