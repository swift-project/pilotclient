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
#include "blackcore/data/updateinfo.h"
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

namespace BlackCore
{
    //! Read the central URLs / locations of our data / setup.
    //!
    //! \details This class should be only used in BlackCore::CApplication. It will also trigger reading
    //!          update information.
    //!
    //! \note This class is no(!) BlackCore::CThreadedReader as it will be loaded once during startup
    //!       and reading setup data is fast.
    //!
    //! \sa BlackCore::Data::GlobalSetup
    //! \sa BlackCore::Data::UpdateInfo
    class BLACKCORE_EXPORT CSetupReader : public QObject
    {
        Q_OBJECT
        friend class CApplication; //!< only using class

    public:
        //! Categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Current setup
        //! \threadsafe
        BlackCore::Data::CGlobalSetup getSetup() const;

        //! Update info
        //! \threadsafe
        BlackCore::Data::CUpdateInfo getUpdateInfo() const;

    signals:
        //! Setup avialable (from web, cache, or local file)
        void setupAvailable(bool available);

        //! Setup avialable (from web, cache
        void updateInfoAvailable(bool available);

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
        bool isUpdateInfoAvailable() const { return m_updateInfoAvailable; }

    private slots:
        //! Setup has been read
        void ps_parseSetupFile(QNetworkReply *nwReply);

        //! Update info has been read
        void ps_parseUpdateInfoFile(QNetworkReply *nwReplyPtr);

        //! Do reading
        void ps_readSetup();

        //! Do reading
        void ps_readUpdateInfo();

        //! Setup has been changed
        void ps_setupChanged();

    private:
        //! Bootstrap mode
        enum BootstrapMode
        {
            Implicit,
            Explicit,
            CacheOnly
        };

        bool m_shutdown = false;
        std::atomic<bool> m_setupAvailable { false };
        std::atomic<bool> m_updateInfoAvailable { false };
        QString m_localSetupFileValue;                         //! Local file for setup, passed by cmd line arguments
        QString m_bootstrapUrlFileValue;                       //! Bootstrap URL if not local
        BootstrapMode m_bootstrapMode = Explicit;              //! How to bootstrap
        BlackMisc::Network::CFailoverUrlList m_bootstrapUrls;  //!< location of setup files
        BlackMisc::Network::CFailoverUrlList m_updateInfoUrls; //!< location of info files
        BlackMisc::CData<BlackCore::Data::GlobalSetup> m_setup {this, &CSetupReader::ps_setupChanged};  //!< data cache setup
        BlackMisc::CData<BlackCore::Data::UpdateInfo>  m_updateInfo {this};                             //!< data cache update info

        QCommandLineOption m_cmdBootstrapUrl
        {
            { "url", "bootstrapurl" },
            QCoreApplication::translate("application", "bootstrap URL, e.g. datastore.swift-project.org"),
            "bootstrapurl"
        };                                                     //!< bootstrap URL
        QCommandLineOption m_cmdBootstrapMode
        {
            { "bmode", "bootstrapmode" },
            QCoreApplication::translate("application", "bootstrap mode: explicit, implicit, cache(-only)"),
            "bootstrapmode", "explicit"
        };                                                     //!< bootstrap mode

        //! Read by local individual file and update cache from that
        bool readLocalBootstrapFile(QString &fileName);

        //! Trigger reading
        BlackMisc::CStatusMessageList triggerReadSetup();

        //! Emit the availability signal and state
        //! \threadsafe
        BlackMisc::CStatusMessageList manageSetupAvailability(bool webRead, bool localRead = false);

        //! Emit the available signal
        //! \threadsafe
        void manageUpdateAvailability(bool webRead);

        //! Convert string to mode
        static BootstrapMode stringToEnum(const QString &s);
    };
} // ns

#endif // guard
