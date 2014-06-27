/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKSIM_SIMULATORINFOLIST_H
#define BLACKSIM_SIMULATORINFOLIST_H

#include "simulatorinfo.h"
#include "blackmisc/sequence.h"
#include "blackmisc/collection.h"
#include <QStringList>

namespace BlackSim
{
    //! Value object encapsulating a list of SimulatorInfo objects.
    class CSimulatorInfoList : public BlackMisc::CSequence<CSimulatorInfo>
    {
    public:
        //! Default constructor
        CSimulatorInfoList();

        //! Construct from a base class object.
        CSimulatorInfoList(const CSequence<CSimulatorInfo> &other);

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const { return QVariant::fromValue(*this); }

        //! Is simulator supported
        bool supportsSimulator(const CSimulatorInfo &info);

        //! String list with meaningful representations
        QStringList toStringList(bool i18n = false) const;
    };
}

Q_DECLARE_METATYPE(BlackSim::CSimulatorInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackSim::CSimulatorInfo>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackSim::CSimulatorInfo>)

#endif // guard
