// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CMODELSETBUILDER_H
#define SWIFT_CORE_CMODELSETBUILDER_H

#include <QFlags>
#include <QMetaType>
#include <QObject>

#include "core/swiftcoreexport.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributorlist.h"
#include "misc/simulation/simulatorinfo.h"

namespace swift::core
{
    /*!
     * Create model set (normally from own models)
     */
    class SWIFT_CORE_EXPORT CModelSetBuilder : public QObject
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
        swift::misc::simulation::CAircraftModelList buildModelSet(
            const swift::misc::simulation::CSimulatorInfo &simulator,
            const swift::misc::simulation::CAircraftModelList &models,
            const swift::misc::simulation::CAircraftModelList &currentSet, Builder options,
            const swift::misc::simulation::CDistributorList &distributors = {}) const;
    };
} // namespace swift::core

Q_DECLARE_METATYPE(swift::core::CModelSetBuilder::BuilderFlag)
Q_DECLARE_METATYPE(swift::core::CModelSetBuilder::Builder)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::core::CModelSetBuilder::Builder)

#endif // guard
