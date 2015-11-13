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
#include "blackmisc/threadedreader.h"
#include "blackmisc/lockfree.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/data/download.h"

#include <QObject>
#include <QTimer>
#include <QNetworkReply>

namespace BlackCore
{
    //! Read the central URLs / locations of our data / setup
    class BLACKCORE_EXPORT CSetupReader : public BlackMisc::CThreadedReader
    {
        Q_OBJECT

    public:
        //! Single instance
        static CSetupReader &instance();

    signals:
        //! Setup has been read
        void setupSynchronized(bool success);

    protected slots:
        //! \copydoc CThreadedReader::initialize
        virtual void initialize() override;

    private slots:
        //! Setup has been read
        //! \threadsafe
        void ps_parseSetupFile(QNetworkReply *nwReply);

        //! Download has been read
        //! \threadsafe
        void ps_parseDownloadFile(QNetworkReply *nwReplyPtr);

        //! Do reading
        void ps_readSetup();

        //! Do reading
        void ps_readDownload();

        //! Setup has beem syncronized
        void ps_setupSyncronized(bool success);

    private:
        QNetworkAccessManager              *m_networkManagerBootstrap = nullptr;
        QNetworkAccessManager              *m_networkManagerDownload = nullptr;
        BlackMisc::LockFree<BlackMisc::Network::CFailoverUrlList> m_bootstrapUrls;
        BlackMisc::LockFree<BlackMisc::Network::CFailoverUrlList> m_downloadUrls;
        CData<BlackCore::Data::GlobalSetup> m_setup {this};    //!< data cache setup
        CData<BlackCore::Data::Download>    m_download {this}; //!< data cache downloads

        //! Constructor
        explicit CSetupReader(QObject *owner);

        //! Read by local individual file
        bool localBootstrapFile(QString &fileName);

        //! Read for development environment?
        static bool isForDevelopment();
    };
} // ns

#endif // guard
