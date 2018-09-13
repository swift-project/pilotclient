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
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/compare.h"
#include "blackmisc/iterator.h"
#include "blackmisc/range.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"

#include <QStringBuilder>
#include <QJsonValue>
#include <QList>
#include <QMultiMap>
#include <QMap>
#include <tuple>

using namespace BlackMisc::Network;
using namespace BlackMisc::Math;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

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

        bool CAircraftModelList::containsAirlineLivery(const CAirlineIcaoCode &airline) const
        {
            if (!airline.hasValidDesignator()) { return false; }
            return this->contains(&CAircraftModel::getAirlineIcaoCode, airline);
        }

        CAircraftModelList CAircraftModelList::findByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return model.matchesModelString(modelString, sensitivity);
            });
        }

        CAircraftModelList CAircraftModelList::findEmptyModelStrings() const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return !model.hasModelString();
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

        CAircraftModelList CAircraftModelList::findByModelMode(CAircraftModel::ModelMode mode) const
        {
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return (model.getModelMode() == mode);
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

        CAircraftModelList CAircraftModelList::findModelsWithoutExistingFile() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return !model.hasExistingCorrespondingFile();
            });
        }

        CAircraftModelList CAircraftModelList::findModelsWithExistingFile() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return model.hasExistingCorrespondingFile();
            });
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

        int CAircraftModelList::setModelType(CAircraftModel::ModelType type)
        {
            int c = 0;
            for (CAircraftModel &model : (*this))
            {
                if (model.getModelType() == type) { continue; }
                model.setModelType(type);
                c++;
            }
            return c;
        }

        int CAircraftModelList::setCG(const CLength &cg)
        {
            int c = 0;
            for (CAircraftModel &model : (*this))
            {
                if (model.getCG() == cg) { continue; }
                model.setCG(cg);
                c++;
            }
            return c;
        }

        int CAircraftModelList::keepModelsWithString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity)
        {
            const int cs = this->size();
            (*this) = (findByModelStrings(modelStrings, sensitivity));
            const int d = cs - this->size();
            return d;
        }

        int CAircraftModelList::removeModelsWithString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity)
        {
            const int cs = this->size();
            (*this) = (this->findByNotInModelStrings(modelStrings, sensitivity));
            const int d = cs - this->size();
            return d;
        }

        int CAircraftModelList::removeIfNotMatchingSimulator(const CSimulatorInfo &needToMatch)
        {
            if (this->isEmpty()) { return 0; }
            const int oldSize = this->size();
            CAircraftModelList models;
            for (const CAircraftModel &model : *this)
            {
                if (model.matchesSimulator(needToMatch))
                {
                    models.push_back(model);
                }
            }
            const int diff = models.size() - oldSize;
            if (diff > 0) { *this = models; }
            return diff;
        }

        int CAircraftModelList::removeAllWithoutModelString()
        {
            if (this->isEmpty()) { return 0; }
            const int s = this->size();
            CAircraftModelList withModelStr;
            for (const CAircraftModel &model : *this)
            {
                if (!model.hasModelString()) { continue; }
                withModelStr.push_back(model);
            }
            const int diff =  s - withModelStr.size();
            if (diff < 1) { return 0; }
            *this = withModelStr;
            return diff;
        }

        int CAircraftModelList::removeIfExcluded()
        {
            if (this->isEmpty()) { return 0; }
            const int s = this->size();
            CAircraftModelList onlyIncluded;
            for (const CAircraftModel &model : *this)
            {
                if (model.getModelMode() == CAircraftModel::Exclude) { continue; }
                onlyIncluded.push_back(model);
            }
            const int diff =  s - onlyIncluded.size();
            if (diff < 1) { return 0; }
            *this = onlyIncluded;
            return diff;
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

        int CAircraftModelList::countVtolAircraft() const
        {
            int count = 0;
            for (const CAircraftModel &model : (*this))
            {
                if (model.isVtol()) { count++; }
            }
            return count;
        }

        int CAircraftModelList::countMilitaryAircraft() const
        {
            int count = 0;
            for (const CAircraftModel &model : (*this))
            {
                if (model.isMilitary()) { count++; }
            }
            return count;
        }

        int CAircraftModelList::countCivilianAircraft() const
        {
            int count = 0;
            for (const CAircraftModel &model : (*this))
            {
                if (model.isCivilian()) { count++; }
            }
            return count;
        }

        int CAircraftModelList::countDifferentAirlines() const
        {
            return this->getAirlineVDesignators().size();
        }

        int CAircraftModelList::countCombinedTypes() const
        {
            return this->getCombinedTypes().size();
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

        CAircraftIcaoCodeList CAircraftModelList::getAircraftIcaoCodesFromDb() const
        {
            if (this->isEmpty()) { return CAircraftIcaoCodeList(); }
            QSet<int> keys;
            CAircraftIcaoCodeList icaos;
            for (const CAircraftModel &model : *this)
            {
                const CAircraftIcaoCode icao = model.getAircraftIcaoCode();
                if (!icao.hasValidDbKey()) { continue; }

                const int key = icao.getDbKey();
                if (keys.contains(key)) { continue; }
                icaos.push_back(icao);
                keys.insert(key);
            }
            return icaos;
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

        QSet<QString> CAircraftModelList::getAircraftDesignatorsForAirline(const CAirlineIcaoCode &airlineCode) const
        {
            QSet<QString> designators;
            if (!airlineCode.hasValidDesignator()) { return designators; }
            for (const CAircraftModel &model : *this)
            {
                if (model.getAirlineIcaoCode() != airlineCode) { continue; }
                designators.insert(model.getAircraftIcaoCodeDesignator());
            }
            return designators;
        }

        CAircraftIcaoCodeList CAircraftModelList::getAicraftIcaoCodesForAirline(const CAirlineIcaoCode &airlineCode) const
        {
            CAircraftIcaoCodeList icaos;
            if (!airlineCode.hasValidDesignator()) { return icaos; }
            for (const CAircraftModel &model : *this)
            {
                if (model.getAirlineIcaoCode() != airlineCode) { continue; }
                icaos.push_back(model.getAircraftIcaoCode());
            }
            return icaos;
        }

        CAirlineIcaoCodeList CAircraftModelList::getAirlineIcaoCodesFromDb() const
        {
            if (this->isEmpty()) { return CAirlineIcaoCodeList(); }
            QSet<int> keys;
            CAirlineIcaoCodeList icaos;
            for (const CAircraftModel &model : *this)
            {
                const CAirlineIcaoCode icao = model.getAirlineIcaoCode();
                if (!icao.hasValidDbKey()) { continue; }

                const int key = icao.getDbKey();
                if (keys.contains(key)) { continue; }
                icaos.push_back(icao);
                keys.insert(key);
            }
            return icaos;
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

        QSet<QString> CAircraftModelList::getCombinedTypes() const
        {
            QSet<QString> combinedCodes;
            for (const CAircraftModel &model : *this)
            {
                const QString ct = model.getAircraftIcaoCode().getCombinedType();
                if (ct.isEmpty()) { continue; }
                combinedCodes.insert(ct);
            }
            return combinedCodes;
        }

        QString CAircraftModelList::getCombinedTypesAsString(const QString &separator) const
        {
            return this->getCombinedTypes().values().join(separator);
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
            CMatchingUtils::addLogDetailsToList(log, remoteModel.getCallsign(), QString("--- Start scoring in list with %1 models").arg(this->size()));
            CMatchingUtils::addLogDetailsToList(log, remoteModel.getCallsign(), this->coverageSummaryForModel(remoteModel));

            int c = 1;
            for (const CAircraftModel &model : *this)
            {
                CStatusMessageList subMsgs;
                const int score = model.calculateScore(remoteModel, preferColorLiveries, log ? &subMsgs : nullptr);
                if (ignoreZeroScores && score < 1) { continue; }

                static const QString bMsg("--- Calculating #%1 '%2'---");
                static const QString eMsg("--- End calculating #%1 ---");
                CMatchingUtils::addLogDetailsToList(log, remoteModel.getCallsign(), bMsg.arg(c).arg(model.getModelStringAndDbKey()));
                if (log) { log->push_back(subMsgs); }
                CMatchingUtils::addLogDetailsToList(log, remoteModel.getCallsign(), eMsg.arg(c));
                c++;
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

        void CAircraftModelList::convertFromMemoizedJson(const QJsonObject &json, bool fallbackToConvertToJson)
        {
            clear();
            QJsonValue value = json.value("containerbase");
            if (value.isUndefined()) { throw CJsonException("Missing 'containerbase'"); }
            QJsonArray array = value.toArray();

            CAircraftModel::MemoHelper::CUnmemoizer helper;
            const QJsonValue aircraftIcaos = json.value("aircraftIcaos");
            const QJsonValue liveries = json.value("liveries");
            const QJsonValue distributors = json.value("distributors");

            const bool undefAc = aircraftIcaos.isUndefined();
            const bool undefLiv = liveries.isUndefined();
            const bool undefDist = distributors.isUndefined();
            const bool undefAll = undefAc && undefDist && undefLiv;

            if (fallbackToConvertToJson && undefAll)
            {
                this->convertFromJson(json);
                return;
            }
            else
            {
                if (undefAc)   { throw CJsonException("Missing 'aircraftIcaos'"); }
                if (undefLiv)  { throw CJsonException("Missing 'liveries'"); }
                if (undefDist) { throw CJsonException("Missing 'distributors'"); }
            }

            // convert
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
                this->push_back(value);
            }
        }

        QJsonArray CAircraftModelList::toDatabaseJson() const
        {
            QJsonArray array;
            for (const CAircraftModel &model : *this)
            {
                CAircraftModel copy(model);
                copy.normalizeFileNameForDb(); // strip full path
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
                html += html.isEmpty() ?
                        model.asHtmlSummary(" ") :
                        QStringLiteral("<br>") % model.asHtmlSummary(" ");
            }
            return html;
        }

        QString CAircraftModelList::coverageSummary(const QString &separator) const
        {
            if (this->isEmpty()) { return "no models"; } // avoid division by 0

            const int dbEntries = this->countWithValidDbKey();
            const double dbRatio = CMathUtils::round(static_cast<double>(100 * dbEntries) / this->size(), 1);
            return
                QStringLiteral("Entries: ") % QString::number(this->size()) %
                QStringLiteral(" | valid DB keys: ") % QString::number(dbEntries) %
                QStringLiteral(" (") % QString::number(dbRatio) % QStringLiteral("%)") % separator %
                QStringLiteral("color liveries: ") % QString::number(this->countModelsWithColorLivery()) %
                QStringLiteral(" | airline liveries: ") % QString::number(this->countModelsWithAirlineLivery()) % separator %
                QStringLiteral("VTOL: ") % QString::number(this->countVtolAircraft()) %
                QStringLiteral(" | military: ") % QString::number(this->countMilitaryAircraft()) %
                QStringLiteral(" | civilian: ") % QString::number(this->countCivilianAircraft()) % separator %
                QStringLiteral("Different airlines: ") % QString::number(this->countDifferentAirlines()) % separator %
                QStringLiteral("Combined types: '") % this->getCombinedTypesAsString() % QStringLiteral("'") % separator %
                QStringLiteral("Simulators: ") % this->countPerSimulator().toQString();
        }

        QString CAircraftModelList::coverageSummaryForModel(const CAircraftModel &checkModel, const QString &separator) const
        {
            const bool combinedCodeForModel = this->containsCombinedType(checkModel.getAircraftIcaoCode().getCombinedType());
            const bool airlineForModel = checkModel.hasAirlineDesignator() && this->containsAirlineLivery(checkModel.getAirlineIcaoCode());
            return coverageSummary(separator) % separator %
                   QStringLiteral("Data for input model, has combined: ") % boolToYesNo(combinedCodeForModel) %
                   (
                       checkModel.hasAirlineDesignator() ?
                       QStringLiteral(" airline '") % checkModel.getAirlineIcaoCodeDesignator() % QStringLiteral("': ") % boolToYesNo(airlineForModel) :
                       QStringLiteral("")
                   );
        }

        QString CAircraftModelList::htmlStatistics(bool aircraftStats, bool airlineStats) const
        {
            static const QString sep("<br>");
            const bool notOnlyDb = this->containsAnyObjectWithoutKey();
            QString stats = this->coverageSummary(sep);
            if (aircraftStats)
            {
                const CAircraftIcaoCodeList icaos = this->getAircraftIcaoCodesFromDb();
                QStringList designators(icaos.allDesignators().toList());
                designators.sort();
                stats += sep % sep %
                         QStringLiteral("Aircraft ICAOs from DB: ") % sep %
                         designators.join(", ");
            }

            if (airlineStats)
            {
                const CAirlineIcaoCodeList icaos = this->getAirlineIcaoCodesFromDb();
                const QStringList designators = icaos.toIcaoDesignatorCompleterStrings();
                stats += sep % sep %
                         QStringLiteral("Airline ICAOs from DB: ") % sep %
                         designators.join(", ");
            }

            if (notOnlyDb)
            {
                const CAircraftModelList dbModels = this->findObjectsWithDbKey();
                stats += sep %
                         sep %
                         QStringLiteral("DB objects:<br>---------") %
                         sep %
                         dbModels.htmlStatistics(false, false);
            }
            return stats;
        }

        CAircraftModelList CAircraftModelList::fromDatabaseJsonCaching(const QJsonArray &array)
        {
            AircraftIcaoIdMap aircraftIcaos;
            LiveryIdMap       liveries;
            DistributorIdMap  distributors;

            CAircraftModelList models;
            for (const QJsonValue &value : array)
            {
                models.push_back(CAircraftModel::fromDatabaseJsonCaching(value.toObject(), aircraftIcaos, liveries, distributors));
            }
            return models;
        }
    } // namespace
} // namespace
