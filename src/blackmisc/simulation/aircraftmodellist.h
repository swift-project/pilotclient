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
#include "blackmisc/simulation/distributorlist.h"
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
            public IDatastoreObjectList<CAircraftModel, CAircraftModelList, int>,
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

            //! Contains model with model string or id
            bool containsModelStringOrDbKey(const BlackMisc::Simulation::CAircraftModel &model, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find by model string
            //! \remark normally CAircraftModelList::findFirstByModelStringOrDefault would be used
            CAircraftModelList findByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find first by model string
            CAircraftModel findFirstByModelStringOrDefault(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find first by callsign
            CAircraftModel findFirstByCallsignOrDefault(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Find models starting with
            CAircraftModelList findModelsStartingWith(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find by a given list of models by strings
            CAircraftModelList findByModelStrings(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity) const;

            //! Find by a given list of models trings
            CAircraftModelList findByNotInModelStrings(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity) const;

            //! Find by model string
            CAircraftModelList findByIcaoDesignators(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode) const;

            //! Find by model string
            CAircraftModelList findByAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const;

            //! Find by livery code
            CAircraftModelList findByLiveryCode(const BlackMisc::Aviation::CLivery &livery) const;

            //! With file name
            CAircraftModelList findWithFileName() const;

            //! All models from given distributors
            CAircraftModelList findByDistributors(const CDistributorList &distributors) const;

            //! Models with aircraft ICAO code set
            CAircraftModelList findWithAircraftDesignator() const;

            //! Models with aircraft ICAO code from list
            CAircraftModelList findWithAircraftDesignator(const QStringList &designators) const;

            //! Models with a known aircraft ICAO code set
            CAircraftModelList findWithKnownAircraftDesignator() const;

            //! Find by manufacturer
            CAircraftModelList findByManunfacturer(const QString &manufacturer) const;

            //! Models with aircraft family
            CAircraftModelList findByFamily(const QString &family) const;

            //! Find by combined code, wildcards possible e.g. L*P, *2J
            CAircraftModelList findByCombinedCode(const QString &combinedCode) const;

            //! Find by military flag
            CAircraftModelList findByMilitaryFlag(bool military) const;

            //! Model icon path
            QString findModelIconPathByModelString(const QString &modelString) const;

            //! Model icon path
            QString findModelIconPathByCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Take a designator and find its family
            QString designatorToFamily(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode) const;

            //! Find for given simulator
            CAircraftModelList matchesSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

            //! Set simulator for all elements
            int setSimulatorInfo(const BlackMisc::Simulation::CSimulatorInfo &info);

            //! Which simulators are supported in that model list
            CSimulatorInfo simulatorsSupported() const;

            //! Set mode for all elements
            int setModelMode(BlackMisc::Simulation::CAircraftModel::ModelMode mode);

            //! Keep only those models with given model strings
            //! \return number of elements removed
            int keepModelsWithString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity);

            //! Remove those models with given model strings
            //! \return number of elements removed
            int removeModelsWithString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity);

            //! Remove if not matching simulator
            //! \return number of elements removed
            int removeIfNotMatchingSimulator(const CSimulatorInfo &needToMatch);

            //! Remove if having no model string
            //! \return number of elements removed
            int removeAllWithoutModelString();

            //! Remove if excluded CAircraftModel::Exclude
            //! \return number of elements removed
            int removeIfExcluded();

            //! Replace or add based on model string
            //! \return number of elements removed
            int replaceOrAddModelsWithString(const CAircraftModelList &addOrReplaceList, Qt::CaseSensitivity sensitivity);

            //! Model strings
            QStringList getModelStrings(bool sort = true) const;

            //! Simulator counts
            CCountPerSimulator countPerSimulator() const;

            //! Which simulator(s) have the most entries
            CSimulatorInfo simulatorsWithMaxEntries() const;

            //! Update distributors
            void updateDistributor(const CDistributor &distributor);

            //! Update aircraft ICAO
            void updateAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

            //! Update livery
            void updateLivery(const BlackMisc::Aviation::CLivery &livery);

            //! Completer strings
            QStringList toCompleterStrings(bool sorted = true) const;

            //! Validate for publishing
            CStatusMessageList validateForPublishing() const;

            //! Validate for publishing
            CStatusMessageList validateForPublishing(CAircraftModelList &validateModels) const;

            //! To database JSON
            QJsonArray toDatabaseJson() const;

            //! To database JSON
            QString toDatabaseJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Compact) const;
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModelList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CAircraftModel>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::CAircraftModel>)

#endif //guard
