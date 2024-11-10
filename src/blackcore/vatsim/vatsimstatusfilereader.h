// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_VATSIM_VATSIMSTATUSFILEREADER_H
#define BLACKCORE_VATSIM_VATSIMSTATUSFILEREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/vatsimsetup.h"
#include "misc/datacache.h"
#include "misc/network/entityflags.h"
#include "misc/network/url.h"
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
        swift::misc::network::CUrl getMetarFileUrl() const;

        //! Data file URL
        //! \threadsafe
        swift::misc::network::CUrl getDataFileUrl() const;

        //! Start reading in own thread
        void readInBackgroundThread();

    signals:
        //! Data have been read
        void statusFileRead(int bytes);

        //! Data have been read
        void dataRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState state, int bytes);

    private:
        //! Read / re-read data file
        void read();

        //! Data have been read, parse VATSIM file
        void parseVatsimFile(QNetworkReply *nwReply);

        swift::misc::CData<BlackCore::Data::TVatsimSetup> m_lastGoodSetup { this };
    };
} // ns

#endif // guard
