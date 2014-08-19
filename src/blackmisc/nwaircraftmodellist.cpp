/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nwaircraftmodellist.h"
#include "predicates.h"

using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Empty constructor
         */
        CAircraftModelList::CAircraftModelList() { }

        /*
         * Construct from base class object
         */
        CAircraftModelList::CAircraftModelList(const CSequence<CAircraftModel> &other) :
            CSequence<CAircraftModel>(other)
        { }

        CAircraftModelList CAircraftModelList::findByModelString(const QString modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ = ](const CAircraftModel &model)
            {
                return model.matchesModelString(modelString, sensitivity);
            });
        }

        /*
         * Register metadata
         */
        void CAircraftModelList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAircraftModel>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAircraftModel>>();
            qRegisterMetaType<BlackMisc::CCollection<CAircraftModel>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAircraftModel>>();
            qRegisterMetaType<CAircraftModelList>();
            qDBusRegisterMetaType<CAircraftModelList>();
        }

    } // namespace
} // namespace
