/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/db/databasewriter.h"
#include "blackcore/db/databaseutils.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/statusmessage.h"

#include <QStringBuilder>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QString>
#include <QUrl>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Db;

namespace BlackCore
{
    namespace Db
    {
        CDatabaseWriter::CDatabaseWriter(const Network::CUrl &baseUrl, QObject *parent) :
            QObject(parent),
            m_modelPublishUrl(getModelPublishUrl(baseUrl))
        {
            // void
        }

        CStatusMessageList CDatabaseWriter::asyncPublishModel(const CAircraftModel &model)
        {
            return this->asyncPublishModels(CAircraftModelList({ model }));
        }

        CStatusMessageList CDatabaseWriter::asyncPublishModels(const CAircraftModelList &models)
        {
            CStatusMessageList msgs;
            if (m_shutdown)
            {
                msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, u"Database writer shuts down"));
                return msgs;
            }

            if (this->isReplyOverdue())
            {
                const bool killed = this->killPendingReply();
                if (killed)
                {
                    const CStatusMessage msg(CStatusMessage::SeverityWarning, u"Aborted outdated pending reply");
                    msgs.push_back(CStatusMessage(msg));
                    // need to let a potential receiver know it has failed
                    emit this->publishedModels(CAircraftModelList(), CAircraftModelList(), msg, false, false);
                }
            }

            if (m_pendingReply)
            {
                msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, u"Another write operation in progress"));
                return msgs;
            }

            const bool compress = models.size() > 3;
            QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
            multiPart->append(CDatabaseUtils::getJsonTextMultipart(models.toDatabaseJson(), compress));
            if (sApp->getGlobalSetup().dbDebugFlag())
            {
                multiPart->append(CDatabaseUtils::getMultipartWithDebugFlag());
            }

            QUrl url(m_modelPublishUrl.toQUrl());
            if (compress) { url.setQuery(CDatabaseUtils::getCompressedQuery()); }
            QNetworkRequest request(url);
            CNetworkUtils::ignoreSslVerification(request);
            const int logId = m_writeLog.addPendingUrl(url);
            m_pendingReply = sApp->postToNetwork(request, logId, multiPart, { this, &CDatabaseWriter::postedModelsResponse});
            m_replyPendingSince = QDateTime::currentMSecsSinceEpoch();
            return msgs;
        }

        void CDatabaseWriter::gracefulShutdown()
        {
            m_shutdown = true;
            this->killPendingReply();
        }

        const QString &CDatabaseWriter::getName()
        {
            static const QString n("Database writer");
            return n;
        }

        void CDatabaseWriter::postedModelsResponse(QNetworkReply *nwReplyPtr)
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::swiftDbWebservice()}));
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
            if (m_shutdown)
            {
                nwReply->abort();
                return;
            }

            m_pendingReply = nullptr;
            const QUrl url(nwReply->url());
            const QString urlString(url.toString());
            if (nwReply->error() == QNetworkReply::NoError)
            {
                const QString dataFileData(nwReply->readAll().trimmed());
                nwReply->close(); // close asap
                if (dataFileData.isEmpty())
                {
                    const CStatusMessageList msgs({CStatusMessage(cats, CStatusMessage::SeverityError, u"No response data from " % urlString)});
                    emit this->publishedModels(CAircraftModelList(), CAircraftModelList(), msgs, false, false);
                    return;
                }

                CAircraftModelList modelsPublished;
                CAircraftModelList modelsSkipped;
                CStatusMessageList msgs;
                bool directWrite;
                const bool sendingSuccessful = CDatastoreUtility::parseSwiftPublishResponse(dataFileData, modelsPublished, modelsSkipped, msgs, directWrite);
                const int c = CDatabaseUtils::fillInMissingAircraftAndLiveryEntities(modelsPublished);
                emit this->publishedModels(modelsPublished, modelsSkipped, msgs, sendingSuccessful, directWrite);
                if (!modelsPublished.isEmpty())
                {
                    emit this->publishedModelsSimplified(modelsPublished, directWrite);
                }
                Q_UNUSED(c);
            }
            else
            {
                const QString error = nwReply->errorString();
                nwReply->close(); // close asap
                const CStatusMessageList msgs( {CStatusMessage(cats, CStatusMessage::SeverityError, u"HTTP error: " % error)});
                emit this->publishedModels(CAircraftModelList(), CAircraftModelList(), msgs, false, false);
            }
        }

        bool CDatabaseWriter::killPendingReply()
        {
            if (!m_pendingReply) { return false; }
            m_pendingReply->abort();
            m_pendingReply = nullptr;
            m_replyPendingSince = -1;
            return true;
        }

        bool CDatabaseWriter::isReplyOverdue() const
        {
            if (m_replyPendingSince < 0 || !m_pendingReply) { return false; }
            const qint64 ms = QDateTime::currentMSecsSinceEpoch() - m_replyPendingSince;
            return ms > 7500;
        }

        CUrl CDatabaseWriter::getModelPublishUrl(const Network::CUrl &baseUrl)
        {
            return baseUrl.withAppendedPath("service/publishmodels.php");
        }

        QList<QByteArray> CDatabaseWriter::splitData(const QByteArray &data, int size)
        {
            if (data.size() <= size) { return QList<QByteArray>({data}); }
            int pos = 0, arrsize = data.size();
            QList<QByteArray> arrays;
            while (pos < arrsize)
            {
                QByteArray arr = data.mid(pos, size);
                arrays << arr;
                pos += arr.size();
            }
            return arrays;
        }
    } // ns
} // ns
