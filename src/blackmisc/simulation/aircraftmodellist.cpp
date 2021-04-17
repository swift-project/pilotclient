/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/iterator.h"
#include "blackmisc/range.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"

#include <QStringBuilder>
#include <QJsonValue>
#include <QList>
#include <QMultiMap>
#include <QFileInfo>
#include <QDir>
#include <tuple>

using namespace BlackConfig;
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

        CAircraftModel CAircraftModelList::findFirstByModelStringAliasOrDefault(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            if (modelString.isEmpty()) { return CAircraftModel(); }
            return this->findFirstByOrDefault([ & ](const CAircraftModel & model)
            {
                return model.matchesModelStringOrAlias(modelString, sensitivity);
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

        CAircraftModelList CAircraftModelList::findByAircraftAndAirline(const CAircraftIcaoCode &aircraftIcaoCode, const CAirlineIcaoCode &airlineIcaoCode) const
        {
            return this->findBy(&CAircraftModel::getAircraftIcaoCode, aircraftIcaoCode, &CAircraftModel::getAirlineIcaoCode, airlineIcaoCode);
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

        CAircraftModelList CAircraftModelList::findByAircraftAndLivery(const CAircraftIcaoCode &aircraftIcaoCode, const CLivery &livery) const
        {
            return this->findBy(&CAircraftModel::getAircraftIcaoCode, aircraftIcaoCode, &CAircraftModel::getLivery, livery);
        }

        CAircraftModelList CAircraftModelList::findByAirlineGroup(const CAirlineIcaoCode &airline) const
        {
            const int id = airline.getGroupId();
            if (id < 0) return {};
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return model.getAirlineIcaoCode().getGroupId() == id;
            });
        }

        CAircraftModelList CAircraftModelList::findByAirlineNameAndTelephonyDesignator(const QString &name, const QString &telephony, bool onlyIfExistInModel) const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                if (!model.hasAirlineDesignator() || !model.hasValidDbKey()) { return false; }
                const CAirlineIcaoCode &icao = model.getAirlineIcaoCode();
                if (!name.isEmpty() && (icao.hasName() || !onlyIfExistInModel))
                {
                    if (!icao.getName().contains(name, Qt::CaseInsensitive)) { return false; }
                }

                if (!telephony.isEmpty() && (icao.hasTelephonyDesignator() || !onlyIfExistInModel))
                {
                    if (!icao.getTelephonyDesignator().contains(name, Qt::CaseInsensitive)) { return false; }
                }
                return true;
            });
        }

        CAircraftModelList CAircraftModelList::findByAirlineNamesOrTelephonyDesignator(const QString &name) const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                if (!model.hasAirlineDesignator() || !model.hasValidDbKey()) { return false; }
                const CAirlineIcaoCode &icao = model.getAirlineIcaoCode();
                return icao.matchesNamesOrTelephonyDesignator(name);
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

        CAircraftModelList CAircraftModelList::findByDistributor(const CDistributor &distributor) const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return model.getDistributor() == distributor;
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

        CAircraftModelList CAircraftModelList::findByDesignatorsOrFamilyWithColorLivery(const QStringList &designators) const
        {
            return this->findBy([ & ](const CAircraftModel & model)
            {
                if (!model.getLivery().isColorLivery()) { return false; }
                const CAircraftIcaoCode modelIcao(model.getAircraftIcaoCode());
                if (designators.contains(modelIcao.getDesignator())) { return true; }
                return modelIcao.hasFamily() && designators.contains(modelIcao.getFamily());
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

        CAircraftModelList CAircraftModelList::findByCombinedAndManufacturer(const CAircraftIcaoCode &icao) const
        {
            return this->findByCombinedAndManufacturer(icao.getCombinedType(), icao.getManufacturer());
        }

        CAircraftModelList CAircraftModelList::findByCombinedAndManufacturer(const QString &combinedType, const QString &manufacturer) const
        {
            if (manufacturer.isEmpty()) { return this->findByCombinedType(combinedType); }
            if (combinedType.isEmpty()) { return this->findByManufacturer(manufacturer); }
            return this->findBy([ & ](const CAircraftModel & model)
            {
                return model.getAircraftIcaoCode().matchesCombinedTypeAndManufacturer(combinedType, manufacturer);
            });
        }

        CAircraftModelList CAircraftModelList::findClosestColorDistance(const CRgbColor &fuselage, const CRgbColor &tail) const
        {
            double distance = 2.0;
            CAircraftModelList models;
            for (const CAircraftModel &m : (*this))
            {
                const CLivery &l = m.getLivery();
                if (!l.hasColorTail() || !l.hasColorFuselage()) { continue; }
                const double d = l.getColorDistance(fuselage, tail);
                if (qFuzzyCompare(d, distance))
                {
                    models.push_back(m);
                }
                else if (distance > d)
                {
                    models.clear();
                    models.push_back(m);
                    distance = d;
                }
            }
            return models;
        }

        CAircraftModelList CAircraftModelList::findClosestFuselageColorDistance(const CRgbColor &color) const
        {
            return this->findClosestColorDistance(color, color);
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

        CAircraftModelList CAircraftModelList::findByCategoryFirstLevel(int firstLevel) const
        {
            if (firstLevel < 0) { return CAircraftModelList(); }
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return (model.hasCategory() && model.getAircraftIcaoCode().getCategory().getFirstLevel() == firstLevel);
            });
        }

        CAircraftModelList CAircraftModelList::findByCategory(const CAircraftCategory &category) const
        {
            if (category.isNull()) { return CAircraftModelList(); }
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return (model.hasCategory() && model.getAircraftIcaoCode().getCategory() == category);
            });
        }

        CAircraftModelList CAircraftModelList::findByCategories(const CAircraftCategoryList &categories) const
        {
            if (categories.isEmpty()) { return CAircraftModelList(); }
            return this->findBy([ = ](const CAircraftModel & model)
            {
                return (model.hasCategory() && categories.contains(model.getAircraftIcaoCode().getCategory()));
            });
        }

        CAircraftModelList CAircraftModelList::findFsFamilyModels() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return model.getSimulator().isMicrosoftOrPrepare3DSimulator();
            });
        }

        CAircraftModelList CAircraftModelList::findNonFsFamilyModels() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return !model.getSimulator().isMicrosoftOrPrepare3DSimulator();
            });
        }

        CAircraftModelList CAircraftModelList::findWithValidDbKey() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return model.hasValidDbKey();
            });
        }

        CAircraftModelList CAircraftModelList::findWithoutValidDbKey() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return !model.hasValidDbKey();
            });
        }

        CAircraftModelList CAircraftModelList::findWithoutValidDbKey(int maxElements) const
        {
            CAircraftModelList ml = this->findWithoutValidDbKey();
            ml.truncate(maxElements);
            return ml;
        }

        CAircraftModelList CAircraftModelList::findNonDbModelsForAirline(const QString &airline) const
        {
            const CAircraftModelList noDb = this->findWithoutValidDbKey();
            return noDb.findBy([&](const CAircraftModel & model)
            {
                return model.getAirlineIcaoCode().matchesDesignator(airline);
            });
        }

        CAircraftModelList CAircraftModelList::findNonDbModelsForAircraft(const QString &airline) const
        {
            const CAircraftModelList noDb = this->findWithoutValidDbKey();
            return noDb.findBy([&](const CAircraftModel & model)
            {
                return model.getAircraftIcaoCode().matchesDesignator(airline);
            });
        }

        CAircraftModelList CAircraftModelList::findNonDbModelsForModelString(const QString &modelString) const
        {
            const CAircraftModelList noDb = this->findWithoutValidDbKey();
            return noDb.findBy([&](const CAircraftModel & model)
            {
                return model.matchesModelString(modelString, Qt::CaseInsensitive);
            });
        }

        CAircraftModelList CAircraftModelList::findAllIncludedModels() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return model.getModelMode() == CAircraftModel::Include;
            });
        }

        CAircraftModelList CAircraftModelList::findAllIncludedModels(int maxElements) const
        {
            CAircraftModelList ml = this->findAllIncludedModels();
            ml.truncate(maxElements);
            return ml;
        }

        CAircraftModelList CAircraftModelList::findAllExcludedModels() const
        {
            return this->findBy([](const CAircraftModel & model)
            {
                return model.getModelMode() == CAircraftModel::Exclude;
            });
        }

        CAircraftModelList CAircraftModelList::findAllExcludedModels(int maxElements) const
        {
            CAircraftModelList ml = this->findAllExcludedModels();
            ml.truncate(maxElements);
            return ml;
        }

        CAircraftModelList CAircraftModelList::findDuplicateModelStrings() const
        {
            const QMap<QString, int> modelStrings = this->countPerModelString();
            CAircraftModelList duplicates;
            for (const auto [string, count] : makePairsRange(modelStrings))
            {
                if (count > 1)
                {
                    duplicates.push_back(this->findByModelString(string, Qt::CaseInsensitive));
                }
            }
            return duplicates;
        }

        QMap<QString, int> CAircraftModelList::countPerModelString() const
        {
            QMap<QString, int> modelStrings;
            for (const CAircraftModel &model : *this)
            {
                if (modelStrings.contains(model.getModelString()))
                {
                    modelStrings[model.getModelModeAsString()]++;
                }
                else
                {
                    modelStrings[model.getModelModeAsString()] = 1;
                }
            }
            return modelStrings;
        }

        QMap<CDistributor, int> CAircraftModelList::countPerDistributor() const
        {
            QMap<CDistributor, int> distributors;
            for (const CAircraftModel &model : *this)
            {
                if (!model.hasDistributor()) { continue; }
                if (distributors.contains(model.getDistributor()))
                {
                    distributors[model.getDistributor()]++;
                }
                else
                {
                    distributors[model.getDistributor()] = 1;
                }
            }
            return distributors;
        }

        QMap<CAircraftIcaoCode, int> CAircraftModelList::countPerAircraftIcao() const
        {
            QMap<CAircraftIcaoCode, int> icaos;
            for (const CAircraftModel &model : *this)
            {
                if (!model.hasAircraftDesignator()) { continue; }
                if (icaos.contains(model.getAircraftIcaoCode()))
                {
                    icaos[model.getAircraftIcaoCode()]++;
                }
                else
                {
                    icaos[model.getAircraftIcaoCode()] = 1;
                }
            }
            return icaos;
        }

        QMap<CAirlineIcaoCode, int> CAircraftModelList::countPerAirlineIcao() const
        {
            QMap<CAirlineIcaoCode, int> icaos;
            for (const CAircraftModel &model : *this)
            {
                if (!model.hasAirlineDesignator()) { continue; }
                if (icaos.contains(model.getAirlineIcaoCode()))
                {
                    icaos[model.getAirlineIcaoCode()]++;
                }
                else
                {
                    icaos[model.getAirlineIcaoCode()] = 1;
                }
            }
            return icaos;
        }

        CAirlineIcaoCode CAircraftModelList::getAirlineWithMaxCount() const
        {
            const QMap<CAirlineIcaoCode, int> ac = this->countPerAirlineIcao();
            if (ac.size() < 1) { return {}; }
            if (ac.size() == 1) { return ac.firstKey(); }
            const QList<int> values = ac.values();
            const int max = *std::max_element(values.begin(), values.end());
            return ac.key(max);
        }

        QString CAircraftModelList::findModelIconPathByModelString(const QString &modelString) const
        {
            if (modelString.isEmpty()) { return {}; }
            const CAircraftModel m(findFirstByModelStringOrDefault(modelString, Qt::CaseInsensitive));
            return m.getIconFile();
        }

        QString CAircraftModelList::findModelIconPathByCallsign(const CCallsign &callsign) const
        {
            if (callsign.isEmpty()) { return {}; }
            const CAircraftModel m(findFirstByCallsignOrDefault(callsign));
            return m.getIconFile();
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

        bool CAircraftModelList::containsCategory() const
        {
            return this->containsBy([ & ](const CAircraftModel & model)
            {
                return model.hasCategory();
            });
        }

        bool CAircraftModelList::containsCategory(int firstLevel) const
        {
            if (firstLevel < 0) { return false; }
            return this->containsBy([ & ](const CAircraftModel & model)
            {
                return model.hasCategory() && model.getAircraftIcaoCode().getCategory().getFirstLevel() == firstLevel;
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

        namespace Private
        {
            bool isLikelyImpl(double count, double total)
            {
                if (total < 1) { return false; }
                const double fsRatio = count / total;
                return fsRatio > 0.95;
            }
        }

        bool CAircraftModelList::isLikelyFsFamilyModelList() const
        {
            if (this->isEmpty()) { return false; } // avoid DIV 0
            return Private::isLikelyImpl(this->countPerSimulator().getCountForFsFamilySimulators(), this->size());
        }

        bool CAircraftModelList::isLikelyFsxFamilyModelList() const
        {
            if (this->isEmpty()) { return false; } // avoid DIV 0
            return Private::isLikelyImpl(this->countPerSimulator().getCountForFsxFamilySimulators(), this->size());
        }

        bool CAircraftModelList::isLikelyXPlaneModelList() const
        {
            if (this->isEmpty()) { return false; } // avoid DIV 0
            return Private::isLikelyImpl(this->countPerSimulator().getCount(CSimulatorInfo::xplane()), this->size());
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

        bool CAircraftModelList::removeModelWithString(const QString &modelString, Qt::CaseSensitivity sensitivity)
        {
            if (modelString.isEmpty()) { return false; }
            if (this->isEmpty()) { return false; }
            const int r = this->removeIf([&](const CAircraftModel & model) { return model.matchesModelString(modelString, sensitivity); });
            return r > 0;
        }

        int CAircraftModelList::removeModelsWithString(const CAircraftModelList &models, Qt::CaseSensitivity sensitivity)
        {
            if (models.isEmpty()) { return 0; }
            return this->removeModelsWithString(models.getModelStringList(false), sensitivity);
        }

        int CAircraftModelList::removeModelsWithString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity)
        {
            if (modelStrings.isEmpty()) { return 0; }
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

        int CAircraftModelList::removeXPlaneFlyablePlanes()
        {
            return this->removeIfExcluded();
        }

        int CAircraftModelList::removeByDistributor(const CDistributor &distributor)
        {
            return this->removeIf(&CAircraftModel::getDistributor, distributor);
        }

        int CAircraftModelList::removeByAircraftAndLivery(const CAircraftIcaoCode &aircraftIcao, const CLivery &livery)
        {
            return this->removeIf(&CAircraftModel::getAircraftIcaoCode, aircraftIcao, &CAircraftModel::getLivery, livery);
        }

        int CAircraftModelList::removeByAircraftAndAirline(const CAircraftIcaoCode &aircraftIcao, const CAirlineIcaoCode &airline)
        {
            return this->removeIf(&CAircraftModel::getAircraftIcaoCode, aircraftIcao, &CAircraftModel::getAirlineIcaoCode, airline);
        }

        int CAircraftModelList::removeIfNotFsFamily()
        {
            if (this->isEmpty()) { return 0; }
            CAircraftModelList fsOnly = this->findFsFamilyModels();
            if (fsOnly.size() == this->size()) { return 0; }
            const int delta = this->size() - fsOnly.size();
            *this = fsOnly;
            return delta;
        }

        CAircraftModelList CAircraftModelList::removeIfFileButNotInSet(const QString &fileName, const QSet<QString> &modelStrings)
        {
            CAircraftModelList removed;
            for (const CAircraftModel &model : *this)
            {
                if (!model.matchesFileName(fileName)) { continue; }
                if (modelStrings.contains(model.getModelString())) { continue; }
                removed.push_back(model);
            }

            this->removeIfIn(removed);
            return removed;
        }

        bool CAircraftModelList::replaceOrAddModelWithString(const CAircraftModel &addOrReplaceModel, Qt::CaseSensitivity sensitivity)
        {
            bool r = false;
            if (!this->isEmpty()) { r = this->removeModelWithString(addOrReplaceModel.getModelString(), sensitivity); }
            this->push_back(addOrReplaceModel);
            return r;
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
            for (const CAircraftModel &model : *this)
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
            for (const CAircraftModel &model : *this)
            {
                if (!model.hasModelString()) { continue; }
                ms.insert(model.getModelString());
            }
            return ms;
        }

        CCountPerSimulator CAircraftModelList::countPerSimulator() const
        {
            CCountPerSimulator count;
            for (const CAircraftModel &model : *this)
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
            for (const CAircraftModel &model : *this)
            {
                if (model.getLivery().isColorLivery()) { count++; }
            }
            return count;
        }

        int CAircraftModelList::countModelsWithAirlineLivery() const
        {
            int count = 0;
            for (const CAircraftModel &model : *this)
            {
                if (model.getLivery().isAirlineLivery()) { count++; }
            }
            return count;
        }

        int CAircraftModelList::countVtolAircraft() const
        {
            int count = 0;
            for (const CAircraftModel &model : *this)
            {
                if (model.isVtol()) { count++; }
            }
            return count;
        }

        int CAircraftModelList::countByMode(CAircraftModel::ModelMode mode) const
        {
            int count = 0;
            for (const CAircraftModel &model : *this)
            {
                if (model.matchesMode(mode)) { count++; }
            }
            return count;
        }

        int CAircraftModelList::countMilitaryAircraft() const
        {
            int count = 0;
            for (const CAircraftModel &model : *this)
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

        int CAircraftModelList::countAliases() const
        {
            int count = 0;
            for (const CAircraftModel &model : (*this))
            {
                if (model.hasModelStringAlias()) { count++; }
            }
            return count;
        }

        void CAircraftModelList::sortByFileName()
        {
            if (CFileUtils::isFileNameCaseSensitive())
            {
                this->sortBy(&CAircraftModel::getFileName);
            }
            else
            {
                this->sortBy(&CAircraftModel::getFileNameLowerCase);
            }
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

        CAirlineIcaoCodeList CAircraftModelList::getAirlineIcaoCodesForGroup(int groupId) const
        {
            if (groupId < 0) { return {}; }
            CAirlineIcaoCodeList icaos;
            for (const CAircraftModel &model : *this)
            {
                if (model.getAirlineIcaoCode().getGroupId() == groupId)
                {
                    icaos.push_back(model.getAirlineIcaoCode());
                }
            }
            return icaos;
        }

        QSet<QString> CAircraftModelList::getAirlineDesignatorsForGroup(int groupId) const
        {
            return this->getAirlineIcaoCodesForGroup(groupId).allDesignators();
        }

        QSet<QString> CAircraftModelList::getAirlineVDesignatorsForGroup(int groupId) const
        {
            return this->getAirlineIcaoCodesForGroup(groupId).allVDesignators();
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

        QSet<QString> CAircraftModelList::getAllFileNames() const
        {
            const bool cs = CFileUtils::isFileNameCaseSensitive();
            QSet<QString> files;
            for (const CAircraftModel &model : *this)
            {
                if (!model.hasFileName()) { continue; }
                files.insert(cs ? model.getFileName() : model.getFileNameLowerCase());
            }
            return files;
        }

        QSet<QString> CAircraftModelList::getAllUNCFileNames() const
        {
            const bool cs = CFileUtils::isFileNameCaseSensitive();
            QSet<QString> files;
            for (const CAircraftModel &model : *this)
            {
                if (!model.hasFileName()) { continue; }
                const QString fn = (cs ? model.getFileName() : model.getFileNameLowerCase());
                if (CFileUtils::isWindowsUncPath(fn))
                {
                    files.insert(fn);
                }
            }
            return files;
        }

        QString CAircraftModelList::getCombinedTypesAsString(const QString &separator) const
        {
            if (this->isEmpty()) { return {}; }
            return joinStringSet(this->getCombinedTypes(), separator);
        }

        QSet<QString> CAircraftModelList::getAicraftAndAirlineDesignators(bool withDbId) const
        {
            QSet<QString> str;
            for (const CAircraftModel &model : *this)
            {
                const QString s = (model.hasAircraftDesignator() ?
                                   (withDbId ? model.getAircraftIcaoCode().getDesignatorDbKey() : model.getAircraftIcaoCodeDesignator()) :
                                   "no aircraft") %
                                  u"/" %
                                  (model.hasAircraftDesignator() ?
                                   (withDbId ? model.getAirlineIcaoCode().getDesignatorDbKey() : model.getAirlineIcaoCodeVDesignator()) :
                                   "no airline");
                str.insert(s);
            }
            return str;
        }

        QString CAircraftModelList::getAicraftAndAirlineDesignatorsAsString(bool withDbId, const QString &separator) const
        {
            if (this->isEmpty()) { return {}; }
            return joinStringSet(this->getAicraftAndAirlineDesignators(withDbId), separator);
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
            CCallsign::addLogDetailsToList(log, remoteModel.getCallsign(), QStringLiteral("Prefer color liveries: '%1', airline: '%2', ignore zero scores: '%3'").arg(boolToYesNo(preferColorLiveries), remoteModel.getAirlineIcaoCodeDesignator(), boolToYesNo(ignoreZeroScores)));
            CCallsign::addLogDetailsToList(log, remoteModel.getCallsign(), QStringLiteral("--- Start scoring in list with %1 models").arg(this->size()));
            CCallsign::addLogDetailsToList(log, remoteModel.getCallsign(), this->coverageSummaryForModel(remoteModel));

            int c = 1;
            for (const CAircraftModel &model : *this)
            {
                CStatusMessageList subMsgs;
                const int score = model.calculateScore(remoteModel, preferColorLiveries, log ? &subMsgs : nullptr);
                if (ignoreZeroScores && score < 1) { continue; }

                CCallsign::addLogDetailsToList(log, remoteModel.getCallsign(), QStringLiteral("--- Calculating #%1 '%2'---").arg(c).arg(model.getModelStringAndDbKey()));
                if (log) { log->push_back(subMsgs); }
                CCallsign::addLogDetailsToList(log, remoteModel.getCallsign(), QStringLiteral("--- End calculating #%1 ---").arg(c));
                c++;
                scoreMap.insertMulti(score, model);
            }
            CCallsign::addLogDetailsToList(log, remoteModel.getCallsign(), QStringLiteral("--- End scoring ---"));
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
            return this->validateForPublishing(validModels, invalidModels);
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
                    CAircraftModelList::addAsValidOrInvalidModel(model, true, validModels, invalidModels);
                    continue;
                }
                if (model.hasModelString())
                {
                    singleMsg.prependMessage(model.getModelString() % u": ");
                }
                msgs.push_back(singleMsg);
                CAircraftModelList::addAsValidOrInvalidModel(model, false, validModels, invalidModels);
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
                const CStatusMessage msg = CStatusMessage(this).validationError(u"No DB distributors for validation");
                msgs.push_back(msg);
                CAircraftModelList::addAsValidOrInvalidModels(*this, false, validModels, invalidModels);
                return msgs;
            }

            for (const CAircraftModel &model : *this)
            {
                const bool valid = (model.hasDbDistributor() || model.matchesAnyDbDistributor(distributorsFromDb));
                CAircraftModelList::addAsValidOrInvalidModel(model, valid, validModels, invalidModels);
                if (!valid)
                {
                    const CStatusMessage msg = CStatusMessage(this).validationError(u"No valid distributor for '%1', was '%2'") << model.getModelString() << model.getDistributor().getDbKey();
                    msgs.push_back(msg);
                }
            }
            return msgs;
        }

        CStatusMessageList CAircraftModelList::validateFiles(CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simRootDirectory, bool alreadySortedByFn) const
        {
            wasStopped = false;

            CStatusMessageList msgs;
            QSet<QString> failedFiles;
            QSet<QString> workingFiles;
            int failedFilesCount = 0;

            // sorting allows to skip multiple files as once when a file fails
            CAircraftModelList sorted(*this);
            if (!alreadySortedByFn) { sorted.sortByFileName(); }

            // avoid hanging if UNC paths are not available
            if (CBuildConfig::isRunningOnWindowsNtPlatform())
            {
                const CStatusMessageList uncMsgs = this->validateUncFiles(sorted.getAllUNCFileNames());
                if (uncMsgs.hasErrorMessages()) { return uncMsgs; }
            }

            const bool caseSensitive = CFileUtils::isFileNameCaseSensitive();
            const QString simRootDir = CFileUtils::normalizeFilePathToQtStandard(
                                           CFileUtils::stripLeadingSlashOrDriveLetter(
                                               caseSensitive ? simRootDirectory : simRootDirectory.toLower()
                                           )
                                       );

            for (const CAircraftModel &model : as_const(sorted))
            {
                if (wasStopped) { break; } // allow breaking from external
                bool ok = false;
                do
                {
                    if (!model.hasModelString())
                    {
                        msgs.push_back(CStatusMessage(this).validationError(u"No model string"));
                        break;
                    }

                    if (!model.hasFileName())
                    {
                        if (ignoreEmptyFileNames) { continue; }
                        msgs.push_back(CStatusMessage(this).validationError(u"'%1', no file name") << model.getModelStringAndDbKey());
                        break;
                    }

                    const QString fn(caseSensitive ? model.getFileName() : model.getFileNameLowerCase());
                    if (failedFiles.contains(fn))
                    {
                        msgs.push_back(CStatusMessage(this).validationError(u"'%1', known failed file '%2' skipped") << model.getModelStringAndDbKey() << model.getFileName());
                        break;
                    }

                    if (workingFiles.contains(fn) || model.hasExistingCorrespondingFile())
                    {
                        if (!simRootDirectory.isEmpty() && !fn.contains(simRootDir))
                        {
                            // check if in root directory
                            msgs.push_back(CStatusMessage(this).validationError(u"'%1', not in root directory '%2', '%3' skipped") << model.getModelStringAndDbKey() << simRootDir << model.getFileName());
                            failedFiles.insert(fn);
                            failedFilesCount++;
                            break;
                        }
                        else
                        {
                            ok = true;
                            workingFiles.insert(fn);
                            // msgs.push_back(CStatusMessage(this).validationInfo(u"'%1', file '%2' existing") << model.getModelStringAndDbKey() << model.getFileName());
                            break;
                        }
                    }

                    failedFiles.insert(fn);
                    failedFilesCount++;
                    msgs.push_back(CStatusMessage(this).validationError(u"'%1', file '%2' not existing") << model.getModelStringAndDbKey() << model.getFileName());
                }
                while (false);

                CAircraftModelList::addAsValidOrInvalidModel(model, ok, validModels, invalidModels);
                if (stopAtFailedFiles > 0 && failedFilesCount >= stopAtFailedFiles)
                {
                    wasStopped = true;
                    msgs.push_back(CStatusMessage(this).validationWarning(u"Stopping after %1 failed files") << failedFilesCount);
                    break;
                }
            }

            // Summary
            if (!validModels.isEmpty())   { msgs.push_back(CStatusMessage(this).validationInfo(u"File validation, valid models: %1") << validModels.size()); }
            if (!invalidModels.isEmpty()) { msgs.push_back(CStatusMessage(this).validationWarning(u"File validation, invalid models: %1") << invalidModels.size()); }

            // done
            return msgs;
        }

        CStatusMessageList CAircraftModelList::validateUncFiles(const QSet<QString> uncFiles) const
        {
            // check if UNC paths can be reached
            CStatusMessageList msgs;
            if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return msgs; }
            if (uncFiles.isEmpty()) { return msgs; }

            const QSet<QString> uncMachines = CFileUtils::windowsUncMachines(uncFiles);
            if (uncMachines.isEmpty())
            {
                msgs.push_back(CStatusMessage(this).validationInfo(u"Found NO UNC machines for %1 files, odd...?") << uncFiles.size());
            }
            else
            {
                const QString machines = joinStringSet(uncMachines, ", ");
                msgs.push_back(CStatusMessage(this).validationInfo(u"Found %1 UNC files on machines: %2") << uncFiles.size() << machines);
            }

            for (const QString &m : uncMachines)
            {
                const bool ping = canPing(m);
                if (!ping)
                {
                    msgs.push_back(CStatusMessage(this).validationError(u"Cannot ping UNC machine(s): %1. UNC files: %2") << m << uncFiles.size());
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
                Q_UNUSED(scope)
                helper.getTable<CAircraftIcaoCode>().convertFromJson(aircraftIcaos.toObject());
            }
            {
                CJsonScope scope("liveries");
                Q_UNUSED(scope)
                helper.getTable<CLivery>().convertFromJson(liveries.toObject());
            }
            {
                CJsonScope scope("distributors");
                Q_UNUSED(scope)
                helper.getTable<CDistributor>().convertFromJson(distributors.toObject());
            }

            int index = 0;
            for (auto i = array.begin(); i != array.end(); ++i)
            {
                CJsonScope scope("containerbase", index++);
                Q_UNUSED(scope)
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
            if (this->isEmpty()) { return {}; }
            QString html;
            for (const CAircraftModel &model : *this)
            {
                html += html.isEmpty() ?
                        model.asHtmlSummary(" ") :
                        u"<br>" % model.asHtmlSummary(" ");
            }
            return html;
        }

        QString CAircraftModelList::coverageSummary(const QString &separator) const
        {
            if (this->isEmpty()) { return "no models"; } // avoid division by 0

            const int dbEntries = this->countWithValidDbKey();
            const double dbRatio = CMathUtils::round(static_cast<double>(100 * dbEntries) / this->size(), 1);
            return
                u"Entries: " % QString::number(this->size()) %
                u" | valid DB keys: " % QString::number(dbEntries) %
                u" (" % QString::number(dbRatio) % u"%)" % separator %
                u"color liveries: " % QString::number(this->countModelsWithColorLivery()) %
                u" | airline liveries: " % QString::number(this->countModelsWithAirlineLivery()) % separator %
                u"VTOL: " % QString::number(this->countVtolAircraft()) %
                u" | military: " % QString::number(this->countMilitaryAircraft()) %
                u" | civilian: " % QString::number(this->countCivilianAircraft()) % separator %
                u"Different airlines: " % QString::number(this->countDifferentAirlines()) % separator %
                u"Combined types: '" % this->getCombinedTypesAsString() % u'\'' % separator %
                (this->size() <= 25 ?
                 (u"Aircraft/airlines: " % this->getAicraftAndAirlineDesignatorsAsString(true) % separator) :
                 QString()) %
                u"Simulators: " % this->countPerSimulator().toQString();
        }

        QString CAircraftModelList::coverageSummaryForModel(const CAircraftModel &checkModel, const QString &separator) const
        {
            const bool combinedCodeForModel = this->containsCombinedType(checkModel.getAircraftIcaoCode().getCombinedType());
            const bool airlineForModel = checkModel.hasAirlineDesignator() && this->containsAirlineLivery(checkModel.getAirlineIcaoCode());
            return coverageSummary(separator) % separator %
                   u"Data for input model, has combined: " % boolToYesNo(combinedCodeForModel) %
                   (
                       checkModel.hasAirlineDesignator() ?
                       u" airline '" % checkModel.getAirlineIcaoCodeDesignator() % u"': " % boolToYesNo(airlineForModel) :
                       QString()
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
                QStringList designators(icaos.allDesignators().values());
                designators.sort();
                stats += sep % sep %
                         u"Aircraft ICAOs from DB: " % sep %
                         designators.join(", ");
            }

            if (airlineStats)
            {
                const CAirlineIcaoCodeList icaos = this->getAirlineIcaoCodesFromDb();
                const QStringList designators = icaos.toIcaoDesignatorCompleterStrings();
                stats += sep % sep %
                         u"Airline ICAOs from DB: " % sep %
                         designators.join(", ");
            }

            if (notOnlyDb)
            {
                const CAircraftModelList dbModels = this->findObjectsWithDbKey();
                stats += sep %
                         sep %
                         u"DB objects:<br>---------" %
                         sep %
                         dbModels.htmlStatistics(false, false);
            }
            return stats;
        }

        CStatusMessage CAircraftModelList::saveInvalidModels() const
        {
            if (this->isEmpty()) { return CStatusMessage(this).info(u"No models"); }
            const QString json = this->toJsonString();
            const bool s = CFileUtils::writeStringToFile(json, invalidModelFileAndPath());
            if (!s) { return CStatusMessage(this).error(u"Unable to save %1 entries to '%2'") << this->size() << invalidModelFileAndPath(); }
            return CStatusMessage(this).info(u"Saved %1 entries to '%2'") << this->size() << invalidModelFileAndPath();
        }

        CStatusMessage CAircraftModelList::loadInvalidModels()
        {
            const QString json = CFileUtils::readFileToString(invalidModelFileAndPath());
            if (json.isEmpty()) { return CStatusMessage(this).error(u"Unable to read from '%1'") << invalidModelFileAndPath(); }
            *this = CAircraftModelList::fromJson(json, true);
            return CStatusMessage(this).info(u"Loaded %1 entries from '%2'") << this->size() << invalidModelFileAndPath();
        }

        CAircraftModelList CAircraftModelList::fromDatabaseJsonCaching(
            const QJsonArray &array,
            const CAircraftIcaoCodeList &icaos,
            const CAircraftCategoryList &categories,
            const CLiveryList &liveries,
            const CDistributorList &distributors
        )
        {
            AircraftIcaoIdMap aircraftIcaosMap = icaos.toDbKeyValueMap();
            LiveryIdMap       liveriesMap = liveries.toDbKeyValueMap();
            DistributorIdMap  distributorsMap = distributors.toDbKeyValueMap();
            const AircraftCategoryIdMap categoriesMap = categories.toDbKeyValueMap();

            CAircraftModelList models;
            for (const QJsonValue &value : array)
            {
                models.push_back(CAircraftModel::fromDatabaseJsonCaching(value.toObject(), aircraftIcaosMap, categoriesMap, liveriesMap, distributorsMap));
            }
            return models;
        }

        const QString &CAircraftModelList::invalidModelFileAndPath()
        {
            static const QString f = CFileUtils::appendFilePathsAndFixUnc(CSwiftDirectories::logDirectory(), "invalidmodels.json");
            return f;
        }

        bool CAircraftModelList::hasInvalidModelFile()
        {
            const QFileInfo fi(invalidModelFileAndPath());
            return fi.exists();
        }

        void CAircraftModelList::addAsValidOrInvalidModel(const CAircraftModel &model, bool valid, CAircraftModelList &validModels, CAircraftModelList &invalidModels)
        {
            if (valid)
            {
                validModels.push_back(model);
                invalidModels.removeModelWithString(model.getModelString(), Qt::CaseInsensitive);
            }
            else
            {
                invalidModels.push_back(model);
                validModels.removeModelWithString(model.getModelString(), Qt::CaseInsensitive);
            }
        }

        void CAircraftModelList::addAsValidOrInvalidModels(const CAircraftModelList &models, bool valid, CAircraftModelList &validModels, CAircraftModelList &invalidModels)
        {
            for (const CAircraftModel &model : models)
            {
                CAircraftModelList::addAsValidOrInvalidModel(model, valid, validModels, invalidModels);
            }
        }
    } // namespace
} // namespace
