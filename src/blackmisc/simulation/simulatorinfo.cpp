/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorinfo.h"

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Simulation
    {
        CSimulatorInfo::CSimulatorInfo()
        { }

        CSimulatorInfo::CSimulatorInfo(Simulator s) : m_simulator(static_cast<int>(s))
        { }

        CSimulatorInfo::CSimulatorInfo(bool fsx, bool fs9, bool xp) :
            m_simulator(boolToFlag(fsx, fs9, xp))
        { }

        bool CSimulatorInfo::isUnspecified() const
        {
            return m_simulator < 1;
        }

        QString CSimulatorInfo::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            Simulator s = getSimulator();
            QString str;
            if (s.testFlag(FSX)) { str.append("FSX "); }
            if (s.testFlag(FS9)) { str.append("FS9 "); }
            if (s.testFlag(XP)) { str.append("XPlane "); }
            return str.trimmed();
        }

        CSimulatorInfo::Simulator CSimulatorInfo::boolToFlag(bool fsx, bool fs9, bool xp)
        {
            Simulator s = fsx ? FSX : None;
            if (fs9) { s |= FS9; }
            if (xp)  { s |= XP; }
            return s;
        }

        CSimulatorInfo::Simulator CSimulatorInfo::identifierToFlag(const QString &identifier)
        {
            QString i(identifier.toLower().trimmed());
            if (i.isEmpty()) { return None; }

            Simulator s = None;
            if (i.contains("fsx"))
            {
                s |= FSX;
            }
            if (i.contains("fs9"))
            {
                s |= FS9;
            }
            if (i.contains("xplane")  || i.contains("xp"))
            {
                s |= XP;
            }
            return s;
        }
    } // ns
} // ns
