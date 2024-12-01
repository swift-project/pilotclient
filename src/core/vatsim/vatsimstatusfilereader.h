// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_VATSIM_VATSIMSTATUSFILEREADER_H
#define SWIFT_CORE_VATSIM_VATSIMSTATUSFILEREADER_H

#include <QObject>

#include "core/data/vatsimsetup.h"
#include "core/swiftcoreexport.h"
#include "core/threadedreader.h"
#include "misc/datacache.h"
#include "misc/network/entityflags.h"
#include "misc/network/url.h"

class QNetworkReply;

namespace swift::core::vatsim
{
    //! Sole purpose is to read the URLs where VATSIM data can be downloaded
    //! \sa https://status.vatsim.net/
    class SWIFT_CORE_EXPORT CVatsimStatusFileReader : public swift::core::CThreadedReader
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
        void dataRead(swift::misc::network::CEntityFlags::Entity entity,
                      swift::misc::network::CEntityFlags::ReadState state, int bytes);

    private:
        //! Read / re-read data file
        void read();

        //! Data have been read, parse VATSIM file
        void parseVatsimFile(QNetworkReply *nwReply);

        swift::misc::CData<swift::core::data::TVatsimSetup> m_lastGoodSetup { this };
    };
} // namespace swift::core::vatsim

#endif // SWIFT_CORE_VATSIM_VATSIMSTATUSFILEREADER_H
