/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_ENTITREADERFLAGS_H
#define BLACKMISC_NETWORK_ENTITREADERFLAGS_H

#include "blackmisc/blackmiscexport.h"
#include <QObject>

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * What and state of reading from DB
         */
        class BLACKMISC_EXPORT CDbFlags
        {
        public:
            //! Which data to read, requires corresponding readers
            enum EntityFlags
            {
                NoEntity               = 0,      ///< no data at all
                VatsimBookings         = 1 << 0, ///< bookings
                VatsimDataFile         = 1 << 1, ///< the VATSIM data file
                AircraftIcaoEntity     = 1 << 2, ///< ICAO codes for aircraft
                AirlineIcaoEntity      = 1 << 3, ///< ICAO codes for airlines
                CountryEntity          = 1 << 4, ///< Country codes
                DistributorEntity      = 1 << 5, ///< distributors
                LiveryEntity           = 1 << 6, ///< liveries
                ModelEntity            = 1 << 7, ///< models
                AllIcaoEntities        = AircraftIcaoEntity | AirlineIcaoEntity, ///< all ICAO codes
                AllIcaoAndCountries    = AircraftIcaoEntity | AirlineIcaoEntity | CountryEntity, ///< all ICAO codes and countries
                DistributorLiveryModel = DistributorEntity | LiveryEntity | ModelEntity, ///< Combinded
                AllEntities            = 0xFFFF  ///< everything
            };
            Q_DECLARE_FLAGS(Entity, EntityFlags)

            //! State of operation
            enum ReadState
            {
                StartRead,    ///< reading has been started
                ReadFinished, ///< reading done
                ReadFailed    ///< reading failed
            };

            //! Convert to string
            static QString flagToString(EntityFlags flag);

            //! Convert to string
            static QString flagToString(BlackMisc::Network::CDbFlags::Entity flag);

            //! Convert to string
            static QString flagToString(ReadState flag);
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CDbFlags::EntityFlags)
Q_DECLARE_METATYPE(BlackMisc::Network::CDbFlags::Entity)
Q_DECLARE_METATYPE(BlackMisc::Network::CDbFlags::ReadState)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Network::CDbFlags::Entity)

#endif // guard
