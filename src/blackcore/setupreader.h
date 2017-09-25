/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SETUPREADER_H
#define BLACKCORE_SETUPREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/db/distributionlist.h"
#include "blackmisc/datacache.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/statusmessagelist.h"

#include <QCommandLineOption>
#include <QCoreApplication>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>
#include <atomic>

class QNetworkReply;

namespace BlackMisc { class CLogCategoryList; }
namespace BlackCoreTest { class CTestConnectivity; }
namespace BlackCore
{
    //! Read the central URLs / locations of our data / setup.
    //!
    //! \details This class should be only used in BlackCore::CApplication. It will also trigger reading
    //!          update information.
    //!
    //! \note This class is no(!) BlackCore::CThreadedReader as it will be loaded once during startup
    //!       and reading setup data is fast. The read file is also called "bootstrap" file as it tells
    //!       swift which data and versions are located where. Without that file we cannot start.
    //!       Once the file is in place (i.e. in the cache) it can be automatically updated.
    //!
    //! \sa BlackCore::Data::TGlobalSetup
    //! \sa BlackMisc::Db::TDistributionInfo
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
        BlackCore::Data::CGlobalSetup getSetup() const;

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

        //! Distributions info (channel, version, platforms, download URLs)
        //! \threadsafe
        BlackMisc::Db::CDistributionList getDistributionInfo() const;

        //! Has cached distribution info?
        //! \threadsafe
        bool hasCachedDistributionInfo() const;

        //! Distribution cache timestamp
        //! \threadsafe
        QDateTime getDistributionCacheTimestamp() const;

        //! Last distribution URL successfully read
        //! \threadsafe
        QString getLastSuccessfulDistributionUrl() const;

        //! Last setup parsing error messages (if any)
        BlackMisc::CStatusMessageList getLastSetupReadErrorMessages() const;

        //! Get bootstrap URL, either m_bootstrapUrlFileValue or m_localSetupFileValue
        const QString &getBootstrapUrlFile() const;

        //! Mode as string
        QString getBootstrapModeAsString() const;

    signals:
        //! Setup fetched or failed (from web, cache, or local file)
        void setupHandlingCompleted(bool available);

        //! Setup avialable (from web, cache
        void distributionInfoAvailable(bool available);

        //! A shared URL was successfully read
        void successfullyReadSharedUrl(const BlackMisc::Network::CUrl &sharedUrl);

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

        //! Version info available?
        //! \threadsafe
        bool isUpdateInfoAvailable() const { return m_distributionInfoAvailable; }

    private:
        //! Bootstrap mode
        enum BootstrapMode
        {
            Implicit,
            Explicit,
            CacheOnly
        };

        std::atomic<bool> m_shutdown { false };
        std::atomic<bool> m_setupAvailable { false };
        std::atomic<bool> m_distributionInfoAvailable { false };
        std::atomic<bool> m_ignoreCmdBootstrapUrl { false };     //!< ignore the explicitly set bootstrap URL
        std::atomic<bool> m_checkCmdBootstrapUrl { true };       //!< check connection on CMD bootstrap URL
        QString m_localSetupFileValue;                           //!< Local file for setup, passed by cmd line arguments
        QString m_bootstrapUrlFileValue;                         //!< Bootstrap URL if not local
        BootstrapMode m_bootstrapMode = Explicit;                //!< How to bootstrap
        BlackMisc::Network::CFailoverUrlList m_bootstrapUrls;    //!< location of setup files
        BlackMisc::Network::CFailoverUrlList m_distributionUrls; //!< location of info files
        QCommandLineOption m_cmdBootstrapUrl;                    //!< bootstrap URL
        QCommandLineOption m_cmdBootstrapMode;                   //!< bootstrap mode
        mutable QReadWriteLock m_lockSetup;                      //!< lock for setup
        mutable QReadWriteLock m_lockDistribution;               //!< lock for distribution
        QString m_lastSuccessfulSetupUrl;                        //!< last successful read setup URL
        QString m_lastSuccessfulDistributionUrl;                 //!< last successful read distribution URL
        BlackMisc::CStatusMessageList m_setupReadErrorMsgs;      //!< last parsing error messages
        BlackMisc::CData<BlackCore::Data::TGlobalSetup>     m_setup { this, &CSetupReader::setupChanged }; //!< data cache setup
        BlackMisc::CData<BlackMisc::Db::TDistributionsInfo> m_distributions { this };                         //!< data cache distributions

        //! Setup has been read (aka bootstrap file)
        void parseSetupFile(QNetworkReply *nwReply);

        //! Update info has been read
        void parseDistributionsFile(QNetworkReply *nwReplyPtr);

        //! Do reading
        void readSetup();

        //! Do reading of distributions
        void readDistributionInfo();

        //! Setup has been changed
        void setupChanged();

        //! Read by local individual file and update cache from that
        BlackMisc::CStatusMessageList readLocalBootstrapFile(const QString &fileName);

        //! Trigger reading
        BlackMisc::CStatusMessageList triggerReadSetup();

        //! Emit the availability signal and state and trigger follow up actions
        //! \threadsafe
        BlackMisc::CStatusMessageList manageSetupAvailability(bool webRead, bool localRead = false);

        //! Emit the available signal
        //! \threadsafe
        void manageDistributionsInfoAvailability(bool webRead);

        //! Set last setup parsing messages
        void setLastSetupReadErrorMessages(const BlackMisc::CStatusMessageList &messages);

        //! Convert string to mode
        static BootstrapMode stringToEnum(const QString &s);

        //! Bootsrap URL used for unit tests
        static const QString &unitTestBootstrapUrl();
    };
} // ns

#endif // guard
