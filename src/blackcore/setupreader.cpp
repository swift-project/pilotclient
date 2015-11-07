/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/networkutils.h"
#include "blackmisc/sequence.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/json.h"
#include "blackmisc/project.h"
#include "blackmisc/fileutilities.h"
#include "blackmisc/logmessage.h"
#include "setupreader.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore;
using namespace BlackCore::Data;

namespace BlackCore
{
    CSetupReader::CSetupReader(QObject *owner) :
        CThreadedReader(owner, "CSetupReader")
    {
        QString localFileName;
        if (localFile(localFileName))
        {
            // initialized by local file for testing
            // I do not even need to start in background here
            CLogMessage(this).info("Using local bootstrap file: %1") << localFileName;
        }
        else
        {
            this->m_networkManager = new QNetworkAccessManager(this);
            this->connect(this->m_networkManager, &QNetworkAccessManager::finished, this, &CSetupReader::ps_parseSetupFile);
            this->start(QThread::LowPriority);
        }
    }

    void CSetupReader::initialize()
    {
        CThreadedReader::initialize();
        QTimer::singleShot(500, this, &CSetupReader::ps_read);
    }

    void CSetupReader::ps_read()
    {
        this->threadAssertCheck();
        Q_ASSERT_X(this->m_networkManager, Q_FUNC_INFO, "Missing network manager");
        CUrlList urls(getRemainingUrls());
        if (urls.isEmpty()) { return; }
        int urlsSize = urls.size();

        QUrl url(urls.getNextUrl(false));
        if (urlsSize > 1)
        {
            // more than one URL one, quick check if this can be contacted
            QString m;
            if (!CNetworkUtils::canConnect(url, m))
            {
                m_failedUrls.push_back(url);
                CLogMessage(this).warning("Cannot connect to %1") << url.toString();
                url = urls.getNextUrl();
            }
        }

        // now I have the first or second URL for reading
        if (url.isEmpty()) { return; }
        QNetworkRequest request(url);
        CNetworkUtils::ignoreSslVerification(request);

        // request
        this->m_networkManager->get(request);
    }

    bool CSetupReader::localFile(QString &fileName)
    {
        QString dir(CProject::getSwiftPrivateResourceDir());
        if (dir.isEmpty()) { return false; }

        fileName = CFileUtils::appendFilePaths(dir, "bootstrap/" + CGlobalSetup::versionString() + "/bootstrap.json");
        QString content(CFileUtils::readFileToString(fileName));
        if (content.isEmpty()) { return false; }
        CGlobalSetup s;
        s.convertFromJson(content);
        s.setDevelopment(true);
        m_setup.set(s);
        return true;
    }

    bool CSetupReader::isForDevelopment()
    {
        return CProject::useDevelopmentSetup();
    }

    CUrlList CSetupReader::getRemainingUrls() const
    {
        CUrlList urls(m_setup.get().bootstrapUrls().appendPath(appendPathAndFile()));
        urls.removeIfIn(m_failedUrls);
        return urls;
    }

    QString CSetupReader::appendPathAndFile()
    {
        return isForDevelopment() ?
               CGlobalSetup::versionString() + "/development/bootstrap.json" :
               CGlobalSetup::versionString() + "/productive/bootstrap.json";
    }

    void CSetupReader::ps_parseSetupFile(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        QUrl url(nwReply->url());
        QString urlString(url.toString());
        QString replyMessage(nwReply->errorString());

        this->threadAssertCheck();
        if (this->isFinishedOrShutdown())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info("Terminated loading bootstrap files");
            nwReply->abort();
            return; // stop, terminate straight away, ending thread
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            qint64 lastModified = -1;
            QVariant lastModifiedQv = nwReply->header(QNetworkRequest::LastModifiedHeader);
            if (lastModifiedQv.isValid() && lastModifiedQv.canConvert<QDateTime>())
            {
                lastModified = lastModifiedQv.value<QDateTime>().toMSecsSinceEpoch();
            }

            QString setupJson(nwReplyPtr->readAll());
            nwReplyPtr->close();
            if (setupJson.isEmpty())
            {
                CLogMessage(this).info("No bootstrap setup file");
                m_failedUrls.push_back(url);
                // try next URL
            }
            else
            {
                CGlobalSetup currentSetup(m_setup.get()); // from cache
                CGlobalSetup loadedSetup;
                loadedSetup.convertFromJson(Json::jsonObjectFromString(setupJson));
                loadedSetup.setDevelopment(isForDevelopment()); // always update, regardless what persistent setting says
                if (loadedSetup.getMSecsSinceEpoch() == 0 && lastModified > 0) { loadedSetup.setMSecsSinceEpoch(lastModified); }
                bool sameType = loadedSetup.hasSameType(currentSetup);
                qint64 currentVersionTimestamp = currentSetup.getMSecsSinceEpoch();
                qint64 newVersionTimestamp = loadedSetup.getMSecsSinceEpoch();
                bool sameVersionLoaded = sameType && (newVersionTimestamp  == currentVersionTimestamp);
                if (sameVersionLoaded)
                {
                    CLogMessage(this).info("Same version loaded from %1 as already in data cache %2") << urlString << CDataCache::persistentStore();
                    return; // success
                }

                bool outdatedVersionLoaded = sameType && (newVersionTimestamp  < currentVersionTimestamp);
                if (outdatedVersionLoaded)
                {
                    CLogMessage(this).info("Version loaded from %1 outdated, older than version in data cache %2") << urlString << CDataCache::persistentStore();
                    // try next URL
                }
                else
                {
                    CStatusMessage m = m_setup.set(loadedSetup);
                    if (!m.isEmpty())
                    {
                        CLogMessage(this).preformatted(m);
                        return; // issue with cache
                    }
                    else
                    {
                        CLogMessage(this).info("Updated data cache in %1") << CDataCache::persistentStore();
                        return; // success
                    } // cache
                } // outdated?

            } // json empty
        } // no error
        else
        {
            // network error
            CLogMessage(this).warning("Reading setup failed %1 %2") << replyMessage << urlString;
            nwReply->abort();
        }

        // try next one if any
        const int maxTrials = 2;
        m_failedUrls.push_back(url);
        if (m_failedUrls.size() >= maxTrials) { return; }

        int urlsNo = getRemainingUrls().size();
        if (urlsNo >= 0)
        {
            QTimer::singleShot(500, this, &CSetupReader::ps_read);
        }
    } // method

} // namespace
