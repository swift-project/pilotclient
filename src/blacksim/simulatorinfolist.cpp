/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
        QStringList infoList;
        foreach(CSimulatorInfo info, (*this))
        {
            QString i = info.toQString(i18n);
            infoList.append(i);
        }
        return infoList;
    }

} // namespace BlackSim
