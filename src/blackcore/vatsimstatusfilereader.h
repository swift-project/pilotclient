/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIMSTATUSFILEREADER_H
#define BLACKCORE_VATSIMSTATUSFILEREADER_H

#include "blackcoreexport.h"
#include "blackcore/data/vatsimsetup.h"
#include "blackmisc/threadedreader.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/network/entityflags.h"

#include <QObject>
#include <QTimer>
#include <QReadWriteLock>

namespace BlackCore
{
    //! Sole purpose is to read the URLs where VATSIM data can be downloaded
    //! \sa https://status.vatsim.net/
    class BLACKCORE_EXPORT CVatsimStatusFileReader : public BlackMisc::CThreadedReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimStatusFileReader(QObject *owner);

        //! METAR URLs
        //! \threadsafe
        BlackMisc::Network::CUrlList getMetarFileUrls() const;

        //! Data file URLs
        //! \threadsafe
        BlackMisc::Network::CUrlList getDataFileUrls() const;

    public slots:
        //! Start reading in own thread
        void readInBackgroundThread();

    signals:
        //! Data have been read
        void dataFileRead(int lines);

        //! Data have been read
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

    protected:
        //! \copydoc BlackMisc::CThreadedReader::cleanup
        virtual void cleanup() override;

    private slots:
        //! Data have been read, parse VATSIM file
        void ps_parseVatsimFile(QNetworkReply *nwReply);

        //! Read / re-read data file
        void ps_read();

    private:
        BlackMisc::CData<BlackCore::Data::VatsimSetup> m_lastGoodSetup { this };
    };
} // ns

#endif // guard
