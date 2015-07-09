/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODELLIST_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODELLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/datastoreobjectlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Value object encapsulating a list of aircraft models
        class BLACKMISC_EXPORT CAircraftModelList :
            public CSequence<CAircraftModel>,
            public IDatastoreObjectListWithIntegerKey<CAircraftModel, CAircraftModelList>,
            public BlackMisc::Mixin::MetaType<CAircraftModelList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftModelList)

            //! Empty constructor.
            CAircraftModelList();

            //! Construct from a base class object.
            CAircraftModelList(const CSequence<CAircraftModel> &other);

            //! Contains model string
            bool containsModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find by model string
            CAircraftModelList findByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find first by model string
            CAircraftModel findFirstByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find models starting with
            CAircraftModelList findModelsStartingWith(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Model strings
            QStringList getSortedModelStrings() const;
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModelList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CAircraftModel>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::CAircraftModel>)

#endif //guard
