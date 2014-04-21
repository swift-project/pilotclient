/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKSIM_SIMULATORINFOLIST_H
#define BLACKSIM_SIMULATORINFOLIST_H

#include "simulatorinfo.h"
#include "blackmisc/sequence.h"
#include "blackmisc/collection.h"

namespace BlackSim
{
    //! \brief Value object encapsulating a list of SimulatorInfos.
    class CSimulatorInfoList : public BlackMisc::CSequence<CSimulatorInfo>
    {
    public:
        CSimulatorInfoList();

        //! \brief Construct from a base class object.
        CSimulatorInfoList(const CSequence<CSimulatorInfo> &other);

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const
        {
            return QVariant::fromValue(*this);
        }
    };

}

Q_DECLARE_METATYPE(BlackSim::CSimulatorInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackSim::CSimulatorInfo>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackSim::CSimulatorInfo>)

#endif // BLACKSIM_SIMULATORINFOLIST_H
