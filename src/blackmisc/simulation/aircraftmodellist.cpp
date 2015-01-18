/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/predicates.h"

using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {
        CAircraftModelList::CAircraftModelList() { }

        CAircraftModelList::CAircraftModelList(const CSequence<CAircraftModel> &other) :
            CSequence<CAircraftModel>(other)
        { }

        bool CAircraftModelList::containsModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            for (const CAircraftModel &model : (*this))
            {
                if (model.matchesModelString(modelString, sensitivity)) { return true; }
            }
            return false;
        }

        CAircraftModelList CAircraftModelList::findByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return model.matchesModelString(modelString, sensitivity);
            });
        }

        CAircraftModel CAircraftModelList::findFirstByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            CAircraftModelList ml = findByModelString(modelString, sensitivity);
            return ml.frontOrDefault();
        }

        CAircraftModelList CAircraftModelList::findModelsStartingWith(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            CAircraftModelList ml;
            for (const CAircraftModel &model : (*this))
            {
                if (model.getModelString().startsWith(modelString, sensitivity))
                {
                    ml.push_back(model);
                }
            }
            return ml;
        }

        QStringList CAircraftModelList::getModelStrings() const
        {
            QStringList ms;
            for (const CAircraftModel &model : (*this))
            {
                ms.append(model.getModelString());
            }
            ms.sort(Qt::CaseInsensitive);
            return ms;
        }

        void CAircraftModelList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAircraftModel>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAircraftModel>>();
            qRegisterMetaType<BlackMisc::CCollection<CAircraftModel>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAircraftModel>>();
            qRegisterMetaType<CAircraftModelList>();
            qDBusRegisterMetaType<CAircraftModelList>();
            registerMetaValueType<CAircraftModelList>();
        }

    } // namespace
} // namespace
