// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_VATSIM_VATSIMMETARREADER_H
#define BLACKCORE_VATSIM_VATSIMMETARREADER_H

#include "blackcore/blackcoreexport.h"
#include "misc/weather/metar.h"
#include "misc/weather/metardecoder.h"
#include "misc/weather/metarlist.h"
#include "misc/network/entityflags.h"
#include "misc/aviation/airporticaocode.h"
#include "blackcore/threadedreader.h"

#include <QObject>

class QNetworkReply;

namespace BlackCore::Vatsim
{
    //! Read METARs from VATSIM
    class BLACKCORE_EXPORT CVatsimMetarReader :
        public BlackCore::CThreadedReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimMetarReader(QObject *owner);

        //! Read / re-read metars
        void readInBackgroundThread();

        //! Get METARs
        //! \threadsafe
        virtual swift::misc::weather::CMetarList getMetars() const;

        //! Get METAR for airport
        //! \threadsafe
        virtual swift::misc::weather::CMetar getMetarForAirport(const swift::misc::aviation::CAirportIcaoCode &icao) const;

    signals:
        //! METARs have been read and converted to swift::misc::weather::CMetarList
        void metarsRead(const swift::misc::weather::CMetarList &metars);

        //! Data have been read
        void dataRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

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
        swift::misc::weather::CMetarDecoder m_metarDecoder;
        swift::misc::weather::CMetarList m_metars;
        swift::misc::CSettingReadOnly<BlackCore::Vatsim::TVatsimMetars> m_settings { this, &CVatsimMetarReader::reloadSettings };
    };
} // ns
#endif // guard
