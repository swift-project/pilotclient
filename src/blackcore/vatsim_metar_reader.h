/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIMMETARREADER_H
#define BLACKCORE_VATSIMMETARREADER_H

#include "blackcoreexport.h"
#include "blackmisc/threadedreader.h"
#include "blackmisc/weather/metardecoder.h"
#include "blackmisc/weather/metarset.h"

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore
{
    //! Read bookings from VATSIM
    class BLACKCORE_EXPORT CVatsimMetarReader : public BlackMisc::CThreadedReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimMetarReader(QObject *owner, const QString &url);

        //! Read / re-read bookings
        void readInBackgroundThread();

    signals:
        //! Bookings have been read and converted to BlackMisc::Aviation::CAtcStationList
        void metarUpdated(const BlackMisc::Weather::CMetarSet &metars);

    private slots:
        //! Decode metar
        //! \threadsafe
        void ps_decodeMetar(QNetworkReply *nwReply);

        //! Do reading
        void ps_readMetar();

    private:
        QString m_metarUrl; //!< URL of the service
        QNetworkAccessManager *m_networkManager = nullptr;
        BlackMisc::Weather::CMetarDecoder m_metarDecoder;
    };
}
#endif // guard
