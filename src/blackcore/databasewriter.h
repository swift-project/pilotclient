/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_DATABASE_WRITER_H
#define BLACKCORE_DATABASE_WRITER_H

//! \file

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/threadedreader.h"
#include "blackmisc/simulation/aircraftmodellist.h"

#include <QNetworkReply>
#include <QJsonArray>
#include <QDateTime>

namespace BlackCore
{
    //! Write to the swift DB
    class BLACKCORE_EXPORT CDatabaseWriter : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CDatabaseWriter(const BlackMisc::Network::CUrl &baseUrl, QObject *parent);

        //! Write model to DB
        BlackMisc::CStatusMessageList asyncPublishModels(const BlackMisc::Simulation::CAircraftModelList &models);

        //! Shutdown
        void gracefulShutdown();

    signals:
        //! Published models, the response to \sa asyncPublishModels
        void published(const BlackMisc::Simulation::CAircraftModelList &modelsPublished, const BlackMisc::Simulation::CAircraftModelList &modelsSkipped, const BlackMisc::CStatusMessageList &messages);

    private slots:
        //! Post response
        void ps_postResponse(QNetworkReply *nwReplyPtr);

    private:
        BlackMisc::CData<BlackCore::Data::GlobalSetup> m_setup {this}; //!< data cache
        BlackMisc::Network::CUrl                       m_modelPublishUrl;
        QNetworkAccessManager                         *m_networkManager = nullptr;
        QNetworkReply                                 *m_pendingReply = nullptr;
        bool                                           m_shutdown = false;

        //! URL model web service
        static BlackMisc::Network::CUrl getModelPublishUrl(const BlackMisc::Network::CUrl &baseUrl);

        //! Split data array
        static QList<QByteArray> splitData(const QByteArray &data, int size);

    };
} // namespace

#endif // guard
