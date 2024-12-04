// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DATABASE_WRITER_H
#define SWIFT_CORE_DATABASE_WRITER_H

#include <QByteArray>
#include <QList>
#include <QObject>

#include "core/swiftcoreexport.h"
#include "misc/network/url.h"
#include "misc/network/urlloglist.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/statusmessagelist.h"

class QNetworkReply;

namespace swift::misc::simulation
{
    class CAutoPublishData;
}
namespace swift::core::db
{
    //! Write to the swift DB
    class SWIFT_CORE_EXPORT CDatabaseWriter : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CDatabaseWriter(const swift::misc::network::CUrl &baseUrl, QObject *parent);

        //! Write model to DB
        swift::misc::CStatusMessageList asyncPublishModel(const swift::misc::simulation::CAircraftModel &model,
                                                          const QString &extraInfo);

        //! Write models to DB
        swift::misc::CStatusMessageList asyncPublishModels(const swift::misc::simulation::CAircraftModelList &models,
                                                           const QString &extraInfo);

        //! Write auto publis data
        swift::misc::CStatusMessageList asyncAutoPublish(const swift::misc::simulation::CAutoPublishData &data);

        //! Shutdown
        void gracefulShutdown();

        //! Shutting down?
        bool isShuttingDown() const { return m_shutdown; }

        //! Name of the worker
        const QString &getName();

        //! Write log
        const swift::misc::network::CUrlLogList &getWriteLog() const { return m_writeLog; }

        //! Log categories
        static const QStringList &getLogCategories();

    signals:
        //! Published models, the response to \sa asyncPublishModels
        void publishedModels(const swift::misc::simulation::CAircraftModelList &modelsPublished,
                             const swift::misc::simulation::CAircraftModelList &modelsSkipped,
                             const swift::misc::CStatusMessageList &messages, bool sendingSuccessful, bool directWrite);

        //! Published models, simplified version of publishedModels
        void publishedModelsSimplified(const swift::misc::simulation::CAircraftModelList &modelsPublished,
                                       bool directWrite);

        //! Auto publishing completed
        void autoPublished(bool success, const QString &url, const swift::misc::CStatusMessageList &msgs);

    private:
        swift::misc::network::CUrlLogList m_writeLog;
        swift::misc::network::CUrl m_modelPublishUrl; //!< model publishing
        swift::misc::network::CUrl m_autoPublishUrl; //!< auto publish data
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
        static swift::misc::network::CUrl getModelPublishUrl(const swift::misc::network::CUrl &baseUrl);

        //! URL auto publish web service
        static swift::misc::network::CUrl getAutoPublishUrl(const swift::misc::network::CUrl &baseUrl);

        //! Split data array
        static QList<QByteArray> splitData(const QByteArray &data, int size);

        //! Get data from a DB response
        static bool parseSwiftPublishResponse(const QString &jsonResponse,
                                              swift::misc::simulation::CAircraftModelList &publishedModels,
                                              swift::misc::simulation::CAircraftModelList &skippedModels,
                                              swift::misc::CStatusMessageList &messages, bool &directWrite);
    };
} // namespace swift::core::db

#endif // SWIFT_CORE_DATABASE_WRITER_H
