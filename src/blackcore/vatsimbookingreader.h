/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VATSIMBOOKINGREADER_H
#define BLACKCORE_VATSIMBOOKINGREADER_H

//! \file

#include "blackmisc/threadedreader.h"
#include "blackmisc/avatcstationlist.h"

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore
{
    /*!
     * Read bookings from VATSIM
     */
    class CVatsimBookingReader : public QObject, public BlackMisc::CThreadedReader<void>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimBookingReader(const QString &url, QObject *parent = nullptr);

        //! Read / re-read bookings
        void read();

    private slots:
        //! Bookings have been read
        void loadFinished(QNetworkReply *nwReply);

    private:
        QString m_serviceUrl; /*!< URL of the service */
        QNetworkAccessManager *m_networkManager;

        //! Parse received bookings
        //! \threadsafe
        void parseBookings(QNetworkReply *nwReply);

    signals:
        //! Bookings have been read and converted to BlackMisc::Aviation::CAtcStationList
        void dataRead(const BlackMisc::Aviation::CAtcStationList &bookedStations);
    };
}
#endif // guard
