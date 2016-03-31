/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/sequence.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/json.h"
#include "blackmisc/buildconfig.h"
#include "blackmisc/fileutils.h"
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
    CSetupReader::CSetupReader(QObject *parent) :
        QObject(parent)
    {
        connect(this, &CSetupReader::setupSynchronized, this, &CSetupReader::ps_setupSyncronized);
        connect(this, &CSetupReader::updateInfoSynchronized, this, &CSetupReader::ps_versionInfoSyncronized);
    }

    QList<QCommandLineOption> CSetupReader::getCmdLineOptions() const
    {
        return  QList<QCommandLineOption>
        {
            {
                this->m_cmdBootstrapUrl,
                this->m_cmdBootstrapMode
            }
        };
    }

    CStatusMessage CSetupReader::asyncLoad()
    {
        if (this->readLocalBootstrapFile(this->m_localSetupFileValue))
        {
            // initialized by local file for testing
            emit this->setupSynchronized(true);
            return CStatusMessage(this, CStatusMessage::SeverityInfo, "Using local bootstrap file: " + this->m_localSetupFileValue);
        }

        m_setup.synchronize(); // make sure it is loaded
        if (this->m_bootstrapMode == CacheOnly)
        {

            CGlobalSetup currentSetup = m_setup.getCopy();
            this->m_updateInfoUrls = currentSetup.getUpdateInfoFileUrls();
            emit this->setupSynchronized(true);
            emit this->updateInfoSynchronized(true);
            return CStatusMessage(this, CStatusMessage::SeverityInfo, "Cache only setup, using it as it is");
        }

        this->m_bootstrapUrls.clear(); // clean up previous values

        // web URL
        if (!this->m_bootstrapUrlFileValue.isEmpty())
        {
            // start with the one from cmd args
            this->m_bootstrapUrls.push_front(CUrl(this->m_bootstrapUrlFileValue));
        }

        // if ever loaded add those URLs
        const CGlobalSetup currentSetup = m_setup.getCopy();
        if (this->m_bootstrapMode != Explicit)
        {
            // also use previously cached URLs
            this->m_bootstrapUrls.push_back(currentSetup.getBootstrapFileUrls());

            // fail over if still empty
            //! \todo do we want to keep this or use a cmd line flag to enable the behaviour. Risk here to use an undesired setup
            if (this->m_bootstrapUrls.isEmpty())
            {
                CGlobalSetup resourceSetup(CGlobalSetup::fromJsonFile(
                                               CBuildConfig::getBootstrapResourceFile()
                                           ));
                this->m_bootstrapUrls.push_back(resourceSetup.getBootstrapFileUrls());
            }
        }

        this->m_bootstrapUrls.removeDuplicates(); // clean up
        if (this->m_bootstrapUrls.isEmpty())
        {
            // after all still empty
            return CStatusMessage(this, CStatusMessage::SeverityError, "No bootstrap URLs, cannot load setup");
        }
        else
        {
            this->ps_readSetup(); // start reading
            return CStatusMessage(this, CStatusMessage::SeverityInfo, "Will start loading setup");
        }
    }

    bool CSetupReader::parseCmdLineArguments()
    {
        this->m_bootstrapUrlFileValue = CGlobalSetup::buildBootstrapFileUrl(
                                            sApp->getParserValue(this->m_cmdBootstrapUrl)
                                        );
        this->m_bootstrapMode = stringToEnum(sApp->getParserValue(this->m_cmdBootstrapMode));
        QUrl url(this->m_bootstrapUrlFileValue);

        // check on local file
        if (url.isLocalFile())
        {
            this->m_localSetupFileValue = url.toLocalFile();
            QFile f(this->m_localSetupFileValue);
            if (!f.exists())
            {
                sApp->cmdLineErrorMessage("File " + this->m_localSetupFileValue + " does not exist");
                return false;
            }
        }

        // check on explicit URL
        if (this->m_bootstrapMode == Explicit)
        {
            if (!url.isLocalFile())
            {
                if (!CNetworkUtils::canConnect(url))
                {
                    sApp->cmdLineErrorMessage("URL " + url.toString() + " not reachable");
                    return false;
                }
            }
        }
        return true;
    }

    void CSetupReader::gracefulShutdown()
    {
        this->m_shutdown = true;
    }

    void CSetupReader::ps_readSetup()
    {
        const CUrl url(this->m_bootstrapUrls.obtainNextWorkingUrl());
        if (url.isEmpty())
        {
            CLogMessage(this).warning("Cannot read setup, URLs: %1, failed URLs: %2")
                    << this->m_bootstrapUrls
                    << this->m_bootstrapUrls.getFailedUrls();
            emit setupSynchronized(false);
            return;
        }
        if (m_shutdown) { return; }
        sApp->getFromNetwork(url.toNetworkRequest(), { this, &CSetupReader::ps_parseSetupFile });
    }

    void CSetupReader::ps_readUpdateInfo()
    {
        const CUrl url(this->m_updateInfoUrls.obtainNextWorkingUrl());
        if (url.isEmpty())
        {
            CLogMessage(this).warning("Cannot read update info, URLs: %1, failed URLs: %2")
                    << this->m_updateInfoUrls
                    << this->m_updateInfoUrls.getFailedUrls();
            emit updateInfoSynchronized(false);
            return;
        }
        if (m_shutdown) { return; }
        sApp->getFromNetwork(url.toNetworkRequest(), { this, &CSetupReader::ps_parseUpdateInfoFile});
    }

    void CSetupReader::ps_setupSyncronized(bool success)
    {
        // trigger consecutive read
        this->m_setupSyncronized = success;
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

    void CSetupReader::ps_versionInfoSyncronized(bool success)
    {
        this->m_updateInfoSyncronized = success;
    }

    void CSetupReader::ps_setupChanged()
    {
        // settings have changed on disk
    }

    CSetupReader::BootstrapMode CSetupReader::stringToEnum(const QString &s)
    {
        const QString bsm(s.toLower().trimmed());
        if (bsm.startsWith("expl")) return Explicit;
        if (bsm.startsWith("cache")) return CacheOnly;
        return Default;
    }

    bool CSetupReader::readLocalBootstrapFile(QString &fileName)
    {
        if (fileName.isEmpty()) { return false; }
        QString fn;
        QFile file(fileName);
        if (!file.exists())
        {
            // relative name?
            QString dir(sApp->getCmdSwiftPrivateSharedDir());
            if (dir.isEmpty()) { return false; }

            // no version for local files, as those come withe the current code
            fn = CFileUtils::appendFilePaths(dir, "bootstrap/bootstrap.json");
        }
        else
        {
            fn = fileName;
        }

        QString content(CFileUtils::readFileToString(fn));
        if (content.isEmpty()) { return false; }
        CGlobalSetup s;
        s.convertFromJson(content);
        s.setDevelopment(true);
        m_setup.set(s);
        return true;
    }

    void CSetupReader::ps_parseSetupFile(QNetworkReply *nwReplyPtr)
    {
        // wrap pointer, make sure any exit cleans up reply
        // required to use delete later as object is created in a different thread
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);
        if (m_shutdown) { return; }

        QUrl url(nwReply->url());
        QString urlString(url.toString());
        QString replyMessage(nwReply->errorString());

        if (nwReply->error() == QNetworkReply::NoError)
        {
            qint64 lastModified = CNetworkUtils::lastModifiedMsSinceEpoch(nwReply.data());
            QString setupJson(nwReplyPtr->readAll());
            nwReplyPtr->close();
            if (setupJson.isEmpty())
            {
                CLogMessage(this).info("No bootstrap setup file at %1") << urlString;
                // try next URL
            }
            else
            {
                const CGlobalSetup currentSetup = m_setup.get();
                this->m_updateInfoUrls = currentSetup.getUpdateInfoFileUrls();
                CGlobalSetup loadedSetup;
                loadedSetup.convertFromJson(Json::jsonObjectFromString(setupJson));
                loadedSetup.markAsLoaded(true);
                if (lastModified > 0 && lastModified > loadedSetup.getMSecsSinceEpoch()) { loadedSetup.setMSecsSinceEpoch(lastModified); }
                bool sameVersionLoaded = (loadedSetup == currentSetup);
                if (sameVersionLoaded)
                {
                    CLogMessage(this).info("Same setup version loaded from %1 as already in data cache %2") << urlString << m_setup.getFilename();
                    emit setupSynchronized(true);
                    return; // success
                }

                qint64 currentVersionTimestamp = currentSetup.getMSecsSinceEpoch();
                qint64 newVersionTimestamp = loadedSetup.getMSecsSinceEpoch();
                bool outdatedVersionLoaded = (newVersionTimestamp  < currentVersionTimestamp);
                if (this->m_bootstrapMode != Explicit && outdatedVersionLoaded)
                {
                    CLogMessage(this).info("Setup loaded from %1 outdated, older than version in data cache %2") << urlString << m_setup.getFilename();
                    // try next URL
                }
                else
                {
                    CStatusMessage m = m_setup.set(loadedSetup, loadedSetup.getMSecsSinceEpoch());
                    if (m.isWarningOrAbove())
                    {
                        m.setCategories(getLogCategories());
                        CLogMessage::preformatted(m);
                        emit setupSynchronized(false);
                        return; // issue with cache
                    }
                    else
                    {
                        this->m_updateInfoUrls = loadedSetup.getUpdateInfoFileUrls();
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
        if (this->m_bootstrapUrls.addFailedUrl(url))
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
        if (m_shutdown) { return; }

        QUrl url(nwReply->url());
        QString urlString(url.toString());
        QString replyMessage(nwReply->errorString());

        if (nwReply->error() == QNetworkReply::NoError)
        {
            qint64 lastModified = CNetworkUtils::lastModifiedMsSinceEpoch(nwReply.data());
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
                if (lastModified > 0 && lastModified > loadedUpdateInfo.getMSecsSinceEpoch()) { loadedUpdateInfo.setMSecsSinceEpoch(lastModified); }
                bool sameVersionLoaded = (loadedUpdateInfo == currentUpdateInfo);
                if (sameVersionLoaded)
                {
                    CLogMessage(this).info("Same update info version loaded from %1 as already in data cache %2") << urlString << m_setup.getFilename();
                    emit updateInfoSynchronized(true);
                    return; // success
                }

                qint64 currentVersionTimestamp = currentUpdateInfo.getMSecsSinceEpoch();
                qint64 newVersionTimestamp = loadedUpdateInfo.getMSecsSinceEpoch();
                bool outdatedVersionLoaded = (newVersionTimestamp  < currentVersionTimestamp);
                if (outdatedVersionLoaded)
                {
                    CLogMessage(this).info("Update info loaded from %1 outdated, older than version in data cache %2") << urlString << m_updateInfo.getFilename();
                    // try next URL
                }
                else
                {
                    CStatusMessage m = m_updateInfo.set(loadedUpdateInfo, loadedUpdateInfo.getMSecsSinceEpoch());
                    if (!m.isEmpty())
                    {
                        m.setCategories(getLogCategories());
                        CLogMessage::preformatted(m);
                        emit updateInfoSynchronized(false);
                        return; // issue with cache
                    }
                    else
                    {
                        CLogMessage(this).info("Update info: Updated data cache in %1") << m_updateInfo.getFilename();
                        emit updateInfoSynchronized(true);
                        return; // success
                    } // cache
                } // outdated

            } // json empty
        } // no error
        else
        {
            // network error
            CLogMessage(this).warning("Reading update info failed %1 %2") << replyMessage << urlString;
            nwReply->abort();
        }

        // try next one if any
        if (this->m_updateInfoUrls.addFailedUrl(url))
        {
            QTimer::singleShot(500, this, &CSetupReader::ps_readSetup);
        }
        else
        {
            emit updateInfoSynchronized(false);
        }
    } // function

    const CLogCategoryList &CSetupReader::getLogCategories()
    {
        static const CLogCategoryList cats({ CLogCategory("swift.setupreader"), CLogCategory::webservice()});
        return cats;
    }

    CGlobalSetup CSetupReader::getSetup() const
    {
        return m_setup.getCopy();
    }

    CUpdateInfo CSetupReader::getUpdateInfo() const
    {
        return m_updateInfo.getCopy();
    }
} // namespace
