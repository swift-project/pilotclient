// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_VATSIM_VATSIMMETARREADER_H
#define BLACKCORE_VATSIM_VATSIMMETARREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/weather/metar.h"
#include "blackmisc/weather/metardecoder.h"
#include "blackmisc/weather/metarlist.h"
#include "blackmisc/network/ecosystemprovider.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackcore/threadedreader.h"

#include <QObject>

class QNetworkReply;

namespace BlackCore::Vatsim
{
    //! Read METARs from VATSIM
    class BLACKCORE_EXPORT CVatsimMetarReader :
        public BlackCore::CThreadedReader,
        public BlackMisc::Network::CEcosystemAware
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimMetarReader(QObject *owner);

        //! Read / re-read metars
        void readInBackgroundThread();

        //! Get METARs
        //! \threadsafe
        virtual BlackMisc::Weather::CMetarList getMetars() const;

        //! Get METAR for airport
        //! \threadsafe
        virtual BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &icao) const;

    signals:
        //! METARs have been read and converted to BlackMisc::Weather::CMetarList
        void metarsRead(const BlackMisc::Weather::CMetarList &metars);

        //! Data have been read
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number, const QUrl &url);

    protected:
        // CThreadedReader overrides
        virtual void doWorkImpl() override;

    private:
        //! Decode METARs
        //! \threadsafe
        void decodeMetars(QNetworkReply *nwReply);

        //! Do reading
        void read();

        //! Reload settings
        void reloadSettings();

    private:
        BlackMisc::Weather::CMetarDecoder m_metarDecoder;
        BlackMisc::Weather::CMetarList m_metars;
        BlackMisc::CSettingReadOnly<BlackCore::Vatsim::TVatsimMetars> m_settings { this, &CVatsimMetarReader::reloadSettings };
    };
} // ns
#endif // guard
