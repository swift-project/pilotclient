// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_DATABASE_WRITER_H
#define BLACKCORE_DATABASE_WRITER_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/network/urlloglist.h"
#include "blackmisc/network/url.h"
#include "blackmisc/statusmessagelist.h"

#include <QByteArray>
#include <QList>
#include <QObject>

class QNetworkReply;

namespace BlackMisc::Simulation
{
    class CAutoPublishData;
}
namespace BlackCore::Db
{
    //! Write to the swift DB
    class BLACKCORE_EXPORT CDatabaseWriter : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CDatabaseWriter(const BlackMisc::Network::CUrl &baseUrl, QObject *parent);

        //! Write model to DB
        BlackMisc::CStatusMessageList asyncPublishModel(const BlackMisc::Simulation::CAircraftModel &model, const QString &extraInfo);

        //! Write models to DB
        BlackMisc::CStatusMessageList asyncPublishModels(const BlackMisc::Simulation::CAircraftModelList &models, const QString &extraInfo);

        //! Write auto publis data
        BlackMisc::CStatusMessageList asyncAutoPublish(const BlackMisc::Simulation::CAutoPublishData &data);

        //! Shutdown
        void gracefulShutdown();

        //! Shutting down?
        bool isShuttingDown() const { return m_shutdown; }

        //! Name of the worker
        const QString &getName();

        //! Write log
        const BlackMisc::Network::CUrlLogList &getWriteLog() const { return m_writeLog; }

        //! Log categories
        static const QStringList &getLogCategories();

    signals:
        //! Published models, the response to \sa asyncPublishModels
        void publishedModels(const BlackMisc::Simulation::CAircraftModelList &modelsPublished,
                             const BlackMisc::Simulation::CAircraftModelList &modelsSkipped,
                             const BlackMisc::CStatusMessageList &messages,
                             bool sendingSuccessful, bool directWrite);

        //! Published models, simplified version of publishedModels
        void publishedModelsSimplified(const BlackMisc::Simulation::CAircraftModelList &modelsPublished, bool directWrite);

        //! Auto publishing completed
        void autoPublished(bool success, const QString &url, const BlackMisc::CStatusMessageList &msgs);

    private:
        BlackMisc::Network::CUrlLogList m_writeLog;
        BlackMisc::Network::CUrl m_modelPublishUrl; //!< model publishing
        BlackMisc::Network::CUrl m_autoPublishUrl; //!< auto publish data
        QNetworkReply *m_pendingModelPublishReply = nullptr;
        QNetworkReply *m_pendingAutoPublishReply = nullptr;
        qint64 m_modelReplyPendingSince = -1;
        qint64 m_autoPublishReplyPendingSince = -1;
        bool m_shutdown = false;

        //! Post response for models
        void postedModelsResponse(QNetworkReply *nwReplyPtr);

        //! Post response for auto publish
        void postedAutoPublishResponse(QNetworkReply *nwReplyPtr);

        //! Kill the pending reply
        bool killPendingModelReply();

        //! Reply timed out?
        bool isModelReplyOverdue() const;

        //! URL model web service
        static BlackMisc::Network::CUrl getModelPublishUrl(const BlackMisc::Network::CUrl &baseUrl);

        //! URL auto publish web service
        static BlackMisc::Network::CUrl getAutoPublishUrl(const BlackMisc::Network::CUrl &baseUrl);

        //! Split data array
        static QList<QByteArray> splitData(const QByteArray &data, int size);

        //! Get data from a DB response
        static bool parseSwiftPublishResponse(const QString &jsonResponse,
                                              BlackMisc::Simulation::CAircraftModelList &publishedModels,
                                              BlackMisc::Simulation::CAircraftModelList &skippedModels,
                                              BlackMisc::CStatusMessageList &messages, bool &directWrite);
    };
} // ns

#endif // guard
