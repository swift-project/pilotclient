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
#include "blackcore/data/updateinfo.h"

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

        //! Version bas been read
        void versionSynchronized(bool success);

    protected slots:
        //! \copydoc CThreadedReader::initialize
        virtual void initialize() override;

        //! \copydoc CThreadedReader::cleanup
        virtual void cleanup() override;

    private slots:
        //! Setup has been read
        //! \threadsafe
        void ps_parseSetupFile(QNetworkReply *nwReply);

        //! Update info has been read
        //! \threadsafe
        void ps_parseUpdateInfoFile(QNetworkReply *nwReplyPtr);

        //! Do reading
        void ps_readSetup();

        //! Do reading
        void ps_readUpdateInfo();

        //! Setup has beem syncronized
        void ps_setupSyncronized(bool success);

        //! Setup has been changed
        void ps_setupChanged();

    private:
        QNetworkAccessManager              *m_networkManagerBootstrap = nullptr;
        QNetworkAccessManager              *m_networkManagerUpdateInfo = nullptr;
        BlackMisc::LockFree<BlackMisc::Network::CFailoverUrlList> m_bootstrapUrls;
        BlackMisc::LockFree<BlackMisc::Network::CFailoverUrlList> m_updateInfoUrls;
        BlackMisc::CData<BlackCore::Data::GlobalSetup> m_setup {this, &CSetupReader::ps_setupChanged};  //!< data cache setup
        BlackMisc::CData<BlackCore::Data::UpdateInfo>  m_updateInfo {this};                             //!< data cache update info

        //! Constructor
        explicit CSetupReader(QObject *owner);

        //! Read by local individual file
        bool localBootstrapFile(QString &fileName);

        //! Read for development environment?
        static bool isForDevelopment();
    };
} // ns

#endif // guard
