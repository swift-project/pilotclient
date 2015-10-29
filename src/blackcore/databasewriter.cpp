/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "databasewriter.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/datastoreutility.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/project.h"
#include <QUrlQuery>
#include <QJsonDocument>
#include <QHttpPart>
#include <QHttpMultiPart>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    CDatabaseWriter::CDatabaseWriter(const Network::CUrl &baseUrl, QObject *parent) :
        QObject(parent),
        m_modelUrl(getModelWriteUrl(baseUrl))
    {
        this->m_networkManager = new QNetworkAccessManager(this);
        this->connect(this->m_networkManager, &QNetworkAccessManager::finished, this, &CDatabaseWriter::ps_postResponse);
    }

    CStatusMessageList CDatabaseWriter::asyncWriteModel(const CAircraftModel &model)
    {
        CStatusMessageList msg;
        if (m_shutdown)
        {
            msg.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Database writer shuts down"));
            return msg;
        }

        if (m_pendingReply)
        {
            msg.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Another write operation in progress"));
            return msg;
        }

        QUrl url(m_modelUrl.toQUrl());
        QNetworkRequest request(url);
        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
        multiPart->append(CNetworkUtils::getJsonTextMutlipart(model.toJson()));
        if (m_setup.get().dbDebugFlag()) { multiPart->append(CNetworkUtils::getMultipartWithDebugFlag()); }

        m_pendingReply = this->m_networkManager->post(request, multiPart);
        multiPart->setParent(m_pendingReply);
        return msg;
    }

    void CDatabaseWriter::gracefulShutdown()
    {
        m_shutdown = true;
        if (m_pendingReply)
        {
            m_pendingReply->abort();
            m_pendingReply = nullptr;
        }
    }

    void CDatabaseWriter::ps_postResponse(QNetworkReply *nwReplyPtr)
    {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        m_pendingReply = nullptr;

        if (m_shutdown)
        {
            nwReply->abort();
            return;
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            const QString dataFileData(nwReply->readAll().trimmed());
            nwReply->close(); // close asap

            if (dataFileData.isEmpty())
            {
                CLogMessage(this).error("No response data");
                return;
            }

            CStatusMessageList msgs;
            CVariant id;
            bool success = CDatastoreUtility::parseSwiftWriteResponse(dataFileData, msgs, id);
            CLogMessage(this).preformatted(msgs);
            Q_UNUSED(success);
        }
        else
        {
            QString error = nwReply->errorString();
            nwReply->close(); // close asap
            CLogMessage(this).error(error);
        }
    }

    Network::CUrl CDatabaseWriter::getModelWriteUrl(const Network::CUrl &baseUrl)
    {
        return baseUrl.withAppendedPath("service/swiftwritemodel.php");
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

} // namespace
