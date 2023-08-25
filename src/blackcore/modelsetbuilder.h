// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CMODELSETBUILDER_H
#define BLACKCORE_CMODELSETBUILDER_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QFlags>
#include <QMetaType>
#include <QObject>

namespace BlackCore
{
    /*!
     * Create model set (normally from own models)
     */
    class BLACKCORE_EXPORT CModelSetBuilder : public QObject
    {
        Q_OBJECT

    public:
        //! Builder flags
        enum BuilderFlag
        {
            NoOptions = 0,
            GivenDistributorsOnly = 1 << 0,
            OnlyDbData = 1 << 1,
            OnlyDbIcaoCodes = 1 << 2,
            Incremental = 1 << 3,
            SortByDistributors = 1 << 4,
            ConsolidateWithDb = 1 << 5
        };
        Q_DECLARE_FLAGS(Builder, BuilderFlag)

        //! Constructor
        CModelSetBuilder(QObject *parent = nullptr);

        //! Build a model set
        BlackMisc::Simulation::CAircraftModelList buildModelSet(
            const BlackMisc::Simulation::CSimulatorInfo &simulator,
            const BlackMisc::Simulation::CAircraftModelList &models,
            const BlackMisc::Simulation::CAircraftModelList &currentSet, Builder options,
            const BlackMisc::Simulation::CDistributorList &distributors = {}) const;
    };
} // ns

Q_DECLARE_METATYPE(BlackCore::CModelSetBuilder::BuilderFlag)
Q_DECLARE_METATYPE(BlackCore::CModelSetBuilder::Builder)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackCore::CModelSetBuilder::Builder)

#endif // guard
