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
#include "blackcore/data/globalsetup.h"

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
        //! Constructor
        explicit CSetupReader(QObject *owner);

    protected slots:
        //! \copydoc CThreadedReader::initialize
        virtual void initialize() override;

    private slots:
        //! Setup has been read
        //! \threadsafe
        void ps_parseSetupFile(QNetworkReply *nwReply);

        //! Do reading
        void ps_read();

    private:
        QNetworkAccessManager              *m_networkManager = nullptr;
        BlackMisc::Network::CUrlList        m_failedUrls;
        CData<BlackCore::Data::GlobalSetup> m_setup {this}; //!< data cache

        //! Read by local individual file
        bool localFile(QString &fileName);

        //! Remaining URLs failed one excluded
        BlackMisc::Network::CUrlList getRemainingUrls() const;

        //! Bootstrap URL
        static QString appendPathAndFile();
    };
} // ns

#endif // guard
