// SPDX-FileCopyrightText: Copyright (C) 2023 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_VATSIM_VATSIMSERVERFILEREADER_H
#define BLACKCORE_VATSIM_VATSIMSERVERFILEREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/vatsimsetup.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/flightplan.h"
#include "misc/network/entityflags.h"
#include "misc/network/serverlist.h"
#include "misc/network/userlist.h"
#include "misc/network/voicecapabilities.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/datacache.h"
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
        swift::misc::network::CServerList getFsdServers() const;

        //! Start reading in own thread
        void readInBackgroundThread();

    signals:
        //! Data have been read
        void dataFileRead(int bytes);

        //! Data have been read
        void dataRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

    protected:
        //! \name BlackCore::CThreadedReader overrides
        //! @{
        virtual void doWorkImpl() override;
        //! @}

    private:
        swift::misc::CData<BlackCore::Data::TVatsimSetup> m_lastGoodSetup { this };

        //! Data have been read, parse VATSIM server file
        void parseVatsimFile(QNetworkReply *nwReply);

        swift::misc::network::CServer parseServer(const QJsonObject &) const;

        //! Read / re-read data file
        void read();
    };
} // ns

#endif // guard
