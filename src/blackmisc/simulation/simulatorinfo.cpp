/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorinfo.h"
#include "blackmisc/project.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"

using namespace BlackMisc;
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

        bool CSimulatorInfo::isNoSimulator() const
        {
            return m_simulator == 0;
        }

        bool CSimulatorInfo::isAllSimulators() const
        {
            return fsx() && fs9() && xplane() && p3d();
        }

        bool CSimulatorInfo::matchesAll(const CSimulatorInfo &otherInfo) const
        {
            return (this->m_simulator & otherInfo.m_simulator) == otherInfo.m_simulator;
        }

        bool CSimulatorInfo::matchesAny(const CSimulatorInfo &otherInfo) const
        {
            return (this->m_simulator & otherInfo.m_simulator) > 0;
        }

        int CSimulatorInfo::comparePropertyByIndex(const CSimulatorInfo &compareValue, const CPropertyIndex &index) const
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
            bool fs9 =
                CProject::isRunningOnWindowsNtPlatform() &&
                !CFsCommonUtil::fs9AircraftDir().isEmpty() &&
                !CFsCommonUtil::fs9Dir().isEmpty();
            bool fsx =
                CProject::isRunningOnWindowsNtPlatform() &&
                !CFsCommonUtil::fsxSimObjectsDir().isEmpty() &&
                !CFsCommonUtil::fsxDir().isEmpty();
            bool p3d =
                CProject::isRunningOnWindowsNtPlatform() &&
                !CFsCommonUtil::p3dDir().isEmpty() &&
                !CFsCommonUtil::p3dSimObjectsDir().isEmpty();
            bool xp = true; //! \todo XP resolution

            sim.setSimulator(
                CSimulatorInfo::boolToFlag(fsx, fs9, xp, p3d)
            );
            return sim;
        }
    } // ns
} // ns
