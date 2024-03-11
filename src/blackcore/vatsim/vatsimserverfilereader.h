// SPDX-FileCopyrightText: Copyright (C) 2023 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_VATSIM_VATSIMSERVERFILEREADER_H
#define BLACKCORE_VATSIM_VATSIMSERVERFILEREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/vatsimsetup.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/datacache.h"
#include "blackcore/threadedreader.h"

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

class QNetworkReply;

namespace BlackCore::Vatsim
{
    //! Read VATSIM server file
    //! \sa https://data.vatsim.net/v3/vatsim-servers.json
    class BLACKCORE_EXPORT CVatsimServerFileReader :
        public CThreadedReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimServerFileReader(QObject *owner);

        //! Get all VATSIM FSD servers
        //! \threadsafe
        BlackMisc::Network::CServerList getFsdServers() const;

        //! Start reading in own thread
        void readInBackgroundThread();

    signals:
        //! Data have been read
        void dataFileRead(int kB);

        //! Data have been read
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number, const QUrl &url);

    protected:
        //! \name BlackCore::CThreadedReader overrides
        //! @{
        virtual void doWorkImpl() override;
        //! @}

    private:
        BlackMisc::CData<BlackCore::Data::TVatsimSetup> m_lastGoodSetup { this };

        //! Data have been read, parse VATSIM server file
        void parseVatsimFile(QNetworkReply *nwReply);

        BlackMisc::Network::CServer parseServer(const QJsonObject &) const;

        //! Read / re-read data file
        void read();
    };
} // ns

#endif // guard
