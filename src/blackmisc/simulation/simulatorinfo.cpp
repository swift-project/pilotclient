/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include <QJsonValue>
#include <QtGlobal>
#include <algorithm>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Simulation::FsCommon;

namespace BlackMisc
{
    namespace Simulation
    {
        CSimulatorInfo::CSimulatorInfo()
        { }

        CSimulatorInfo::CSimulatorInfo(const QString &identifierString) :  m_simulator(identifierToFlag(identifierString))
        { }

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

        bool CSimulatorInfo::fsx() const
        {
            return getSimulator().testFlag(FSX);
        }

        bool CSimulatorInfo::fs9() const
        {
            return getSimulator().testFlag(FS9);
        }

        bool CSimulatorInfo::xplane() const
        {
            return getSimulator().testFlag(XPLANE);
        }

        bool CSimulatorInfo::p3d() const
        {
            return getSimulator().testFlag(P3D);
        }

        bool CSimulatorInfo::isAnySimulator() const
        {
            return fsx() || fs9() || xplane() || p3d();
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
            return fsx() && fs9() && xplane() && p3d();
        }

        bool CSimulatorInfo::isMicrosoftSimulator() const
        {
            return fsx() || fs9();
        }

        bool CSimulatorInfo::isMicrosoftOrPrepare3DSimulator() const
        {
            return isMicrosoftSimulator() || p3d();
        }

        int CSimulatorInfo::numberSimulators() const
        {
            int c = fs9() ? 1 : 0;
            if (fsx()) { c++; }
            if (xplane()) { c++; }
            if (p3d()) { c++; }
            return c;
        }

        bool CSimulatorInfo::matchesAll(const CSimulatorInfo &otherInfo) const
        {
            return (this->m_simulator & otherInfo.m_simulator) == otherInfo.m_simulator;
        }

        bool CSimulatorInfo::matchesAny(const CSimulatorInfo &otherInfo) const
        {
            return (this->m_simulator & otherInfo.m_simulator) > 0;
        }

        int CSimulatorInfo::comparePropertyByIndex(const CPropertyIndex &index, const CSimulatorInfo &compareValue) const
        {
            Q_UNUSED(index);
            return Compare::compare(this->m_simulator, compareValue.m_simulator);
        }

        QString CSimulatorInfo::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            Simulator s = getSimulator();
            QString str;
            if (s.testFlag(FSX)) { str.append("FSX "); }
            if (s.testFlag(FS9)) { str.append("FS9 "); }
            if (s.testFlag(XPLANE)) { str.append("XPlane "); }
            if (s.testFlag(P3D)) { str.append("P3D "); }
            return str.trimmed();
        }

        void CSimulatorInfo::add(const CSimulatorInfo &other)
        {
            if (other.isUnspecified()) { return; }
            this->setSimulator(this->getSimulator() | other.getSimulator());
        }

        CSimulatorInfo::Simulator CSimulatorInfo::boolToFlag(bool fsx, bool fs9, bool xp, bool p3d)
        {
            Simulator s = fsx ? FSX : None;
            if (fs9) { s |= FS9; }
            if (xp)  { s |= XPLANE; }
            if (p3d) { s |= P3D; }
            return s;
        }

        CSimulatorInfo::Simulator CSimulatorInfo::identifierToFlag(const QString &identifier)
        {
            QString i(identifier.toLower().trimmed().remove(' ').remove('-'));
            if (i.isEmpty()) { return None; }

            Simulator s = None;
            if (i.contains("fsx") || i.contains("fs10"))
            {
                s |= FSX;
            }
            if (i.contains("fs9") || i.contains("2004"))
            {
                s |= FS9;
            }
            if (i.contains("plane")  || i.contains("xp"))
            {
                s |= XPLANE;
            }
            if (i.contains("3d")  || i.contains("prepare") || i.contains("martin") || i.contains("lm") || i.contains("lock"))
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

        const CSimulatorInfo CSimulatorInfo::getLocallyInstalledSimulators()
        {
            //! \todo add XP, ...
            CSimulatorInfo sim;
            bool fs9 = false;
            bool fsx = false;
            bool p3d = false;

            if (CBuildConfig::isRunningOnWindowsNtPlatform())
            {
                fs9 =
                    CBuildConfig::isRunningOnWindowsNtPlatform() &&
                    !CFsCommonUtil::fs9AircraftDir().isEmpty() &&
                    !CFsCommonUtil::fs9Dir().isEmpty();
                fsx =
                    CBuildConfig::isRunningOnWindowsNtPlatform() &&
                    !CFsCommonUtil::fsxSimObjectsDir().isEmpty() &&
                    !CFsCommonUtil::fsxDir().isEmpty();
                p3d =
                    CBuildConfig::isRunningOnWindowsNtPlatform() &&
                    !CFsCommonUtil::p3dDir().isEmpty() &&
                    !CFsCommonUtil::p3dSimObjectsDir().isEmpty();
            }
            bool xp = true; //! \todo XP resolution

            sim.setSimulator(CSimulatorInfo::boolToFlag(fsx, fs9, xp, p3d));
            return sim;
        }

        const CSimulatorInfo CSimulatorInfo::guessDefaultSimulator()
        {
            CSimulatorInfo locallyInstalled(getLocallyInstalledSimulators());
            if (CBuildConfig::isRunningOnLinuxPlatform())
            {
                return CSimulatorInfo("XPLANE");
            }
            if (locallyInstalled.p3d()) { return CSimulatorInfo("P3D"); }
            if (locallyInstalled.fsx()) { return CSimulatorInfo("FSX"); }
            if (locallyInstalled.fs9()) { return CSimulatorInfo("FS9"); }

            // fallback
            return CSimulatorInfo("FSX");
        }

        CSimulatorInfo CSimulatorInfo::fromDatabaseJson(const QJsonObject &json, const QString prefix)
        {
            const bool fsx = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "simfsx").toString());
            const bool fs9 = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "simfs9").toString());
            const bool xp = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "simxplane").toString());
            const bool p3d = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "simp3d").toString());

            const CSimulatorInfo simInfo(fsx, fs9, xp, p3d);
            return simInfo;
        }

        CCountPerSimulator::CCountPerSimulator()
        {
            this->m_counts.reserve(CSimulatorInfo::NumberOfSimulators + 1);
            for (int i = 0; i < CSimulatorInfo::NumberOfSimulators + 1; i++)
            {
                this->m_counts.push_back(0);
            }
        }

        int CCountPerSimulator::getCount(const CSimulatorInfo &simulator) const
        {
            return this->m_counts[internalIndex(simulator)];
        }

        int CCountPerSimulator::getCountForUnknownSimulators() const
        {
            return this->m_counts[CSimulatorInfo::NumberOfSimulators];
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
            for (int i = 0; i < this->m_counts.size() - 1; i++)
            {
                if (this->m_counts[i] > 0) { c++; }
            }
            return c;
        }

        QMultiMap<int, CSimulatorInfo> CCountPerSimulator::countPerSimulator() const
        {
            QMultiMap<int, CSimulatorInfo> counts;
            for (int i = 0; i < this->m_counts.size(); i++)
            {
                counts.insertMulti(this->m_counts[i], simulator(i));
            }
            return counts;
        }

        void CCountPerSimulator::setCount(int count, const CSimulatorInfo &simulator)
        {
            this->m_counts[internalIndex(simulator)] = count;
        }

        void CCountPerSimulator::increaseSimulatorCounts(const CSimulatorInfo &simulator)
        {
            if (simulator.isNoSimulator() || simulator.isUnspecified())
            {
                // unknown count
                m_counts[4] = m_counts[4] + 1;
                return;
            }
            if (simulator.fsx()) { m_counts[0] = m_counts[0] + 1; }
            if (simulator.p3d()) { m_counts[1] = m_counts[1] + 1; }
            if (simulator.fs9()) { m_counts[2] = m_counts[2] + 1; }
            if (simulator.xplane()) { m_counts[3] = m_counts[3] + 1; }
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
            default:
                return CSimulatorInfo::NumberOfSimulators; // unknown
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
            default:
                return CSimulatorInfo(CSimulatorInfo::None);
            }
        }
    } // ns
} // ns
