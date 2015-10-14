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
#include "blackmisc/threadedreader.h"
#include "blackmisc/simulation/aircraftmodel.h"

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
        BlackMisc::CStatusMessageList asyncWriteModel(const BlackMisc::Simulation::CAircraftModel &model);

        //! Shutdown
        void gracefulShutdown();

    private slots:
        //! Post response
        void ps_postResponse(QNetworkReply *nwReplyPtr);

    private:
        BlackMisc::Network::CUrl m_modelUrl;
        QNetworkAccessManager *m_networkManager = nullptr;
        QNetworkReply *m_pendingReply = nullptr;
        bool m_shutdown = false;
        bool m_phpDebug = true;

        //! URL model web service
        static BlackMisc::Network::CUrl getModelWriteUrl(const BlackMisc::Network::CUrl &baseUrl);

        //! Split data array
        static QList<QByteArray> splitData(const QByteArray &data, int size);

    };
} // namespace

#endif // guard
