/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_MODELDATAREADER_H
#define BLACKCORE_MODELDATAREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/db/databasereader.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/url.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlist.h"

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QStringList>

class QNetworkReply;

namespace BlackCore
{
    namespace Db
    {
        //! Read model related data from Database
        class BLACKCORE_EXPORT CModelDataReader : public CDatabaseReader
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CModelDataReader(QObject *owner, const CDatabaseReaderConfigList &config);

            //! Get aircraft liveries
            //! \threadsafe
            BlackMisc::Aviation::CLiveryList getLiveries() const;

            //! Get aircraft livery for code
            //! \threadsafe
            BlackMisc::Aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const;

            //! Get aircraft livery for ICAO code
            //! \threadsafe
            BlackMisc::Aviation::CLivery getStdLiveryForAirlineCode(const BlackMisc::Aviation::CAirlineIcaoCode &icao) const;

            //! Get aircraft livery for id
            //! \threadsafe
            BlackMisc::Aviation::CLivery getLiveryForDbKey(int id) const;

            //! Best match specified by livery
            //! \threadsafe
            BlackMisc::Aviation::CLivery smartLiverySelector(const BlackMisc::Aviation::CLivery &livery) const;

            //! Get distributors (of models)
            //! \threadsafe
            BlackMisc::Simulation::CDistributorList getDistributors() const;

            //! Get models
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getModels() const;

            //! Get model for string
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModel getModelForModelString(const QString &modelString) const;

            //! Get model for designator/combined code
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode);

            //! Get aircraft liveries count
            //! \threadsafe
            int getLiveriesCount() const;

            //! Get model distributors count
            //! \threadsafe
            int getDistributorsCount() const;

            //! Best match specified by distributor
            //! \threadsafe
            BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributorPattern) const;

            //! Get models count
            //! \threadsafe
            int getModelsCount() const;

            //! Get model keys
            //! \threadsafe
            QList<int> getModelDbKeys() const;

            //! Get model keys
            //! \threadsafe
            QStringList getModelStrings() const;

            //! All data read?
            //! \threadsafe
            bool areAllDataRead() const;

            //! Read to JSON file
            bool readFromJsonFiles(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead = BlackMisc::Network::CEntityFlags::DistributorLiveryModel);

            //! Read from static DB data file
            bool readFromJsonFilesInBackground(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead = BlackMisc::Network::CEntityFlags::DistributorLiveryModel);

            //! Write to JSON file
            bool writeToJsonFiles(const QString &dir) const;

        signals:
            //! Combined read signal
            void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

        protected:
            //! \name cache handling for base class
            //! @{
            virtual void syncronizeCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
            virtual void invalidateCaches(BlackMisc::Network::CEntityFlags::Entity entities) override;
            virtual QDateTime getCacheTimestamp(BlackMisc::Network::CEntityFlags::Entity entity) override;
            //! @}

        private slots:
            //! Liveries have been read
            void ps_parseLiveryData(QNetworkReply *nwReply);

            //! Distributors have been read
            void ps_parseDistributorData(QNetworkReply *nwReply);

            //! Models have been read
            void ps_parseModelData(QNetworkReply *nwReply);

            //! Read / re-read data file
            void ps_read(BlackMisc::Network::CEntityFlags::Entity entity = BlackMisc::Network::CEntityFlags::DistributorLiveryModel, const QDateTime &newerThan = QDateTime());

        private:
            BlackMisc::Aviation::CLiveryList          m_liveries;
            BlackMisc::Simulation::CDistributorList   m_distributors;
            BlackMisc::Simulation::CAircraftModelList m_models;
            BlackMisc::Network::CUrl m_urlLiveries;
            BlackMisc::Network::CUrl m_urlDistributors;
            BlackMisc::Network::CUrl m_urlModels;

            mutable QReadWriteLock m_lockDistributor;
            mutable QReadWriteLock m_lockLivery;
            mutable QReadWriteLock m_lockModels;

            //! Base URL
            BlackMisc::Network::CUrl getBaseUrl() const;

            //! URL livery web service
            BlackMisc::Network::CUrl getLiveryUrl(bool shared = false) const;

            //! URL distributor web service
            BlackMisc::Network::CUrl getDistributorUrl(bool shared = false) const;

            //! URL model web service
            BlackMisc::Network::CUrl getModelUrl(bool shared = false) const;
        };
    } // ns
} // ns

#endif // guard
