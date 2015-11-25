/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIMBOOKINGREADER_H
#define BLACKCORE_VATSIMBOOKINGREADER_H

#include "blackcoreexport.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/threadedreader.h"
#include "blackmisc/aviation/atcstationlist.h"

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore
{
    //! Read bookings from VATSIM
    class BLACKCORE_EXPORT CVatsimBookingReader : public BlackMisc::CThreadedReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimBookingReader(QObject *owner);

        //! Read / re-read bookings
        void readInBackgroundThread();

    signals:
        //! Bookings have been read and converted to BlackMisc::Aviation::CAtcStationList
        void atcBookingsRead(const BlackMisc::Aviation::CAtcStationList &bookedStations);

        //! Data have been read
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

    protected:
        //! \copydoc CThreadedReader::cleanup
        virtual void cleanup() override;

    private slots:
        //! Bookings have been read
        //! \threadsafe
        void ps_parseBookings(QNetworkReply *nwReply);

        //! Do reading
        void ps_read();

    private:
        BlackCore::CData<BlackCore::Data::GlobalSetup> m_setup {this}; //!< setup cache
        QNetworkAccessManager *m_networkManager = nullptr;
    };
}

#endif // guard
