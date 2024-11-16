// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/db/databasewriter.h"

#include <QByteArray>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QString>
#include <QStringBuilder>
#include <QUrl>
#include <QtGlobal>

#include "core/application.h"
#include "core/data/globalsetup.h"
#include "core/db/databaseutils.h"
#include "misc/db/datastoreutility.h"
#include "misc/logcategories.h"
#include "misc/network/networkutils.h"
#include "misc/simulation/autopublishdata.h"
#include "misc/statusmessage.h"

using namespace swift::misc;
using namespace swift::misc::db;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::core::db;

namespace swift::core::db
{
    CDatabaseWriter::CDatabaseWriter(const network::CUrl &baseUrl, QObject *parent)
        : QObject(parent), m_modelPublishUrl(CDatabaseWriter::getModelPublishUrl(baseUrl)),
          m_autoPublishUrl(CDatabaseWriter::getAutoPublishUrl(baseUrl))
    {
        // void
    }

    CStatusMessageList CDatabaseWriter::asyncPublishModel(const CAircraftModel &model, const QString &extraInfo)
    {
        return this->asyncPublishModels(CAircraftModelList({ model }), extraInfo);
    }

    CStatusMessageList CDatabaseWriter::asyncPublishModels(const CAircraftModelList &models, const QString &extraInfo)
    {
        CStatusMessageList msgs;
        if (m_shutdown || !sApp)
        {
            msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, u"Database writer shutting down"));
            return msgs;
        }

        if (this->isModelReplyOverdue())
        {
            const bool killed = this->killPendingModelReply();
            if (killed)
            {
                const CStatusMessage msg(CStatusMessage::SeverityWarning, u"Aborted outdated pending reply");
                msgs.push_back(CStatusMessage(msg));
                // need to let a potential receiver know it has failed
                emit this->publishedModels(CAircraftModelList(), CAircraftModelList(), msg, false, false);
            }
        }

        if (m_pendingModelPublishReply)
        {
            msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, u"Another write operation in progress"));
            return msgs;
        }

        const bool compress = models.size() > 3;
        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
        multiPart->append(CDatabaseUtils::getJsonTextMultipart(models.toDatabaseJson(), compress));

        if (sApp->getGlobalSetup().dbDebugFlag()) { multiPart->append(CDatabaseUtils::getMultipartWithDebugFlag()); }

        QUrl url(m_modelPublishUrl.toQUrl());
        if (compress) { url.setQuery(CDatabaseUtils::getCompressedQuery()); }
        QNetworkRequest request(url);
        const QByteArray eInfo = extraInfo.toLatin1();
        request.setRawHeader(QByteArray("swift-extrainfo"), eInfo);
        const int logId = m_writeLog.addPendingUrl(url);
        m_pendingModelPublishReply =
            sApp->postToNetwork(request, logId, multiPart, { this, &CDatabaseWriter::postedModelsResponse });
        m_modelReplyPendingSince = QDateTime::currentMSecsSinceEpoch();
        return msgs;
    }

    CStatusMessageList CDatabaseWriter::asyncAutoPublish(const CAutoPublishData &data)
    {
        CStatusMessageList msgs;
        if (m_shutdown || !sApp)
        {
            msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, u"Database writer shutting down"));
            return msgs;
        }

        if (data.isEmpty())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, u"No auto update data"));
            return msgs;
        }

        const QString json = data.toDatabaseJson();
        const bool compress = json.size() > 2048;
        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
        multiPart->append(CDatabaseUtils::getJsonTextMultipart(json, compress));
        if (sApp->getGlobalSetup().dbDebugFlag())
        {
            // add debug flag
            multiPart->append(CDatabaseUtils::getMultipartWithDebugFlag());
        }

        // QUrl url("https://192.168.0.153/service/publishauto.php");
        QUrl url(m_autoPublishUrl.toQUrl());
        QUrlQuery query;
        if (compress) { query = CDatabaseUtils::getCompressedQuery(); }
        url.setQuery(query);

        QNetworkRequest request(url);
        const int logId = m_writeLog.addPendingUrl(url);
        m_pendingAutoPublishReply =
            sApp->postToNetwork(request, logId, multiPart, { this, &CDatabaseWriter::postedAutoPublishResponse });
        m_autoPublishReplyPendingSince = QDateTime::currentMSecsSinceEpoch();
        return msgs;
    }

    void CDatabaseWriter::gracefulShutdown()
    {
        m_shutdown = true;
        this->killPendingModelReply();
    }

    const QString &CDatabaseWriter::getName()
    {
        static const QString n("Database writer");
        return n;
    }

    const QStringList &CDatabaseWriter::getLogCategories()
    {
        static const QStringList cats { CLogCategories::swiftDbWebservice(), CLogCategories::webservice() };
        return cats;
    }

    void CDatabaseWriter::postedModelsResponse(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        if (m_shutdown || !sApp)
        {
            nwReply->abort();
            return;
        }

        m_pendingModelPublishReply = nullptr;
        const QUrl url(nwReply->url());
        const QString urlString(url.toString());
        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString responseData(nwReply->readAll().trimmed());
            nwReply->close(); // close asap
            if (responseData.isEmpty())
            {
                const CStatusMessageList msgs(
                    { CStatusMessage(this, CStatusMessage::SeverityError, u"No response data from " % urlString) });
                emit this->publishedModels(CAircraftModelList(), CAircraftModelList(), msgs, false, false);
                return;
            }

            CAircraftModelList modelsPublished;
            CAircraftModelList modelsSkipped;
            CStatusMessageList msgs;
            bool directWrite;
            const bool sendingSuccessful =
                parseSwiftPublishResponse(responseData, modelsPublished, modelsSkipped, msgs, directWrite);
            const int c = CDatabaseUtils::fillInMissingAircraftAndLiveryEntities(modelsPublished);

            emit this->publishedModels(modelsPublished, modelsSkipped, msgs, sendingSuccessful, directWrite);

            if (!modelsPublished.isEmpty()) { emit this->publishedModelsSimplified(modelsPublished, directWrite); }
            Q_UNUSED(c);
        }
        else
        {
            const QString error = nwReply->errorString();
            nwReply->close(); // close asap
            const CStatusMessageList msgs(
                { CStatusMessage(this, CStatusMessage::SeverityError, u"HTTP error: " % error) });
            emit this->publishedModels(CAircraftModelList(), CAircraftModelList(), msgs, false, false);
        }
    }

    void CDatabaseWriter::postedAutoPublishResponse(QNetworkReply *nwReplyPtr)
    {
        static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategories::swiftDbWebservice() }));
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        if (m_shutdown || !sApp)
        {
            nwReply->abort();
            return;
        }

        m_pendingAutoPublishReply = nullptr;
        const QUrl url(nwReply->url());
        const QString urlString(url.toString());
        const QString responseData(nwReply->readAll().trimmed());
        const QString error = nwReply->errorString();
        nwReply->close(); // close asap

        CStatusMessageList msgs;
        const bool ok = CDatastoreUtility::parseAutoPublishResponse(responseData, msgs);

        if (nwReply->error() == QNetworkReply::NoError)
        {
            // no error
        }
        else { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"HTTP error: " % error)); }

        emit this->autoPublished(ok, urlString, msgs);
    }

    bool CDatabaseWriter::killPendingModelReply()
    {
        if (!m_pendingModelPublishReply) { return false; }
        m_pendingModelPublishReply->abort();
        m_pendingModelPublishReply = nullptr;
        m_modelReplyPendingSince = -1;
        return true;
    }

    bool CDatabaseWriter::isModelReplyOverdue() const
    {
        if (m_modelReplyPendingSince < 0 || !m_pendingModelPublishReply) { return false; }
        const qint64 ms = QDateTime::currentMSecsSinceEpoch() - m_modelReplyPendingSince;
        return ms > 7500;
    }

    CUrl CDatabaseWriter::getModelPublishUrl(const network::CUrl &baseUrl)
    {
        return baseUrl.withAppendedPath("service/publishmodels.php");
    }

    CUrl CDatabaseWriter::getAutoPublishUrl(const CUrl &baseUrl)
    {
        return baseUrl.withAppendedPath("service/publishauto.php");
    }

    QList<QByteArray> CDatabaseWriter::splitData(const QByteArray &data, int size)
    {
        if (data.size() <= size) { return QList<QByteArray>({ data }); }
        int pos = 0, arrsize = data.size();
        QList<QByteArray> arrays;
        while (pos < arrsize)
        {
            const QByteArray arr = data.mid(pos, size);
            arrays << arr;
            pos += arr.size();
        }
        return arrays;
    }

    bool CDatabaseWriter::parseSwiftPublishResponse(const QString &jsonResponse, CAircraftModelList &publishedModels,
                                                    CAircraftModelList &skippedModels, CStatusMessageList &messages,
                                                    bool &directWrite)
    {
        directWrite = false;

        if (jsonResponse.isEmpty())
        {
            messages.push_back(CStatusMessage(static_cast<CDatabaseWriter *>(nullptr), CStatusMessage::SeverityError,
                                              u"Empty JSON data for published models"));
            return false;
        }

        const QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonResponse.toUtf8()));

        // array of messages only
        if (jsonDoc.isArray())
        {
            const CStatusMessageList msgs(CStatusMessageList::fromDatabaseJson(jsonDoc.array()));
            messages.push_back(msgs);
            return true;
        }

        // no object -> most likely some fucked up HTML string with the PHP error
        if (!jsonDoc.isObject())
        {
            const QString phpError(CNetworkUtils::removeHtmlPartsFromPhpErrorMessage(jsonResponse));
            messages.push_back(
                CStatusMessage(static_cast<CDatabaseWriter *>(nullptr), CStatusMessage::SeverityError, phpError));
            return false;
        }

        // fully blown object
        QJsonObject json(jsonDoc.object());
        bool hasData = false;
        if (json.contains("msgs"))
        {
            const QJsonValue msgJson(json.take("msgs"));
            const CStatusMessageList msgs(CStatusMessageList::fromDatabaseJson(msgJson.toArray()));
            if (!msgs.isEmpty())
            {
                messages.push_back(msgs);
                hasData = true;
            }
        }

        // direct write means models written, otherwise CRs
        if (json.contains("directWrite"))
        {
            const QJsonValue dw(json.take("directWrite"));
            directWrite = dw.toBool(false);
        }

        if (json.contains("publishedModels"))
        {
            const QJsonValue publishedJson(json.take("publishedModels"));
            const CAircraftModelList published = CAircraftModelList::fromDatabaseJson(publishedJson.toArray(), "");
            if (!published.isEmpty())
            {
                publishedModels.push_back(published);
                hasData = true;
            }
        }

        if (json.contains("skippedModels"))
        {
            const QJsonValue skippedJson(json.take("skippedModels"));
            const CAircraftModelList skipped = CAircraftModelList::fromDatabaseJson(skippedJson.toArray(), "");
            if (!skipped.isEmpty())
            {
                skippedModels.push_back(skipped);
                hasData = true;
            }
        }

        if (!hasData)
        {
            messages.push_back(CStatusMessage(static_cast<CDatabaseWriter *>(nullptr), CStatusMessage::SeverityError,
                                              u"Received response, but no JSON data"));
        }

        return hasData;
    }
} // namespace swift::core::db
