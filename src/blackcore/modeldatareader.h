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

        //! Get distributors (of models)
        //! \threadsafe
        BlackMisc::Simulation::CDistributorList getDistributors() const;

        //! Get models
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModelList getModels() const;

        //! Get aircraft liveries count
        //! \threadsafe
        int getLiveriesCount() const;

        //! Get model distributors count
        //! \threadsafe
        int getDistributorsCount() const;

        //! Get models count
        //! \threadsafe
        int getModelsCount() const;

        //! All data read
        bool allRead() const;

    signals:
        //! Liveries have been read
        void readLiveries(int number);

        //! Distributors have been read
        void readDistributors(int number);

        //! Models have been read
        void readModels(int number);

        //! All data read
        void readAll();

    private slots:
        //! Liveries have been read
        void ps_parseLiveryData(QNetworkReply *nwReply);

        //! Distributors have been read
        void ps_parseDistributorData(QNetworkReply *nwReply);

        //! Models have been read
        void ps_parseModelData(QNetworkReply *nwReply);

        //! Read / re-read data file
        void ps_read();

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
