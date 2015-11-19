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
        connect(this, &CSetupReader::setupSynchronized, this, &CSetupReader::ps_setupSyncronized);
        QString localFileName;
        if (this->localBootstrapFile(localFileName))
        {
            // initialized by local file for testing
            // I do not even need to start in background here
            CLogMessage(this).info("Using local bootstrap file: %1") << localFileName;
            emit this->setupSynchronized(true);
        }
        else
        {
            this->m_bootstrapUrls.uniqueWrite()->push_back(m_setup.get().bootstrapUrls());
            this->m_updateInfoUrls.uniqueWrite()->push_back(m_setup.get().updateInfoUrls());

            this->m_networkManagerBootstrap = new QNetworkAccessManager(this);
            this->connect(this->m_networkManagerBootstrap, &QNetworkAccessManager::finished, this, &CSetupReader::ps_parseSetupFile);

            this->m_networkManagerUpdateInfo = new QNetworkAccessManager(this);
            this->connect(this->m_networkManagerUpdateInfo, &QNetworkAccessManager::finished, this, &CSetupReader::ps_parseUpdateInfoFile);

            this->start(QThread::LowPriority);
        }
    }

    CSetupReader &CSetupReader::instance()
    {
        static CSetupReader reader(QCoreApplication::instance());
        return reader;
    }

    void CSetupReader::initialize()
    {
        // start to read by myself
        CThreadedReader::initialize();
        QTimer::singleShot(500, this, &CSetupReader::ps_readSetup);
    }

    void CSetupReader::ps_readSetup()
    {
        this->threadAssertCheck();
        Q_ASSERT_X(this->m_networkManagerBootstrap, Q_FUNC_INFO, "Missing network manager");
        CUrl url(this->m_bootstrapUrls.uniqueWrite()->getNextWorkingUrl());
        if (url.isEmpty())
        {
            CLogMessage(this).warning("Cannot read setup, failed URLs: %1") << this->m_bootstrapUrls.read()->getFailedUrls();
            emit setupSynchronized(false);
            return;
        }
        QNetworkRequest request(url);
        CNetworkUtils::ignoreSslVerification(request);
        this->m_networkManagerBootstrap->get(request);
    }

    void CSetupReader::ps_readUpdateInfo()
    {
        this->threadAssertCheck();
        Q_ASSERT_X(this->m_networkManagerUpdateInfo, Q_FUNC_INFO, "Missing network manager");
        CUrl url(this->m_updateInfoUrls.uniqueWrite()->getNextWorkingUrl());
        if (url.isEmpty())
        {
            CLogMessage(this).warning("Cannot read update info, failed URLs: %1") << this->m_updateInfoUrls.read()->getFailedUrls();
            return;
        }
        QNetworkRequest request(url);
        CNetworkUtils::ignoreSslVerification(request);
        this->m_networkManagerUpdateInfo->get(request);
    }

    void CSetupReader::ps_setupSyncronized(bool success)
    {
        // trigger
        if (success)
        {
            CLogMessage(this).info("Setup synchronized, will trigger read of update information");
            QTimer::singleShot(500, this, &CSetupReader::ps_readUpdateInfo);
        }
        else
        {
            CLogMessage(this).error("Setup reading failed, hence version info will not be loaded");
        }
    }

    bool CSetupReader::localBootstrapFile(QString &fileName)
    {
        QString dir(CProject::getSwiftPrivateResourceDir());
        if (dir.isEmpty()) { return false; }

        // no version for local files, as those come withe the current code
        fileName = CFileUtils::appendFilePaths(dir, "bootstrap/bootstrap.json");
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

    void CSetupReader::ps_parseSetupFile(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        this->threadAssertCheck();
        QUrl url(nwReply->url());
        QString urlString(url.toString());
        QString replyMessage(nwReply->errorString());

        if (this->isFinishedOrShutdown())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info("Terminated loading bootstrap files");
            nwReply->abort();
            emit setupSynchronized(false);
            return; // stop, terminate straight away, ending thread
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            qint64 lastModified = this->lastModifiedMsSinceEpoch(nwReply.data());
            QString setupJson(nwReplyPtr->readAll());
            nwReplyPtr->close();
            if (setupJson.isEmpty())
            {
                CLogMessage(this).info("No bootstrap setup file at %1") << urlString;
                // try next URL
            }
            else
            {
                CGlobalSetup currentSetup(m_setup.get()); // from cache
                CGlobalSetup loadedSetup;
                loadedSetup.convertFromJson(Json::jsonObjectFromString(setupJson));
                loadedSetup.setDevelopment(isForDevelopment()); // always update, regardless what persistent setting says
                if (loadedSetup.getMSecsSinceEpoch() == 0 && lastModified > 0) { loadedSetup.setMSecsSinceEpoch(lastModified); }
                qint64 currentVersionTimestamp = currentSetup.getMSecsSinceEpoch();
                qint64 newVersionTimestamp = loadedSetup.getMSecsSinceEpoch();
                bool sameVersionLoaded = (loadedSetup == currentSetup);
                if (sameVersionLoaded)
                {
                    CLogMessage(this).info("Same setup version loaded from %1 as already in data cache %2") << urlString << m_setup.getFilename();
                    emit setupSynchronized(true);
                    return; // success
                }

                bool sameType = loadedSetup.hasSameType(currentSetup);
                bool outdatedVersionLoaded = sameType && (newVersionTimestamp  < currentVersionTimestamp);
                if (outdatedVersionLoaded)
                {
                    CLogMessage(this).info("Setup loaded from %1 outdated, older than version in data cache %2") << urlString << m_setup.getFilename();
                    // try next URL
                }
                else
                {
                    CStatusMessage m = m_setup.set(loadedSetup);
                    if (!m.isEmpty())
                    {
                        CLogMessage(this).preformatted(m);
                        emit setupSynchronized(false);
                        return; // issue with cache
                    }
                    else
                    {
                        CLogMessage(this).info("Setup: Updated data cache in %1") << this->m_setup.getFilename();
                        emit setupSynchronized(true);
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
        if (this->m_bootstrapUrls.uniqueWrite()->addFailedUrl(url))
        {
            QTimer::singleShot(500, this, &CSetupReader::ps_readSetup);
        }
        else
        {
            emit setupSynchronized(false);
        }
    }

    void CSetupReader::ps_parseUpdateInfoFile(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        this->threadAssertCheck();
        QUrl url(nwReply->url());
        QString urlString(url.toString());
        QString replyMessage(nwReply->errorString());

        if (this->isFinishedOrShutdown())
        {
            CLogMessage(this).debug() << Q_FUNC_INFO;
            CLogMessage(this).info("Terminated loading of update info");
            nwReply->abort();
            return; // stop, terminate straight away, ending thread
        }

        if (nwReply->error() == QNetworkReply::NoError)
        {
            qint64 lastModified = this->lastModifiedMsSinceEpoch(nwReply.data());
            QString setupJson(nwReplyPtr->readAll());
            nwReplyPtr->close();
            if (setupJson.isEmpty())
            {
                CLogMessage(this).info("No update info file");
                // try next URL
            }
            else
            {
                CUpdateInfo currentUpdateInfo(m_updateInfo.get()); // from cache
                CUpdateInfo loadedUpdateInfo;
                loadedUpdateInfo.convertFromJson(Json::jsonObjectFromString(setupJson));
                loadedUpdateInfo.setDevelopment(isForDevelopment()); // always update, regardless what persistent setting says
                if (loadedUpdateInfo.getMSecsSinceEpoch() == 0 && lastModified > 0) { loadedUpdateInfo.setMSecsSinceEpoch(lastModified); }
                qint64 currentVersionTimestamp = currentUpdateInfo.getMSecsSinceEpoch();
                qint64 newVersionTimestamp = loadedUpdateInfo.getMSecsSinceEpoch();
                bool sameVersionLoaded = (loadedUpdateInfo == currentUpdateInfo);
                if (sameVersionLoaded)
                {
                    CLogMessage(this).info("Same update info loaded from %1 as already in data cache %2") << urlString << m_updateInfo.getFilename();
                    return; // success
                }

                bool sameType = loadedUpdateInfo.hasSameType(currentUpdateInfo);
                bool outdatedVersionLoaded = sameType && (newVersionTimestamp  < currentVersionTimestamp);
                if (outdatedVersionLoaded)
                {
                    CLogMessage(this).info("Update info loaded from %1 outdated, older than version in data cache %2") << urlString << m_updateInfo.getFilename();
                    // try next URL
                }
                else
                {
                    CStatusMessage m = m_updateInfo.set(loadedUpdateInfo);
                    if (!m.isEmpty())
                    {
                        CLogMessage(this).preformatted(m);
                        return; // issue with cache
                    }
                    else
                    {
                        CLogMessage(this).info("Update info: Updated data cache in %1") << m_updateInfo.getFilename();
                        return; // success
                    } // cache
                } // outdated?

            } // json empty
        } // no error
        else
        {
            // network error
            CLogMessage(this).warning("Reading update info failed %1 %2") << replyMessage << urlString;
            nwReply->abort();
        }

        // try next one if any
        if (this->m_updateInfoUrls.uniqueWrite()->addFailedUrl(url))
        {
            QTimer::singleShot(500, this, &CSetupReader::ps_readSetup);
        }
    } // method

} // namespace
