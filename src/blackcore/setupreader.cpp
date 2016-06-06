/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/setupreader.h"
#include "blackmisc/compare.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/json.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/network/url.h"
#include "blackmisc/statusmessage.h"

#include <QByteArray>
#include <QFile>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QTimer>
#include <QUrl>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore;
using namespace BlackCore::Data;

namespace BlackCore
{
    CSetupReader::CSetupReader(QObject *parent) :
        QObject(parent)
    { }

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

    CStatusMessageList CSetupReader::asyncLoad()
    {
        CStatusMessageList msgs;
        if (this->readLocalBootstrapFile(this->m_localSetupFileValue))
        {
            // initialized by local file for testing
            msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityInfo, "Using local bootstrap file: " + this->m_localSetupFileValue));
            msgs.push_back(this->manageSetupAvailability(false, true));
            return msgs;
        }

        m_setup.synchronize(); // make sure it is loaded
        CGlobalSetup cachedSetup = m_setup.get();
        const bool cacheAvailable = cachedSetup.wasLoaded();
        msgs.push_back(cacheAvailable ?
                       CStatusMessage(this, CStatusMessage::SeverityInfo , "Cached setup syncronized and contains data") :
                       CStatusMessage(this, CStatusMessage::SeverityInfo , "Cached setup syncronized, but no data")
                      );
        if (this->m_bootstrapMode == CacheOnly)
        {
            this->m_updateInfoUrls = cachedSetup.getUpdateInfoFileUrls();
            msgs.push_back(cacheAvailable ?
                           CStatusMessage(this, CStatusMessage::SeverityInfo, "Cache only setup, using it as it is") :
                           CStatusMessage(this, CStatusMessage::SeverityError, "Cache only setup, but cache is empty")
                          );
            msgs.push_back(this->manageSetupAvailability(false, false));
            return msgs;
        }

        this->m_bootstrapUrls.clear(); // clean up previous values

        // web URL
        if (!this->m_bootstrapUrlFileValue.isEmpty())
        {
            // start with the one from cmd args
            this->m_bootstrapUrls.push_front(CUrl(this->m_bootstrapUrlFileValue));
        }

        // if ever loaded add those URLs
        if (cacheAvailable)
        {
            if (this->m_bootstrapMode != Explicit)
            {
                // also use previously cached URLs
                const CUrlList bootstrapCacheUrls(cachedSetup.getBootstrapFileUrls());
                this->m_bootstrapUrls.push_back(bootstrapCacheUrls);
                msgs.push_back(bootstrapCacheUrls.isEmpty() ?
                               CStatusMessage(this, CStatusMessage::SeverityWarning, "No bootsrap URLs in cache") :
                               CStatusMessage(this, CStatusMessage::SeverityInfo, "Adding " + QString::number(bootstrapCacheUrls.size()) + " bootstrap URLs from cache"));
            }
        }
        else
        {
            msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityInfo, "Empty cache, will not add URLs"));
        }

        this->m_bootstrapUrls.removeDuplicates(); // clean up
        if (this->m_bootstrapUrls.isEmpty())
        {
            // after all still empty
            msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityError, "No bootstrap URLs, cannot load setup"));
        }
        else
        {
            CStatusMessageList readMsgs = triggerReadSetup();
            if (cacheAvailable && readMsgs.isFailure())
            {
                // error but cache is available, we can continue
                readMsgs.clampSeverity(CStatusMessage::SeverityWarning);
                msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityWarning, "Loading setup failed, but cache is available, will continue"));
                msgs.push_back(readMsgs);
            }
            else
            {
                msgs.push_back(readMsgs);
            }
        }
        return msgs;
    }

    bool CSetupReader::parseCmdLineArguments()
    {
        this->m_bootstrapUrlFileValue = CGlobalSetup::buildBootstrapFileUrl(
                                            sApp->getParserValue(this->m_cmdBootstrapUrl)
                                        );
        QUrl url(this->m_bootstrapUrlFileValue);
        const QString urlString(url.toString());
        this->m_bootstrapMode = stringToEnum(sApp->getParserValue(this->m_cmdBootstrapMode));
        if (urlString.isEmpty() && this->m_bootstrapMode == Explicit)
        {
            this->m_bootstrapMode = Implicit; // no URL, we use implicit mode
        }

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
                    sApp->cmdLineErrorMessage("URL " + urlString + " not reachable");
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
        const CStatusMessageList msgs(this->triggerReadSetup());
        if (!msgs.isSuccess())
        {
            CLogMessage::preformatted(msgs);
        }
    }

    CStatusMessageList CSetupReader::triggerReadSetup()
    {
        if (m_shutdown) { return CStatusMessage(this, CStatusMessage::SeverityError, "shutdown"); }
        const CUrl url(this->m_bootstrapUrls.obtainNextWorkingUrl());
        if (url.isEmpty())
        {
            const CStatusMessage m(this, CStatusMessage::SeverityError,
                                   "Cannot read setup, URLs: " + this->m_bootstrapUrls.toQString() +
                                   " failed URLs: " + this->m_bootstrapUrls.getFailedUrls().toQString());
            CStatusMessageList msgs(m);
            msgs.push_back(this->manageSetupAvailability(false, false));
            return msgs;
        }
        const CStatusMessage m(this, CStatusMessage::SeverityInfo, "Start reading URL: " + url.toQString());
        sApp->getFromNetwork(url.toNetworkRequest(), { this, &CSetupReader::ps_parseSetupFile });
        return m;
    }

    void CSetupReader::ps_readUpdateInfo()
    {
        const CUrl url(this->m_updateInfoUrls.obtainNextWorkingUrl());
        if (url.isEmpty())
        {
            CLogMessage(this).warning("Cannot read update info, URLs: %1, failed URLs: %2")
                    << this->m_updateInfoUrls
                    << this->m_updateInfoUrls.getFailedUrls();
            this->manageUpdateAvailability(false);
            return;
        }
        if (m_shutdown) { return; }
        sApp->getFromNetwork(url.toNetworkRequest(), { this, &CSetupReader::ps_parseUpdateInfoFile});
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
        return Implicit;
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
                CGlobalSetup loadedSetup;
                loadedSetup.convertFromJson(Json::jsonObjectFromString(setupJson));
                loadedSetup.markAsLoaded(true);
                if (lastModified > 0 && lastModified > loadedSetup.getMSecsSinceEpoch()) { loadedSetup.setMSecsSinceEpoch(lastModified); }
                bool sameVersionLoaded = (loadedSetup == currentSetup);
                if (sameVersionLoaded)
                {
                    this->m_updateInfoUrls = currentSetup.getUpdateInfoFileUrls(); // defaults
                    CLogMessage(this).info("Same setup version loaded from %1 as already in data cache %2") << urlString << m_setup.getFilename();
                    CLogMessage::preformatted(this->manageSetupAvailability(true));
                    return; // success
                }

                // in the past I used to do a timestamp comparison here and skipped further laoding
                // with changed files from a different URL this was wrongly assuming outdated loaded files and was removed
                CStatusMessage m = m_setup.set(loadedSetup, loadedSetup.getMSecsSinceEpoch());
                if (m.isWarningOrAbove())
                {
                    m.setCategories(getLogCategories());
                    CLogMessage::preformatted(m);
                    CLogMessage::preformatted(this->manageSetupAvailability(false));
                    return; // issue with cache
                }
                else
                {
                    this->m_updateInfoUrls = loadedSetup.getUpdateInfoFileUrls();
                    CLogMessage(this).info("Setup: Updated data cache in %1") << this->m_setup.getFilename();
                    CLogMessage::preformatted(this->manageSetupAvailability(true));
                    return; // success
                } // cache

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
            CLogMessage::preformatted(manageSetupAvailability(false));
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
                    this->manageUpdateAvailability(true);
                    return; // success
                }

                CStatusMessage m = m_updateInfo.set(loadedUpdateInfo, loadedUpdateInfo.getMSecsSinceEpoch());
                if (!m.isEmpty())
                {
                    m.setCategories(getLogCategories());
                    CLogMessage::preformatted(m);
                    this->manageUpdateAvailability(false);
                    return; // issue with cache
                }
                else
                {
                    CLogMessage(this).info("Update info: Updated data cache in %1") << m_updateInfo.getFilename();
                    this->manageUpdateAvailability(true);
                    return; // success
                } // cache
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
            QTimer::singleShot(500, this, &CSetupReader::ps_readUpdateInfo);
        }
        else
        {
            this->manageUpdateAvailability(false);
        }
    } // function

    const CLogCategoryList &CSetupReader::getLogCategories()
    {
        static const CLogCategoryList cats({ CLogCategory("swift.setupreader"), CLogCategory::webservice(), CLogCategory::startup()});
        return cats;
    }

    CGlobalSetup CSetupReader::getSetup() const
    {
        return m_setup.get();
    }

    CUpdateInfo CSetupReader::getUpdateInfo() const
    {
        return m_updateInfo.get();
    }

    CStatusMessageList CSetupReader::manageSetupAvailability(bool webRead, bool localRead)
    {
        Q_ASSERT_X(!(webRead && localRead), Q_FUNC_INFO, "Local and web read together seems to be wrong");
        CStatusMessageList msgs;
        if (webRead)
        {
            msgs.push_back(CLogMessage(this).info("Setup loaded from web, will trigger read of update information"));
            QTimer::singleShot(500, this, &CSetupReader::ps_readUpdateInfo);
        }
        if (localRead)
        {
            msgs.push_back(CLogMessage(this).info("Setup loaded locally, will trigger read of update information"));
            QTimer::singleShot(500, this, &CSetupReader::ps_readUpdateInfo);
        }

        bool available = false;
        if (webRead || localRead)
        {
            available = true;
        }
        else
        {
            bool cacheAvailable = this->m_setup.getThreadLocal().wasLoaded();
            available = cacheAvailable && this->m_bootstrapMode != Explicit;
        }

        if (available && !webRead && !localRead)
        {
            msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityInfo, "Setup available, but not updated this time"));
        }
        else if (!available)
        {
            msgs.push_back(CStatusMessage(this, CStatusMessage::SeverityError, "Setup not available"));
        }

        this->m_setupAvailable = available;
        emit setupAvailable(available);

        if (!webRead && !localRead)
        {
            msgs.push_back(CStatusMessage(this).warning("Since setup was not updated this time, will not start loading of update information"));
            this->manageUpdateAvailability(false);
        }
        return msgs;
    }

    void CSetupReader::manageUpdateAvailability(bool webRead)
    {
        if (webRead)
        {
            this->m_updateInfoAvailable = true;
            emit updateInfoAvailable(true);
        }
        else
        {
            bool cached = this->m_updateInfo.isSaved();
            this->m_updateInfoAvailable = cached;
            emit updateInfoAvailable(cached);
        }
    }
} // namespace
