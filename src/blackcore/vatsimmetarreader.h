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
#include "blackcore/data/globalsetup.h"
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
        explicit CVatsimMetarReader(QObject *owner);

        //! Read / re-read bookings
        void readInBackgroundThread();

        //! Get METARs
        //! \threadsafe
        virtual BlackMisc::Weather::CMetarSet getMetars() const;

        //! Get METAR for airport
        //! \threadsafe
        virtual BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &icao) const;

        //! Get METARs count
        //! \threadsafe
        virtual int getMetarsCount() const;

    signals:
        //! METARs have been read and converted to BlackMisc::Weather::CMetarSet
        void metarsRead(const BlackMisc::Weather::CMetarSet &metars);

        //! Data have been read
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

    protected:
        //! \copydoc CThreadedReader::cleanup
        virtual void cleanup() override;

    private slots:
        //! Decode METARs
        //! \threadsafe
        void ps_decodeMetars(QNetworkReply *nwReply);

        //! Do reading
        void ps_readMetars();

    private:
        QNetworkAccessManager            *m_networkManager = nullptr;
        BlackMisc::Weather::CMetarDecoder m_metarDecoder;
        BlackMisc::Weather::CMetarSet     m_metars;
        BlackCore::CData<BlackCore::Data::GlobalSetup> m_setup {this}; //!< setup cache
    };
}
#endif // guard
