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

#include "blackcore/blackcoreexport.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/threadedreader.h"
#include "blackmisc/weather/metar.h"
#include "blackmisc/weather/metardecoder.h"
#include "blackmisc/weather/metarset.h"

#include <QObject>

class QNetworkReply;

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
        //! \copydoc BlackMisc::CThreadedReader::cleanup
        virtual void cleanup() override;

    private slots:
        //! Decode METARs
        //! \threadsafe
        void ps_decodeMetars(QNetworkReply *nwReply);

        //! Do reading
        void ps_readMetars();

    private:
        BlackMisc::Weather::CMetarDecoder m_metarDecoder;
        BlackMisc::Weather::CMetarSet     m_metars;
    };
}
#endif // guard
