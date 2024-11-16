// SPDX-FileCopyrightText: Copyright (C) 2023 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_VATSIM_VATSIMSERVERFILEREADER_H
#define SWIFT_CORE_VATSIM_VATSIMSERVERFILEREADER_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

#include "core/data/vatsimsetup.h"
#include "core/swiftcoreexport.h"
#include "core/threadedreader.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/flightplan.h"
#include "misc/datacache.h"
#include "misc/network/entityflags.h"
#include "misc/network/serverlist.h"
#include "misc/network/userlist.h"
#include "misc/network/voicecapabilities.h"
#include "misc/simulation/simulatedaircraftlist.h"

class QNetworkReply;

namespace swift::core::vatsim
{
    //! Read VATSIM server file
    //! \sa https://data.vatsim.net/v3/vatsim-servers.json
    class SWIFT_CORE_EXPORT CVatsimServerFileReader :
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
        //! \name swift::core::CThreadedReader overrides
        //! @{
        virtual void doWorkImpl() override;
        //! @}

    private:
        swift::misc::CData<swift::core::data::TVatsimSetup> m_lastGoodSetup { this };

        //! Data have been read, parse VATSIM server file
        void parseVatsimFile(QNetworkReply *nwReply);

        swift::misc::network::CServer parseServer(const QJsonObject &) const;

        //! Read / re-read data file
        void read();
    };
} // namespace swift::core::vatsim

#endif // guard
