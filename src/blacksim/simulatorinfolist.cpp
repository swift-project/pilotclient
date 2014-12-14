/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorinfolist.h"

namespace BlackSim
{

    CSimulatorInfoList::CSimulatorInfoList() { }

    bool CSimulatorInfoList::supportsSimulator(const CSimulatorInfo &info)
    {
        return this->contains(info);
    }

    QStringList CSimulatorInfoList::toStringList(bool i18n) const
    {
        return this->transform([i18n](const CSimulatorInfo &info) { return info.toQString(i18n); });
    }

    void CSimulatorInfoList::registerMetadata()
    {
        qRegisterMetaType<BlackMisc::CSequence<CSimulatorInfo>>();
        qDBusRegisterMetaType<BlackMisc::CSequence<CSimulatorInfo>>();
        qRegisterMetaType<BlackMisc::CCollection<CSimulatorInfo>>();
        qDBusRegisterMetaType<BlackMisc::CCollection<CSimulatorInfo>>();
        qRegisterMetaType<CSimulatorInfoList>();
        qDBusRegisterMetaType<CSimulatorInfoList>();
        BlackMisc::registerMetaValueType<CSimulatorInfoList>();
    }

} // namespace BlackSim
