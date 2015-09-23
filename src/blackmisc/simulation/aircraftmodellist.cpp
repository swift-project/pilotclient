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
            return this->findFirstBy([ = ](const CAircraftModel & model)
            {
                return model.matchesModelString(modelString, sensitivity);
            });
        }

        CAircraftModelList CAircraftModelList::findByIcaoDesignators(const CAircraftIcaoCode &aircraftIcaoCode, const CAirlineIcaoCode &airlineIcaoCode) const
        {
            const QString aircraft(aircraftIcaoCode.getDesignator());
            const QString airline(airlineIcaoCode.getDesignator());

            if (airline.isEmpty())
            {
                return this->findBy([ = ](const CAircraftModel & model)
                {
                    return model.getAircraftIcaoCode().getDesignator() == aircraft;
                });
            }
            if (aircraft.isEmpty())
            {
                return this->findBy([ = ](const CAircraftModel & model)
                {
                    return model.getAirlineIcaoCode().getDesignator() == airline;
                });
            }
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return model.getAirlineIcaoCode().getDesignator() == airline &&
                       model.getAircraftIcaoCode().getDesignator() == aircraft;
            });
        }

        CAircraftModelList CAircraftModelList::findByAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const
        {
            if (aircraftDesignator.isEmpty()) { return CAircraftModelList(); }
            return this->findBy([ = ](const CAircraftModel & model)
            {
                if (!model.getAircraftIcaoCode().matchesDesignator(aircraftDesignator)) { return false; }
                return model.getLivery().matchesCombinedCode(combinedCode);
            });
        }

        void CAircraftModelList::setSimulatorInfo(const CSimulatorInfo &info)
        {
            for (CAircraftModel &model : (*this))
            {
                model.setSimulatorInfo(info);
            }
        }

        int CAircraftModelList::keepModelsWithString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity)
        {
            int cs = this->size();
            (*this) = (findByModelStrings(modelStrings, sensitivity));
            int d = cs - this->size();
            return d;
        }

        int CAircraftModelList::removeModelsWithString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity)
        {
            int cs = this->size();
            (*this) = (findByNotInModelStrings(modelStrings, sensitivity));
            int d = cs - this->size();
            return d;
        }

        CAircraftModelList CAircraftModelList::findModelsStartingWith(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return model.getModelString().startsWith(modelString, sensitivity);
            });
        }

        CAircraftModelList CAircraftModelList::findByModelStrings(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return modelStrings.contains(model.getModelString(), sensitivity);
            });
        }

        CAircraftModelList CAircraftModelList::findByNotInModelStrings(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return !modelStrings.contains(model.getModelString(), sensitivity);
            });
        }

        QStringList CAircraftModelList::getSortedModelStrings() const
        {
            QStringList ms;
            for (const CAircraftModel &model : (*this))
            {
                ms.append(model.getModelString());
            }
            ms.sort(Qt::CaseInsensitive);
            return ms;
        }

    } // namespace
} // namespace
