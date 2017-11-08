/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/matchingutils.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/compare.h"
#include "blackmisc/iterator.h"
#include "blackmisc/range.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"

#include <QJsonValue>
#include <QList>
#include <QMultiMap>
#include <tuple>

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

        bool CAircraftModelList::containsModelStringOrDbKey(const CAircraftModel &model, Qt::CaseSensitivity sensitivity) const
        {
            for (const CAircraftModel &m : (*this))
            {
                if (m.hasValidDbKey() && m.getDbKey() == model.getDbKey()) { return true; }
                if (m.matchesModelString(model.getModelString(), sensitivity)) { return true; }
            }
            return false;
        }

        bool CAircraftModelList::containsCallsign(const CCallsign &callsign) const
        {
            return this->contains(&CAircraftModel::getCallsign, callsign);
        }

        bool CAircraftModelList::containsCombinedType(const QString &combinedType) const
        {
            if (combinedType.isEmpty()) { return false; }
            const QString ct(combinedType.toUpper().trimmed());
            return this->containsBy([ & ](const CAircraftModel & model)
            {
                return model.getAircraftIcaoCode().getCombinedType() == ct;
            });
        }

        bool CAircraftModelList::containsModelsWithAircraftIcaoDesignator(const QString &aircraftDesignator) const
        {
            return this->contains(&CAircraftModel::getAircraftIcaoCodeDesignator, aircraftDesignator);
        }

        bool CAircraftModelList::containsModelsWithAircraftAndAirlineIcaoDesignator(const QString &aircraftDesignator, const QString &airlineDesignator) const
        {
            return this->contains(&CAircraftModel::getAircraftIcaoCodeDesignator, aircraftDesignator, &CAircraftModel::getAirlineIcaoCodeDesignator, airlineDesignator);
        }

        CAircraftModelList CAircraftModelList::findByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return model.matchesModelString(modelString, sensitivity);
            });
        }

        CAircraftModel CAircraftModelList::findFirstByModelStringOrDefault(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            if (modelString.isEmpty()) { return CAircraftModel(); }
            return this->findFirstByOrDefault([ & ](const CAircraftModel & model)
            {
                return model.matchesModelString(modelString, sensitivity);
            });
        }

        CAircraftModel CAircraftModelList::findFirstByCallsignOrDefault(const CCallsign &callsign) const
        {
            if (callsign.isEmpty()) { return CAircraftModel(); }
            return this->findFirstByOrDefault([ & ](const CAircraftModel & model)
            {
                return model.getCallsign() == callsign;
            });
        }

        CAircraftModelList CAircraftModelList::findByIcaoDesignators(const CAircraftIcaoCode &aircraftIcaoCode, const CAirlineIcaoCode &airlineIcaoCode) const
        {
            const QString aircraft(aircraftIcaoCode.getDesignator());
            const QString airline(airlineIcaoCode.getDesignator());

            if (airline.isEmpty())
            {
                return this->findBy([ & ](const CAircraftModel & model)
                {
                    return model.getAircraftIcaoCode().getDesignator() == aircraft;
                });
            }
            if (aircraft.isEmpty())
            {
                return this->findBy([ & ](const CAircraftModel & model)
                {
                    return model.getAirlineIcaoCode().getDesignator() == airline;
                });
            }
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return model.getAirlineIcaoCode().getDesignator() == airline &&
                       model.getAircraftIcaoCode().getDesignator() == aircraft;
            });
        }

        CAircraftModelList CAircraftModelList::findByAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const
        {
            if (aircraftDesignator.isEmpty()) { return CAircraftModelList(); }
            return this->findBy([ & ](const CAircraftModel & model)
            {
                if (!model.getAircraftIcaoCode().matchesDesignator(aircraftDesignator)) { return false; }
                return model.getLivery().matchesCombinedCode(combinedCode);
            });
        }

        CAircraftModelList CAircraftModelList::findByLiveryCode(const CLivery &livery) const
        {
            if (!livery.hasCombinedCode()) { return CAircraftModelList(); }
            const QString code(livery.getCombinedCode());
            return this->findBy([ & ](const CAircraftModel & model)
            {
                if (!model.getLivery().hasCombinedCode()) return false;
                return model.getLivery().getCombinedCode() == code;
            });
        }

        CAircraftModelList CAircraftModelList::findWithFileName() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return model.hasFileName();
            });
        }

        CAircraftModelList CAircraftModelList::findWithAircraftDesignator() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return model.hasAircraftDesignator();
            });
        }

        CAircraftModelList CAircraftModelList::findWithAircraftDesignator(const QSet<QString> &designators) const
        {
            if (designators.isEmpty()) { return CAircraftModelList(); }
            return this->findBy([&](const CAircraftModel & model)
            {
                return designators.contains(model.getAircraftIcaoCodeDesignator());
            });
        }

        CAircraftModelList CAircraftModelList::findWithKnownAircraftDesignator() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return model.hasKnownAircraftDesignator();
            });
        }

        CAircraftModelList CAircraftModelList::findByManufacturer(const QString &manufacturer) const
        {
            if (manufacturer.isEmpty()) { return CAircraftModelList(); }
            const QString m(manufacturer.toUpper().trimmed());
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return model.getAircraftIcaoCode().getManufacturer() == m;
            });
        }

        CAircraftModelList CAircraftModelList::findByFamily(const QString &family) const
        {
            if (family.isEmpty()) { return CAircraftModelList(); }
            const QString f(family.toUpper().trimmed());
            return this->findBy([ & ](const CAircraftModel & model)
            {
                const CAircraftIcaoCode icao(model.getAircraftIcaoCode());
                if (!icao.hasFamily()) { return false; }
                return icao.getFamily() == f;
            });
        }

        CAircraftModelList CAircraftModelList::findByFamilyWithColorLivery(const QString &family) const
        {
            if (family.isEmpty()) { return CAircraftModelList(); }
            const QString f(family.toUpper().trimmed());
            return this->findBy([ & ](const CAircraftModel & model)
            {
                if (!model.getLivery().isColorLivery()) { return false; }
                const CAircraftIcaoCode icao(model.getAircraftIcaoCode());
                if (!icao.hasFamily()) { return false; }
                return icao.getFamily() == f;
            });
        }

        CAircraftModelList CAircraftModelList::findByDesignatorOrFamilyWithColorLivery(const CAircraftIcaoCode &icao) const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                if (!model.getLivery().isColorLivery()) { return false; }
                const CAircraftIcaoCode modelIcao(model.getAircraftIcaoCode());
                if (modelIcao.getDesignator() == icao.getDesignator()) { return true; }
                return icao.hasFamily() && modelIcao.getFamily() == icao.getFamily();
            });
        }

        CAircraftModelList CAircraftModelList::findByCombinedType(const QString &combinedType) const
        {
            const QString cc(combinedType.trimmed().toUpper());
            if (combinedType.length() != 3) { return CAircraftModelList(); }
            return this->findBy([ & ](const CAircraftModel & model)
            {
                const CAircraftIcaoCode icao(model.getAircraftIcaoCode());
                return icao.matchesCombinedType(cc);
            });
        }

        CAircraftModelList CAircraftModelList::findByCombinedTypeAndWtc(const QString &combinedType, const QString &wtc) const
        {
            const CAircraftModelList ml = this->findByCombinedType(combinedType);
            if (ml.isEmpty()) { return ml; }
            const QString wtcUc(wtc.toUpper().trimmed());
            return this->findBy([ & ](const CAircraftModel & model)
            {
                const CAircraftIcaoCode icao(model.getAircraftIcaoCode());
                return icao.getWtc() == wtcUc;
            });
        }

        CAircraftModelList CAircraftModelList::findByCombinedTypeWithColorLivery(const QString &combinedType) const
        {
            return this->findByCombinedType(combinedType).findColorLiveries();
        }

        CAircraftModelList CAircraftModelList::findByCombinedTypeAndWtcWithColorLivery(const QString &combinedType, const QString &wtc) const
        {
            return this->findByCombinedTypeAndWtc(combinedType, wtc).findColorLiveries();
        }

        CAircraftModelList CAircraftModelList::findColorLiveries() const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return model.getLivery().isColorLivery();
            });
        }

        CAircraftModelList CAircraftModelList::findByMilitaryFlag(bool military) const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return (model.isMilitary() == military);
            });
        }

        CAircraftModelList CAircraftModelList::findByVtolFlag(bool vtol) const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return (model.isVtol() == vtol);
            });
        }

        CAircraftModelList CAircraftModelList::getAllFsFamilyModels() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return model.getSimulator().isMicrosoftOrPrepare3DSimulator();
            });
        }

        CAircraftModelList CAircraftModelList::getAllIncludedModels() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return model.getModelMode() == CAircraftModel::Include;
            });
        }

        QString CAircraftModelList::findModelIconPathByModelString(const QString &modelString) const
        {
            if (modelString.isEmpty()) { return ""; }
            const CAircraftModel m(findFirstByModelStringOrDefault(modelString, Qt::CaseInsensitive));
            return m.getIconPath();
        }

        QString CAircraftModelList::findModelIconPathByCallsign(const CCallsign &callsign) const
        {
            if (callsign.isEmpty()) { return ""; }
            const CAircraftModel m(findFirstByCallsignOrDefault(callsign));
            return m.getIconPath();
        }

        QString CAircraftModelList::designatorToFamily(const CAircraftIcaoCode &aircraftIcaoCode) const
        {
            if (aircraftIcaoCode.hasFamily()) { return aircraftIcaoCode.getFamily(); }
            for (const CAircraftModel &model : (*this))
            {
                const CAircraftIcaoCode icao(model.getAircraftIcaoCode());
                if (!icao.hasFamily()) continue;
                if (icao.matchesDesignator(aircraftIcaoCode.getDesignator()))
                {
                    return icao.getFamily();
                }
            }
            return QString();
        }

        CAircraftModelList CAircraftModelList::matchesSimulator(const CSimulatorInfo &simulator) const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return model.matchesSimulator(simulator);
            });
        }

        bool CAircraftModelList::containsMatchingSimulator(const CSimulatorInfo &simulators) const
        {
            return this->containsBy([ & ](const CAircraftModel & model)
            {
                return model.matchesSimulator(simulators);
            });
        }

        bool CAircraftModelList::containsNotMatchingSimulator(const CSimulatorInfo &simulators) const
        {
            return this->containsBy([ & ](const CAircraftModel & model)
            {
                return !model.matchesSimulator(simulators);
            });
        }

        bool CAircraftModelList::containsMilitary() const
        {
            return this->containsBy([ & ](const CAircraftModel & model)
            {
                return model.isMilitary();
            });
        }

        bool CAircraftModelList::containsCivilian() const
        {
            return this->containsBy([ & ](const CAircraftModel & model)
            {
                return model.isCivilian();
            });
        }

        bool CAircraftModelList::containsCivilianAndMilitary() const
        {
            return this->containsMilitary() && this->containsCivilian();
        }

        bool CAircraftModelList::containsVtol() const
        {
            return this->containsBy([ & ](const CAircraftModel & model)
            {
                return model.isVtol();
            });
        }

        CAircraftModelList CAircraftModelList::findByDistributors(const CDistributorList &distributors) const
        {
            if (distributors.isEmpty()) { return CAircraftModelList(); }
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return model.matchesAnyDbDistributor(distributors);
            });
        }

        int CAircraftModelList::setSimulatorInfo(const CSimulatorInfo &info)
        {
            int c = 0;
            const CSimulatorInfo::Simulator s = info.getSimulator();
            for (CAircraftModel &model : (*this))
            {
                if (model.getSimulator().getSimulator() == s) { continue; }
                model.setSimulator(info);
                c++;
            }
            return c;
        }

        CSimulatorInfo CAircraftModelList::simulatorsSupported() const
        {
            CSimulatorInfo::Simulator s = CSimulatorInfo::None;
            for (const CAircraftModel &model : (*this))
            {
                s |= model.getSimulator().getSimulator();
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

        int CAircraftModelList::removeIfNotMatchingSimulator(const CSimulatorInfo &needToMatch)
        {
            if (this->isEmpty()) { return 0; }
            int c = 0;
            for (auto it = this->begin(); it != this->end();)
            {
                if (it->matchesSimulator(needToMatch))
                {
                    ++it;
                }
                else
                {
                    c++;
                    it = this->erase(it);
                }
            }
            return c;
        }

        int CAircraftModelList::removeAllWithoutModelString()
        {
            if (this->isEmpty()) { return 0; }
            int c = 0;
            for (auto it = this->begin(); it != this->end();)
            {
                if (it->hasModelString())
                {
                    ++it;
                }
                else
                {
                    c++;
                    it = this->erase(it);
                }
            }
            return c;
        }

        int CAircraftModelList::removeIfExcluded()
        {
            if (this->isEmpty()) { return 0; }
            int c = 0;
            for (auto it = this->begin(); it != this->end();)
            {
                if (it->getModelMode() != CAircraftModel::Exclude)
                {
                    ++it;
                }
                else
                {
                    c++;
                    it = this->erase(it);
                }
            }
            return c;
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
            const QStringList keys(addOrReplaceList.getModelStringList(false));
            newModels.removeModelsWithString(keys, sensitivity);
            int removed = newModels.size();  // size after removing
            newModels.push_back(addOrReplaceList);
            *this = newModels;
            return this->size() - removed;
        }

        CAircraftModelList CAircraftModelList::findModelsStartingWith(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return model.getModelString().startsWith(modelString, sensitivity);
            });
        }

        CAircraftModelList CAircraftModelList::findByModelStrings(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return modelStrings.contains(model.getModelString(), sensitivity);
            });
        }

        CAircraftModelList CAircraftModelList::findByNotInModelStrings(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([&](const CAircraftModel & model)
            {
                const bool c = modelStrings.contains(model.getModelString(), sensitivity);
                return !c;
            });
        }

        QStringList CAircraftModelList::getModelStringList(bool sort) const
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

        QSet<QString> CAircraftModelList::getModelStringSet() const
        {
            QSet<QString> ms;
            for (const CAircraftModel &model : (*this))
            {
                if (!model.hasModelString()) { continue; }
                ms.insert(model.getModelString());
            }
            return ms;
        }

        CCountPerSimulator CAircraftModelList::countPerSimulator() const
        {
            CCountPerSimulator count;
            for (const CAircraftModel &model : (*this))
            {
                count.increaseSimulatorCounts(model.getSimulator());
            }
            return count;
        }

        CSimulatorInfo CAircraftModelList::simulatorsWithMaxEntries() const
        {
            if (this->isEmpty()) { return CSimulatorInfo(); } // not known
            const CCountPerSimulator counts(this->countPerSimulator());
            const int simulatorsRepresented = counts.simulatorsRepresented();
            if (simulatorsRepresented < 1)
            {
                return CSimulatorInfo();
            }
            const QMultiMap<int, CSimulatorInfo> cps(counts.countPerSimulator());
            CSimulatorInfo maxSim = cps.last();
            if (simulatorsRepresented > 0)
            {
                const int count = cps.lastKey(); // how many elements
                const QList<CSimulatorInfo> infoWithMaxValues = cps.values(count); // all with the same counts
                for (const CSimulatorInfo &info : infoWithMaxValues)
                {
                    maxSim.addSimulator(info);
                }
            }
            return maxSim;
        }

        int CAircraftModelList::countModelsWithColorLivery() const
        {
            int count = 0;
            for (const CAircraftModel &model : (*this))
            {
                if (model.getLivery().isColorLivery()) { count++; }
            }
            return count;
        }

        int CAircraftModelList::countModelsWithAirlineLivery() const
        {
            int count = 0;
            for (const CAircraftModel &model : (*this))
            {
                if (model.getLivery().isAirlineLivery()) { count++; }
            }
            return count;
        }

        void CAircraftModelList::updateDistributor(const CDistributor &distributor)
        {
            for (CAircraftModel &model : *this)
            {
                model.setDistributor(distributor);
            }
        }

        CDistributorList CAircraftModelList::getDistributors(bool onlyDbDistributors) const
        {
            if (this->isEmpty()) { return CDistributorList(); }
            CDistributorList distributors;
            for (const CAircraftModel &model : *this)
            {
                const CDistributor d(model.getDistributor());
                if (onlyDbDistributors && !d.hasValidDbKey()) { continue; }
                if (distributors.contains(d)) { continue; }
                distributors.push_back(d);
            }
            return distributors;
        }

        QSet<QString> CAircraftModelList::getAircraftDesignators() const
        {
            QSet<QString> designators;
            for (const CAircraftModel &model : *this)
            {
                if (!model.hasAircraftDesignator()) { continue; }
                designators.insert(model.getAircraftIcaoCodeDesignator());
            }
            return designators;
        }

        QSet<QString> CAircraftModelList::getAirlineDesignators() const
        {
            QSet<QString> designators;
            for (const CAircraftModel &model : *this)
            {
                if (!model.hasAirlineDesignator()) { continue; }
                designators.insert(model.getAirlineIcaoCodeDesignator());
            }
            return designators;
        }

        QSet<QString> CAircraftModelList::getAirlineVDesignators() const
        {
            QSet<QString> designators;
            for (const CAircraftModel &model : *this)
            {
                if (!model.hasAirlineDesignator()) { continue; }
                designators.insert(model.getAirlineIcaoCodeVDesignator());
            }
            return designators;
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

        int CAircraftModelList::updateDistributorOrder(const CDistributorList &distributors)
        {
            if (distributors.isEmpty()) { return 0; }
            int found = 0;
            for (CAircraftModel &model : *this)
            {
                if (model.setDistributorOrder(distributors)) { found ++; }
            }
            return found;
        }

        void CAircraftModelList::normalizeFileNamesForDb()
        {
            for (CAircraftModel &model : *this)
            {
                model.normalizeFileNameForDb();
            }
        }

        ScoredModels CAircraftModelList::scoreFull(const CAircraftModel &remoteModel, bool preferColorLiveries, bool ignoreZeroScores, CStatusMessageList *log) const
        {
            ScoredModels scoreMap;

            // normally prefer colors if there is no airline
            CMatchingUtils::addLogDetailsToList(log, remoteModel.getCallsign(), QString("Prefer color liveries: '%1', airline: '%2', ignore zero scores: '%3'").arg(boolToYesNo(preferColorLiveries), remoteModel.getAirlineIcaoCodeDesignator(), boolToYesNo(ignoreZeroScores)));
            CMatchingUtils::addLogDetailsToList(log, remoteModel.getCallsign(), QString("--- Start scoring in list with %1 models, airline liveries: %2, color liveries: %3").arg(this->size()).arg(this->countModelsWithAirlineLivery()).arg(this->countModelsWithColorLivery()));

            int c = 1;
            for (const CAircraftModel &model : *this)
            {
                CStatusMessageList subMsgs;
                const int score = model.calculateScore(remoteModel, preferColorLiveries, log ? &subMsgs : nullptr);
                if (ignoreZeroScores && score < 1) { continue; }

                CMatchingUtils::addLogDetailsToList(log, remoteModel.getCallsign(), QString("--- Calculating #%1 '%2'---").arg(c++).arg(model.getModelStringAndDbKey()));
                if (log) { log->push_back(subMsgs); }
                CMatchingUtils::addLogDetailsToList(log, remoteModel.getCallsign(), QString("--- End calculating #%1 ---").arg(c));

                scoreMap.insertMulti(score, model);
            }
            CMatchingUtils::addLogDetailsToList(log, remoteModel.getCallsign(), QStringLiteral("--- End scoring ---"));
            return scoreMap;
        }

        QStringList CAircraftModelList::toCompleterStrings(bool sorted, const CSimulatorInfo &simulator) const
        {
            QStringList c;
            for (const CAircraftModel &model : *this)
            {
                if (model.getSimulator().matchesAny(simulator))
                {
                    c.append(model.getModelString());
                }
            }
            if (sorted) { c.sort(); }
            return c;
        }

        CStatusMessageList CAircraftModelList::validateForPublishing() const
        {
            CAircraftModelList invalidModels;
            CAircraftModelList validModels;
            return validateForPublishing(validModels, invalidModels);
        }

        CStatusMessageList CAircraftModelList::validateForPublishing(CAircraftModelList &validModels, CAircraftModelList &invalidModels) const
        {
            if (this->isEmpty()) { return CStatusMessageList(); }
            CStatusMessageList msgs;
            for (const CAircraftModel &model : *this)
            {
                const CStatusMessageList msgsModel(model.validate(false));
                const CStatusMessage msgModel(msgsModel.toSingleMessage());

                QStringList subMsgs;
                if (!model.getDistributor().hasValidDbKey()) { subMsgs << "No distributor from DB"; }
                if (!model.getAircraftIcaoCode().hasValidDbKey()) { subMsgs << "No aircraft ICAO from DB"; }
                if (!model.getLivery().hasValidDbKey()) { subMsgs << "No livery from DB"; }
                if (model.getLivery().isAirlineLivery())
                {
                    // for color codes we do not need to check
                    if (!model.getLivery().getAirlineIcaoCode().hasValidDbKey()) { subMsgs << "No airline ICAO from DB"; }
                }

                const CStatusMessage msgDb(CStatusMessage::SeverityError, subMsgs.join(", "));
                CStatusMessage singleMsg(CStatusMessageList({msgModel, msgDb}).toSingleMessage());
                if (!singleMsg.isWarningOrAbove())
                {
                    validModels.push_back(model);
                    continue;
                }
                if (model.hasModelString())
                {
                    singleMsg.prependMessage(model.getModelString() + ": ");
                }
                msgs.push_back(singleMsg);
                invalidModels.push_back(model);
            }
            return msgs;
        }

        CStatusMessageList CAircraftModelList::validateDistributors(const CDistributorList &distributors, CAircraftModelList &validModels, CAircraftModelList &invalidModels) const
        {
            CStatusMessageList msgs;
            CDistributorList distributorsFromDb(distributors);
            distributorsFromDb.removeIfNotLoadedFromDb();

            // Any DB distributors?
            if (distributorsFromDb.isEmpty())
            {
                const CStatusMessage msg = CStatusMessage(this).validationError("No DB distributors for validation");
                msgs.push_back(msg);
                invalidModels.push_back(*this);
                return msgs;
            }

            for (const CAircraftModel &model : *this)
            {
                if (model.hasDbDistributor() || model.matchesAnyDbDistributor(distributorsFromDb))
                {
                    validModels.push_back(model);
                }
                else
                {
                    const CStatusMessage msg = CStatusMessage(this).validationError("No valid distributor for '%1', was '%2'") << model.getModelString() << model.getDistributor().getDbKey();
                    msgs.push_back(msg);
                    invalidModels.push_back(model);
                }
            }
            return msgs;
        }

        QJsonObject CAircraftModelList::toMemoizedJson() const
        {
            CAircraftModel::MemoHelper::CMemoizer helper;
            QJsonArray array;
            for (auto it = cbegin(); it != cend(); ++it)
            {
                array << it->toMemoizedJson(helper);
            }
            QJsonObject json;
            json.insert("containerbase", array);
            json.insert("aircraftIcaos", helper.getTable<CAircraftIcaoCode>().toJson());
            json.insert("liveries", helper.getTable<CLivery>().toJson());
            json.insert("distributors", helper.getTable<CDistributor>().toJson());
            return json;
        }

        void CAircraftModelList::convertFromMemoizedJson(const QJsonObject &json)
        {
            clear();
            QJsonValue value = json.value("containerbase");
            if (value.isUndefined()) { throw CJsonException("Missing 'containerbase'"); }
            QJsonArray array = value.toArray();

            CAircraftModel::MemoHelper::CUnmemoizer helper;
            QJsonValue aircraftIcaos = json.value("aircraftIcaos");
            QJsonValue liveries = json.value("liveries");
            QJsonValue distributors = json.value("distributors");
            if (aircraftIcaos.isUndefined()) { throw CJsonException("Missing 'aircraftIcaos'"); }
            if (liveries.isUndefined()) { throw CJsonException("Missing 'liveries'"); }
            if (distributors.isUndefined()) { throw CJsonException("Missing 'distributors'"); }
            {
                CJsonScope scope("aircraftIcaos");
                Q_UNUSED(scope);
                helper.getTable<CAircraftIcaoCode>().convertFromJson(aircraftIcaos.toObject());
            }
            {
                CJsonScope scope("liveries");
                Q_UNUSED(scope);
                helper.getTable<CLivery>().convertFromJson(liveries.toObject());
            }
            {
                CJsonScope scope("distributors");
                Q_UNUSED(scope);
                helper.getTable<CDistributor>().convertFromJson(distributors.toObject());
            }

            int index = 0;
            for (auto i = array.begin(); i != array.end(); ++i)
            {
                CJsonScope scope("containerbase", index++);
                Q_UNUSED(scope);
                CAircraftModel value;
                value.convertFromMemoizedJson(i->toObject(), helper);
                insert(value);
            }
        }

        QJsonArray CAircraftModelList::toDatabaseJson() const
        {
            QJsonArray array;
            for (const CAircraftModel &model : *this)
            {
                CAircraftModel copy(model);
                copy.normalizeFileNameForDb();
                QJsonValue v(copy.toDatabaseJson());
                array.append(v);
            }
            return array;
        }

        QString CAircraftModelList::toDatabaseJsonString(QJsonDocument::JsonFormat format) const
        {
            return QJsonDocument(toDatabaseJson()).toJson(format);
        }

        QString CAircraftModelList::asHtmlSummary() const
        {
            if (this->isEmpty()) { return ""; }
            QString html;
            for (const CAircraftModel &model : *this)
            {
                if (!html.isEmpty()) { html += "<br>"; }
                html += model.asHtmlSummary(" ");
            }
            return html;
        }
    } // namespace
} // namespace
