/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_ENTITRFLAGS_H
#define BLACKMISC_NETWORK_ENTITRFLAGS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/statusmessage.h"

#include <QFlags>
#include <QMetaType>
#include <QStringList>

namespace BlackMisc::Network
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
            NoEntity               = 0,                //!< no data at all
            DbInfoObjectEntity     = 1 << 0,           //!< info about DB data (kind of metadata)
            SharedInfoObjectEntity = 1 << 1,           //!< info about shared DB data (metadata)
            AircraftIcaoEntity     = 1 << 2,           //!< ICAO codes for aircraft
            AirlineIcaoEntity      = 1 << 3,           //!< ICAO codes for airlines
            CountryEntity          = 1 << 4,           //!< country codes
            DistributorEntity      = 1 << 5,           //!< distributors
            LiveryEntity           = 1 << 6,           //!< liveries
            ModelEntity            = 1 << 7,           //!< models
            BookingEntity          = 1 << 8,           //!< bookings
            MetarEntity            = 1 << 9,           //!< METAR
            VatsimDataFile         = 1 << 10,          //!< the VATSIM data file (multiple data entities)
            VatsimStatusFile       = 1 << 11,          //!< the VATSIM status file (URLs for data files etc.)
            AirportEntity          = 1 << 12,          //!< airports
            AircraftCategoryEntity = 1 << 13,          //!< aircraft category entities
            AllEntities            = ((1 << 14) - 1),  //!< everything
            AllIcaoEntities        = AircraftIcaoEntity | AirlineIcaoEntity,                 //!< all ICAO codes
            AllIcaoAndCountries    = AircraftIcaoEntity | AirlineIcaoEntity | CountryEntity, //!< all ICAO codes and countries
            AllIcaoCountriesCategory = AllIcaoAndCountries | AircraftCategoryEntity,         //!< includes category
            DistributorLiveryModel = DistributorEntity  | LiveryEntity      | ModelEntity,   //!< Combined
            ModelMatchingEntities  = AllIcaoEntities    | LiveryEntity      | ModelEntity,   //!< all needed for model matching
            AllDbEntitiesNoInfoObjects = AllIcaoCountriesCategory | DistributorLiveryModel | AirportEntity | AircraftCategoryEntity, //!< all DB entities, no info objects
            AllDbEntities              = AllDbEntitiesNoInfoObjects | DbInfoObjectEntity,                    //!< all DB stuff
            AllDbEntitiesNoInfoObjectsNoAirportsAndCategories = AllIcaoAndCountries | DistributorLiveryModel //!< all DB entities, no info objects and airports
        };
        Q_DECLARE_FLAGS(Entity, EntityFlag)

        //! Set of CEntityFlags::Entity
        using EntitySet = QSet<CEntityFlags::Entity>;

        //! State of operation
        enum ReadState
        {
            ReadStarted,             //!< reading has been started
            ReadParsing,             //!< parsing data
            ReadFinished,            //!< reading done
            ReadFinishedRestricted,  //!< finished a timestamp restricted read
            ReadFailed,              //!< reading failed
            ReadSkipped              //!< read skipped, e.g. because network is down
        };

        //! Convert to string
        static QString flagToString(CEntityFlags::EntityFlag flag);

        //! Convert to string
        //! \deprecated use entities to string
        static QString flagToString(CEntityFlags::Entity entities);

        //! Entities to string list
        static QStringList entitiesToStringList(CEntityFlags::Entity entities);

        //! Entities to string list
        static QString entitiesToString(CEntityFlags::Entity entities, const QString &separator = ", ");

        //! Representing single entity?
        static bool isSingleEntity(CEntityFlags::Entity flag);

        //! Any finished state
        static bool isFinishedReadState(ReadState state);

        //! Any finished state
        static bool isFinishedReadStateOrFailure(ReadState state);

        //! Represented number of entities
        static int numberOfEntities(CEntityFlags::Entity entities);

        //! Convert to string
        static const QString &stateToString(ReadState flag);

        //! Flag to severity
        static CStatusMessage::StatusSeverity flagToSeverity(ReadState state);

        //! To entity (when explicit conversion is needed)
        static Entity entityFlagToEntity(EntityFlag entityFlag);

        //! To flag (when explicit conversion is needed)
        static EntityFlag entityToEntityFlag(Entity entity);

        //! Return single entity and remove it from entities
        static Entity iterateDbEntities(Entity &entities);

        //! Read state representing warning or above?
        static bool isWarningOrAbove(ReadState state);

        //! Representing at least one DB entity
        static bool anySwiftDbEntity(Entity entities);

        //! Get by name
        static Entity singleEntityByName(const QString &name);

        //! Get by multiple names
        static Entity multipleEntitiesByNames(const QStringList &names);

        //! As set of single entities
        static EntitySet asSingleEntities(Entity entities);

        //! Register metadata
        static void registerMetadata();
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CEntityFlags::EntityFlag)
Q_DECLARE_METATYPE(BlackMisc::Network::CEntityFlags::Entity)
Q_DECLARE_METATYPE(BlackMisc::Network::CEntityFlags::ReadState)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Network::CEntityFlags::Entity)

#endif // guard
