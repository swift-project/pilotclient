/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_ICAODATAREADER_H
#define BLACKCORE_ICAODATAREADER_H

#include "blackcoreexport.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/threadedreader.h"

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore
{
    //! Read bookings from VATSIM
    class BLACKCORE_EXPORT CIcaoDataReader : public BlackMisc::CThreadedReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CIcaoDataReader(QObject *owner, const QString &aircraftIcaoUrl, const QString &airlineIcaoUrl);

        //! Get aircraft ICAO information
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const;

        //! Get airline ICAO information
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const;

        //! Start reading in own thread
        void readInBackgroundThread();

    signals:
        //! Codes have been read
        void readAircraftIcaoCodes(int number);

        //! Codes have been read
        void readAirlinesIcaoCodes(int number);

    private slots:
        //! Aircraft have been read
        void ps_parseAircraftIcaoData(QNetworkReply *nwReply);

        //! Airlines have been read
        void ps_parseAirlineIcaoData(QNetworkReply *nwReply);

        //! Read / re-read data file
        void ps_read();

    private:
        QNetworkAccessManager *m_networkManagerAircraft = nullptr;
        QNetworkAccessManager *m_networkManagerAirlines = nullptr;
        BlackMisc::Aviation::CAircraftIcaoCodeList m_aircraftIcaos;
        BlackMisc::Aviation::CAirlineIcaoCodeList m_airlineIcaos;
        QString m_urlAircraftIcao;
        QString m_urlAirlineIcao;

        mutable QReadWriteLock m_lockAirline;
        mutable QReadWriteLock m_lockAircraft;

        //! Check if terminated or error, otherwise split into array of objects
        QJsonArray splitReplyIntoArray(QNetworkReply *nwReply) const;

        //! Check if thread has been finished
        bool checkIfFinished() const;
    };
}

#endif // guard
