/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DATABASE_WRITER_H
#define BLACKCORE_DATABASE_WRITER_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/network/url.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/statusmessagelist.h"

#include <QByteArray>
#include <QList>
#include <QObject>

class QNetworkReply;

namespace BlackCore
{
    namespace Db
    {
        //! Write to the swift DB
        class BLACKCORE_EXPORT CDatabaseWriter : public QObject
        {
            Q_OBJECT

        public:
            //! Constructor
            CDatabaseWriter(const BlackMisc::Network::CUrl &baseUrl, QObject *parent);

            //! Write models to DB
            BlackMisc::CStatusMessageList asyncPublishModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! Write models to DB
            BlackMisc::CStatusMessageList asyncPublishModels(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Shutdown
            void gracefulShutdown();

        signals:
            //! Published models, the response to \sa asyncPublishModels
            void publishedModels(const BlackMisc::Simulation::CAircraftModelList &modelsPublished,
                                 const BlackMisc::Simulation::CAircraftModelList &modelsSkipped,
                                 const BlackMisc::CStatusMessageList &messages,
                                 bool sendingSuccessful, bool directWrite);

        private slots:
            //! Post response
            void ps_postModelsResponse(QNetworkReply *nwReplyPtr);

        private:
            BlackMisc::Network::CUrl m_modelPublishUrl;
            QNetworkReply           *m_pendingReply = nullptr;
            qint64                   m_replyPendingSince = -1;
            bool                     m_shutdown = false;

            //! Kill the pending reply
            bool killPendingReply();

            //! Reply timed out?
            bool isReplyOverdue() const;

            //! URL model web service
            static BlackMisc::Network::CUrl getModelPublishUrl(const BlackMisc::Network::CUrl &baseUrl);

            //! Split data array
            static QList<QByteArray> splitData(const QByteArray &data, int size);
        };
    } // ns
} // ns

#endif // guard
