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
#include "blackcore/databasereader.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/aircraftmodellist.h"

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore
{
    //! Read model related data from Database
    class BLACKCORE_EXPORT CModelDataReader : public CDatabaseReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelDataReader(QObject *owner, const QString &protocol, const QString &server, const QString &baseUrl);

        //! Get aircraft liveries
        //! \threadsafe
        BlackMisc::Aviation::CLiveryList getLiveries() const;

        //! Get aircraft livery for code
        //! \threadsafe
        BlackMisc::Aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const;

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
        BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributor) const;

        //! Get models count
        //! \threadsafe
        int getModelsCount() const;

        //! All data read?
        //! \threadsafe
        bool areAllDataRead() const;

        //! Can connect?
        virtual bool canConnect(QString &message) const override;

        //! \copydoc CDatabaseReader::canConnect()
        using CDatabaseReader::canConnect;

        //! Write to JSON file
        bool readFromJsonFiles(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead = BlackMisc::Network::CEntityFlags::DistributorLiveryModel);

        //! Read from static DB data file
        bool readFromJsonFilesInBackground(const QString &dir, BlackMisc::Network::CEntityFlags::Entity whatToRead = BlackMisc::Network::CEntityFlags::DistributorLiveryModel);

        //! Write to JSON file
        bool writeToJsonFiles(const QString &dir) const;

    signals:
        //! Combined read signal
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

    private slots:
        //! Liveries have been read
        void ps_parseLiveryData(QNetworkReply *nwReply);

        //! Distributors have been read
        void ps_parseDistributorData(QNetworkReply *nwReply);

        //! Models have been read
        void ps_parseModelData(QNetworkReply *nwReply);

        //! Read / re-read data file
        void ps_read(BlackMisc::Network::CEntityFlags::Entity entity = BlackMisc::Network::CEntityFlags::DistributorLiveryModel);

    private:
        QNetworkAccessManager *m_networkManagerLivery = nullptr;
        QNetworkAccessManager *m_networkManagerDistributor = nullptr;
        QNetworkAccessManager *m_networkManagerModel = nullptr;
        BlackMisc::Aviation::CLiveryList m_liveries;
        BlackMisc::Simulation::CDistributorList m_distributors;
        BlackMisc::Simulation::CAircraftModelList m_models;
        QString m_urlLiveries;
        QString m_urlDistributors;
        QString m_urlModels;

        mutable QReadWriteLock m_lockDistributor;
        mutable QReadWriteLock m_lockLivery;
        mutable QReadWriteLock m_lockModels;

        //! URL livery web service
        static QString getLiveryUrl(const QString &protocol, const QString &server, const QString &baseUrl);

        //! URL distributor web service
        static QString getDistributorUrl(const QString &protocol, const QString &server, const QString &baseUrl);

        //! URL model web service
        static QString getModelUrl(const QString &protocol, const QString &server, const QString &baseUrl);
    };
} // ns

#endif // guard
