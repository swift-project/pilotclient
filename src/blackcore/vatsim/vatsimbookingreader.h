/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIM_VATSIMBOOKINGREADER_H
#define BLACKCORE_VATSIM_VATSIMBOOKINGREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/network/ecosystemprovider.h"
#include "blackmisc/network/entityflags.h"
#include "blackcore/threadedreader.h"

#include <QObject>

class QNetworkReply;

namespace BlackCore::Vatsim
{
    //! Read bookings from VATSIM
    class BLACKCORE_EXPORT CVatsimBookingReader :
        public BlackCore::CThreadedReader,
        public BlackMisc::Network::CEcosystemAware
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

        //! Bookings have been re-read but did not change
        void atcBookingsReadUnchanged();

        //! Data have been read
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number, const QUrl &url);

    protected:
        //! \name BlackCore::CThreadedReader overrides
        //! @{
        virtual void doWorkImpl() override;
        //! @}

    private:
        //! Bookings have been read
        //! \threadsafe
        void parseBookings(QNetworkReply *nwReply);

        //! Do reading
        void read();

        //! Settings changed
        void settingsChanged();

        int m_failures = 0;
        BlackMisc::CSettingReadOnly<BlackCore::Vatsim::TVatsimBookings> m_settings { this, &CVatsimBookingReader::settingsChanged };
    };
} // ns

#endif // guard
