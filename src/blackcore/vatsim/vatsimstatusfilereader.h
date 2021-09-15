/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIM_VATSIMSTATUSFILEREADER_H
#define BLACKCORE_VATSIM_VATSIMSTATUSFILEREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/vatsimsetup.h"
#include "blackmisc/datacache.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/urllist.h"
#include "blackcore/threadedreader.h"

#include <QObject>

class QNetworkReply;

namespace BlackCore::Vatsim
{
    //! Sole purpose is to read the URLs where VATSIM data can be downloaded
    //! \sa https://status.vatsim.net/
    class BLACKCORE_EXPORT CVatsimStatusFileReader : public BlackCore::CThreadedReader
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

        //! Start reading in own thread
        void readInBackgroundThread();

    signals:
        //! Data have been read
        void dataFileRead(int lines);

        //! Data have been read
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

    private:
        //! Read / re-read data file
        void read();

        //! Data have been read, parse VATSIM file
        void parseVatsimFile(QNetworkReply *nwReply);

        BlackMisc::CData<BlackCore::Data::TVatsimSetup> m_lastGoodSetup { this };
    };
} // ns

#endif // guard
