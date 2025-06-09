// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_VATSIM_VATSIMMETARREADER_H
#define SWIFT_CORE_VATSIM_VATSIMMETARREADER_H

#include <QObject>

#include "core/swiftcoreexport.h"
#include "core/threadedreaderperiodic.h"
#include "misc/aviation/airporticaocode.h"
#include "misc/network/entityflags.h"
#include "misc/weather/metar.h"
#include "misc/weather/metardecoder.h"
#include "misc/weather/metarlist.h"

class QNetworkReply;

namespace swift::core::vatsim
{
    //! Read METARs from VATSIM
    class SWIFT_CORE_EXPORT CVatsimMetarReader : public swift::core::CThreadedReaderPeriodic
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimMetarReader(QObject *owner);

        //! Get METARs
        //! \threadsafe
        virtual swift::misc::weather::CMetarList getMetars() const;

        //! Get METAR for airport
        //! \threadsafe
        virtual swift::misc::weather::CMetar
        getMetarForAirport(const swift::misc::aviation::CAirportIcaoCode &icao) const;

    signals:
        //! METARs have been read and converted to swift::misc::weather::CMetarList
        void metarsRead(const swift::misc::weather::CMetarList &metars);

        //! Data have been read
        void dataRead(swift::misc::network::CEntityFlags::Entity entity,
                      swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

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
        swift::misc::CSettingReadOnly<swift::core::vatsim::TVatsimMetars> m_settings {
            this, &CVatsimMetarReader::reloadSettings
        };
    };
} // namespace swift::core::vatsim
#endif // SWIFT_CORE_VATSIM_VATSIMMETARREADER_H
