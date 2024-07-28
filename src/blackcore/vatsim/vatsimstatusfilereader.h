// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_VATSIM_VATSIMSTATUSFILEREADER_H
#define BLACKCORE_VATSIM_VATSIMSTATUSFILEREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/vatsimsetup.h"
#include "blackmisc/datacache.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/url.h"
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

        //! METAR URL
        //! \threadsafe
        BlackMisc::Network::CUrl getMetarFileUrl() const;

        //! Data file URL
        //! \threadsafe
        BlackMisc::Network::CUrl getDataFileUrl() const;

        //! Start reading in own thread
        void readInBackgroundThread();

    signals:
        //! Data have been read
        void statusFileRead(int bytes);

        //! Data have been read
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int bytes);

    private:
        //! Read / re-read data file
        void read();

        //! Data have been read, parse VATSIM file
        void parseVatsimFile(QNetworkReply *nwReply);

        BlackMisc::CData<BlackCore::Data::TVatsimSetup> m_lastGoodSetup { this };
    };
} // ns

#endif // guard
