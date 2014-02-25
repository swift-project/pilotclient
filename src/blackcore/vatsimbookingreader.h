/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_VATSIMBOOKINGREADER_H
#define BLACKCORE_VATSIMBOOKINGREADER_H

#include "blackmisc/avatcstationlist.h"
#include <QObject>
#include <QTimer>
#include <QNetworkReply>

namespace BlackCore
{
    /*!
     * \brief Read bookings from VATSIM
     */
    class CVatsimBookingReader : public QObject
    {
        Q_OBJECT

    public:
        //! \brief Constructor
        explicit CVatsimBookingReader(const QString &url, QObject *parent = nullptr);

        //! \brief Update timestamp
        QDateTime getUpdateTimestamp() const { return this->m_updateTimestamp; }

        //! \brief Read / re-read bookings
        void read();

        /*!
         * \brief Set the update time
         * \param updatePeriodMs 0 stops the timer
         */
        void setInterval(int updatePeriodMs);

        //! \brief Get the timer interval (ms)
        int interval() const { return this->m_updateTimer->interval();}

    private slots:
        //! \brief Bookings have been read
        void loadFinished(QNetworkReply *nwReply);

    private:
        QString m_serviceUrl; /*!< URL of the service */
        QNetworkAccessManager *m_networkManager;
        QDateTime m_updateTimestamp;
        QTimer *m_updateTimer;

    signals:
        //! \brief Bookings have been read and converted to BlackMisc::Aviation::CAtcStationList
        void dataRead(BlackMisc::Aviation::CAtcStationList bookedStations);
    };
}
#endif // guard
