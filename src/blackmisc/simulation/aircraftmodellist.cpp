/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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

        bool CAircraftModelList::containsModelStringOrId(const CAircraftModel &model, Qt::CaseSensitivity sensitivity) const
        {
            for (const CAircraftModel &m : (*this))
            {
                if (m.hasValidDbKey() && m.getDbKey() == model.getDbKey()) { return true; }
                if (m.matchesModelString(model.getModelString(), sensitivity)) { return true; }
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

        CAircraftModel CAircraftModelList::findFirstByModelStringOrDefault(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->findFirstByOrDefault([ = ](const CAircraftModel & model)
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

        CAircraftModelList CAircraftModelList::findWithFileName() const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return model.hasFileName();
            });
        }

        CAircraftModelList CAircraftModelList::findWithAircraftDesignator() const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return model.hasAircraftDesignator();
            });
        }

        CAircraftModelList CAircraftModelList::findWithAircraftDesignator(const QStringList &designators) const
        {
            if (designators.isEmpty()) { return CAircraftModelList(); }
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return designators.contains(model.getAircraftIcaoCodeDesignator());
            });
        }

        CAircraftModelList CAircraftModelList::findWithKnownAircraftDesignator() const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return model.hasKnownAircraftDesignator();
            });
        }

        CAircraftModelList CAircraftModelList::matchesSimulator(const CSimulatorInfo &simulator) const
        {
            const CSimulatorInfo::Simulator s = simulator.getSimulator();
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return (s & model.getSimulatorInfo().getSimulator()) > 0;
            });
        }

        CAircraftModelList CAircraftModelList::findByDistributors(const CDistributorList &distributors) const
        {
            if (distributors.isEmpty()) { return CAircraftModelList(); }
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return model.matchesAnyDistributor(distributors);
            });
        }

        int CAircraftModelList::setSimulatorInfo(const CSimulatorInfo &info)
        {
            int c = 0;
            const CSimulatorInfo::Simulator s = info.getSimulator();
            for (CAircraftModel &model : (*this))
            {
                if (model.getSimulatorInfo().getSimulator() == s) { continue; }
                model.setSimulatorInfo(info);
                c++;
            }
            return c;
        }

        CSimulatorInfo CAircraftModelList::simulatorsSupported() const
        {
            CSimulatorInfo::Simulator s = CSimulatorInfo::None;
            for (const CAircraftModel &model : (*this))
            {
                s |= model.getSimulatorInfo().getSimulator();
                if (s == CSimulatorInfo::All) { break; }
            }
            return CSimulatorInfo(s);
        }

        int CAircraftModelList::setModelMode(CAircraftModel::ModelMode mode)
        {
            int c = 0;
            for (CAircraftModel &model : (*this))
            {
                if (model.getModelMode() == mode) { continue; }
                model.setModelMode(mode);
                c++;
            }
            return c;
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
            (*this) = (this->findByNotInModelStrings(modelStrings, sensitivity));
            int d = cs - this->size();
            return d;
        }

        int CAircraftModelList::replaceOrAddModelsWithString(const CAircraftModelList &addOrReplaceList, Qt::CaseSensitivity sensitivity)
        {
            if (addOrReplaceList.isEmpty()) { return 0; }
            if (this->isEmpty())
            {
                *this = addOrReplaceList;
                return addOrReplaceList.size();
            }
            CAircraftModelList newModels(*this);
            const QStringList keys(addOrReplaceList.getModelStrings(false));
            newModels.removeModelsWithString(keys, sensitivity);
            int removed = newModels.size();  // size after removing
            newModels.push_back(addOrReplaceList);
            *this = newModels;
            return this->size() - removed;
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
                const bool c = modelStrings.contains(model.getModelString(), sensitivity);
                return !c;
            });
        }

        QStringList CAircraftModelList::getModelStrings(bool sort) const
        {
            QStringList ms;
            for (const CAircraftModel &model : (*this))
            {
                if (!model.hasModelString()) { continue; }
                ms.append(model.getModelString());
            }
            if (sort) { ms.sort(Qt::CaseInsensitive); }
            return ms;
        }

        void CAircraftModelList::updateDistributor(const CDistributor &distributor)
        {
            for (CAircraftModel &model : *this)
            {
                model.setDistributor(distributor);
            }
        }

        void CAircraftModelList::updateAircraftIcao(const CAircraftIcaoCode &icao)
        {
            for (CAircraftModel &model : *this)
            {
                model.setAircraftIcaoCode(icao);
            }
        }

        void CAircraftModelList::updateLivery(const CLivery &livery)
        {
            for (CAircraftModel &model : *this)
            {
                model.setLivery(livery);
            }
        }

        CStatusMessageList CAircraftModelList::validateForPublishing() const
        {
            CAircraftModelList invalidModels;
            return validateForPublishing(invalidModels);
        }

        CStatusMessageList CAircraftModelList::validateForPublishing(CAircraftModelList &validateModels) const
        {
            if (this->isEmpty()) { return CStatusMessageList(); }
            CStatusMessageList msgs;
            for (const CAircraftModel &model : *this)
            {
                const CStatusMessageList msgsModel(model.validate(false));
                CStatusMessage msgModel(msgsModel.toSingleMessage());

                QStringList subMsgs;
                if (!model.getDistributor().hasValidDbKey()) { subMsgs << "No distributor from DB"; }
                if (!model.getAircraftIcaoCode().hasValidDbKey()) { subMsgs << "No aircraft ICAO from DB"; }
                if (!model.getLivery().hasValidDbKey()) { subMsgs << "No livery from DB"; }
                if (model.getLivery().isAirlineLivery())
                {
                    // for color codes we do not need to check
                    if (!model.getLivery().getAirlineIcaoCode().hasValidDbKey()) { subMsgs << "No airline ICAO from DB"; }
                }
                CStatusMessage msgDb(CStatusMessage::SeverityError, subMsgs.join(", "));

                CStatusMessage singleMsg(CStatusMessageList({msgModel, msgDb}).toSingleMessage());
                if (!singleMsg.isWarningOrAbove()) { continue; }
                if (model.hasModelString())
                {
                    singleMsg.prependMessage(model.getModelString() + ": ");
                }
                msgs.push_back(singleMsg);
                validateModels.push_back(model);
            }
            return msgs;
        }

        QJsonArray CAircraftModelList::toDatabaseJson() const
        {
            QJsonArray array;
            for (const CAircraftModel &model : *this)
            {
                QJsonValue v(model.toDatabaseJson());
                array.append(v);
            }
            return array;
        }

        QString CAircraftModelList::toDatabaseJsonString(QJsonDocument::JsonFormat format) const
        {
            return QJsonDocument(toDatabaseJson()).toJson(format);
        }
    } // namespace
} // namespace
