/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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

        //! Register the metatypes
        static void registerMetadata();

    };
}

Q_DECLARE_METATYPE(BlackSim::CSimulatorInfoList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackSim::CSimulatorInfo>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackSim::CSimulatorInfo>)

#endif // guard
