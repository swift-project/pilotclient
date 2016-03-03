/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_ENTITRFLAGS_H
#define BLACKMISC_NETWORK_ENTITRFLAGS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/statusmessage.h"
#include <QObject>

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * What and state of reading from web services
         */
        class BLACKMISC_EXPORT CEntityFlags
        {
        public:
            //! Which data to read, requires corresponding readers
            enum EntityFlag
            {
                NoEntity               = 0,            //!< no data at all
                BookingEntity          = 1 << 0,       //!< bookings
                MetarEntity            = 1 << 1,       //!< METAR
                AircraftIcaoEntity     = 1 << 2,       //!< ICAO codes for aircraft
                AirlineIcaoEntity      = 1 << 3,       //!< ICAO codes for airlines
                CountryEntity          = 1 << 4,       //!< country codes
                DistributorEntity      = 1 << 5,       //!< distributors
                LiveryEntity           = 1 << 6,       //!< liveries
                ModelEntity            = 1 << 7,       //!< models
                VatsimDataFile         = 1 << 8,       //!< the VATSIM data file (multiple data entities)
                VatsimStatusFile       = 1 << 9,       //!< the VATSIM status file (URLs for data files etc.)
                AllEntities            = ((1<<10)-1),  //!< everything
                AllIcaoEntities        = AircraftIcaoEntity | AirlineIcaoEntity,                  //!< all ICAO codes
                AllIcaoAndCountries    = AircraftIcaoEntity | AirlineIcaoEntity | CountryEntity,  //!< all ICAO codes and countries
                DistributorLiveryModel = DistributorEntity | LiveryEntity | ModelEntity,          //!< Combinded
                AllDbEntities          = AllIcaoEntities | DistributorLiveryModel,                //!< All DB stuff
            };
            Q_DECLARE_FLAGS(Entity, EntityFlag)

            //! State of operation
            enum ReadState
            {
                StartRead,                 //!< reading has been started
                ReadFinished,              //!< reading done
                ReadFinishedRestricted,    //!< finished a timestamp restricted read
                ReadFailed                 //!< reading failed
            };

            //! Convert to string
            static QString flagToString(EntityFlag flag);

            //! Convert to string
            static QString flagToString(BlackMisc::Network::CEntityFlags::Entity flag);

            //! Convert to string
            static QString flagToString(ReadState flag);

            //! Flag to severity
            static BlackMisc::CStatusMessage::StatusSeverity flagToSeverity(ReadState state);

            //! Read state representing warning or above?
            static bool isWarningOrAbove(ReadState state);

            //! Register metadata
            static void registerMetadata();
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CEntityFlags::EntityFlag)
Q_DECLARE_METATYPE(BlackMisc::Network::CEntityFlags::Entity)
Q_DECLARE_METATYPE(BlackMisc::Network::CEntityFlags::ReadState)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Network::CEntityFlags::Entity)

#endif // guard
