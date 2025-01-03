// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_ENTITRFLAGS_H
#define SWIFT_MISC_NETWORK_ENTITRFLAGS_H

#include <QFlags>
#include <QMetaType>
#include <QStringList>

#include "misc/statusmessage.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::network
{
    /*!
     * What and state of reading from web services
     */
    class SWIFT_MISC_EXPORT CEntityFlags
    {
    public:
        //! Which data to read, requires corresponding readers
        enum EntityFlag
        {
            NoEntity = 0, //!< no data at all
            DbInfoObjectEntity = 1 << 0, //!< info about DB data (kind of metadata)
            SharedInfoObjectEntity = 1 << 1, //!< info about shared DB data (metadata)
            AircraftIcaoEntity = 1 << 2, //!< ICAO codes for aircraft
            AirlineIcaoEntity = 1 << 3, //!< ICAO codes for airlines
            CountryEntity = 1 << 4, //!< country codes
            DistributorEntity = 1 << 5, //!< distributors
            LiveryEntity = 1 << 6, //!< liveries
            ModelEntity = 1 << 7, //!< models
            MetarEntity = 1 << 8, //!< METAR
            VatsimDataFile = 1 << 9, //!< the VATSIM data file (multiple data entities)
            VatsimStatusFile = 1 << 10, //!< the VATSIM status file (URLs for data files etc.)
            AirportEntity = 1 << 11, //!< airports
            AircraftCategoryEntity = 1 << 12, //!< aircraft category entities
            AllEntities = ((1 << 13) - 1), //!< everything
            AllIcaoEntities = AircraftIcaoEntity | AirlineIcaoEntity, //!< all ICAO codes
            AllIcaoAndCountries =
                AircraftIcaoEntity | AirlineIcaoEntity | CountryEntity, //!< all ICAO codes and countries
            AllIcaoCountriesCategory = AllIcaoAndCountries | AircraftCategoryEntity, //!< includes category
            DistributorLiveryModel = DistributorEntity | LiveryEntity | ModelEntity, //!< Combined
            ModelMatchingEntities = AllIcaoEntities | LiveryEntity | ModelEntity, //!< all needed for model matching
            AllDbEntitiesNoInfoObjects =
                AllIcaoCountriesCategory | DistributorLiveryModel | AirportEntity, //!< all DB entities, no info objects
            AllDbEntities = AllDbEntitiesNoInfoObjects | DbInfoObjectEntity, //!< all DB stuff
            AllDbEntitiesNoInfoObjectsNoAirportsAndCategories =
                AllIcaoAndCountries | DistributorLiveryModel //!< all DB entities, no info objects and airports
        };
        Q_DECLARE_FLAGS(Entity, EntityFlag)

        //! Set of CEntityFlags::Entity
        using EntitySet = QSet<CEntityFlags::Entity>;

        //! State of operation
        enum ReadState
        {
            ReadStarted, //!< reading has been started
            ReadParsing, //!< parsing data
            ReadFinished, //!< reading done
            ReadFinishedRestricted, //!< finished a timestamp restricted read
            ReadFailed, //!< reading failed
            ReadSkipped //!< read skipped, e.g. because network is down
        };

        //! Convert to string
        static QString flagToString(CEntityFlags::EntityFlag flag);

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
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CEntityFlags::EntityFlag)
Q_DECLARE_METATYPE(swift::misc::network::CEntityFlags::Entity)
Q_DECLARE_METATYPE(swift::misc::network::CEntityFlags::ReadState)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::misc::network::CEntityFlags::Entity)

#endif // SWIFT_MISC_NETWORK_ENTITRFLAGS_H
