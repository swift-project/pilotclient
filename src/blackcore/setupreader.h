/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SETUPREADER_H
#define BLACKCORE_SETUPREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/db/updateinfo.h"
#include "blackmisc/datacache.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/statusmessagelist.h"

#include <QCommandLineOption>
#include <QCoreApplication>
#include <QJsonObject>
#include <QNetworkReply>
#include <QList>
#include <QObject>
#include <QString>
#include <atomic>

namespace BlackMisc { class CLogCategoryList; }
namespace BlackCoreTest { class CTestConnectivity; }
namespace BlackCore
{
    //! Read the central URLs / locations of our data, setup and versions.
    //!
    //! \details This class should be only used in BlackCore::CApplication.
    //!
    //! \note This class is no(!) BlackCore::CThreadedReader as it will be loaded once during startup
    //!       and reading setup data is fast. The read file is also called "bootstrap" file as it tells
    //!       swift which data and versions are located where. Without that file we cannot start.
    //!       Once the file is in place (i.e. in the cache) it can be automatically updated.
    //!
    //! \sa BlackCore::Data::TGlobalSetup
    //! \sa BlackMisc::Db::TUpdateInfo
    class BLACKCORE_EXPORT CSetupReader : public QObject
    {
        Q_OBJECT
        friend class CApplication; //!< only using class
        friend class BlackCoreTest::CTestConnectivity;

    public:
        //! Categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Has a given cmd line argument for bootstrap URL?
        bool hasCmdLineBootstrapUrl() const;

        //! CMD line argument for bootstrap URL
        QString getCmdLineBootstrapUrl() const;

        //! Ignore the bootstrap URL
        //! \threadsafe
        void setIgnoreCmdLineBootstrapUrl(bool ignore);

        //! Check connection of the bootstrap URL
        //! \threadsafe
        void setCheckCmdLineBootstrapUrl(bool check) { m_checkCmdBootstrapUrl = check; }

        //! Current setup (reader URLs, DB location, crash server)
        //! \remarks aka "bootstrap file"
        //! \threadsafe
        Data::CGlobalSetup getSetup() const;

        //! Has cached setup ("bootstrap") data?
        //! \threadsafe
        bool hasCachedSetup() const;

        //! Get setup cache timestamp
        //! \threadsafe
        QDateTime getSetupCacheTimestamp() const;

        //! Load the cache file local bootstrap file
        //! \remark can be used during installation as failover
        //! \threadsafe
        bool prefillCacheWithLocalResourceBootstrapFile();

        //! Last distribution URL successfully read
        //! \threadsafe
        QString getLastSuccessfulSetupUrl() const;

        //! Synchronize the caches
        void synchronize();

        //! Last setup parsing error messages (if any)
        BlackMisc::CStatusMessageList getLastSetupReadErrorMessages() const;

        //! Get bootstrap URL, either m_bootstrapUrlFileValue or m_localSetupFileValue
        const QString &getBootstrapUrlFile() const;

        //! Mode as string
        QString getBootstrapModeAsString() const;

    signals:
        //! Setup fetched or failed (from web, cache, or local file)?
        void setupHandlingCompleted(bool available);

        //! A shared URL was successfully read
        void successfullyReadSharedUrl(const BlackMisc::Network::CUrl &sharedUrl);

        //! Message about the loading status
        void setupLoadingMessages(const BlackMisc::CStatusMessageList &messages);

    protected:
        //! Constructor
        explicit CSetupReader(QObject *parent);

        //! Load the data
        BlackMisc::CStatusMessageList asyncLoad();

        //! Parse cmd line arguments
        bool parseCmdLineArguments();

        //! Add cmd line arguments to BlackCore::CApplication
        QList<QCommandLineOption> getCmdLineOptions() const;

        //! Terminate
        void gracefulShutdown();

        //! Setup available?
        //! \threadsafe
        bool isSetupAvailable() const { return m_setupAvailable; }

        //! Force an availability update
        //! \remark check for cached setup if the read check never got triggered
        void forceAvailabilityUpdate();

    private:
        //! Bootstrap mode
        enum BootstrapMode
        {
            Implicit,
            Explicit,
            CacheOnly
        };

        std::atomic<bool> m_shutdown { false };                  //!< shutdown in progress
        std::atomic<bool> m_setupAvailable { false };            //!< setup available?
        std::atomic<bool> m_ignoreCmdBootstrapUrl { false };     //!< ignore the explicitly set bootstrap URL
        std::atomic<bool> m_checkCmdBootstrapUrl { true };       //!< check connection on CMD bootstrap URL
        std::atomic_int   m_bootstrapReadErrors  { 0 };          //!< failed bootstrap reads
        std::atomic_int   m_updateInfoReadErrors { 0 };          //!< failed version info reads
        QString m_localSetupFileValue;                           //!< Local file for setup, passed by cmd line arguments
        QString m_bootstrapUrlFileValue;                         //!< Bootstrap URL if not local
        BootstrapMode m_bootstrapMode = Explicit;                //!< How to bootstrap
        BlackMisc::Network::CUrlList m_bootstrapUrls;            //!< location of setup files
        QCommandLineOption m_cmdBootstrapUrl;                    //!< bootstrap URL
        QCommandLineOption m_cmdBootstrapMode;                   //!< bootstrap mode
        mutable QReadWriteLock m_lockSetup;                      //!< lock for setup
        mutable QReadWriteLock m_lockUpdateInfo;                 //!< lock for update info
        QString m_lastSuccessfulSetupUrl;                        //!< last successful read setup URL
        BlackMisc::CStatusMessageList m_setupReadErrorMsgs;      //!< last parsing error messages
        BlackMisc::CData<Data::TGlobalSetup> m_setup { this };   //!< data cache setup

        //! Setup has been read (aka bootstrap file)
        void parseBootstrapFile(QNetworkReply *nwReplyPtr);

        //! Update info has been read
        void parseUpdateInfoFile(QNetworkReply *nwReplyPtr);

        //! Do reading
        void readSetup();

        //! Read by local individual file and update cache from that
        BlackMisc::CStatusMessageList readLocalBootstrapFile(const QString &fileName);

        //! Trigger reading
        BlackMisc::CStatusMessageList triggerReadSetup();

        //! Emit the availability signal and state and trigger follow up actions
        //! \threadsafe
        BlackMisc::CStatusMessageList manageSetupAvailability(bool webRead, bool localRead = false);

        //! Set last setup parsing messages
        void setLastSetupReadErrorMessages(const BlackMisc::CStatusMessageList &messages);

        //! Progress
        void networkReplyProgress(int logId, qint64 current, qint64 max, const QUrl &url);

        //! Convert string to bootstrap mode
        static BootstrapMode stringToEnum(const QString &s);

        //! Bootsrap URL used for unit tests
        static const QString &unitTestBootstrapUrl();
    };
} // ns

#endif // guard
