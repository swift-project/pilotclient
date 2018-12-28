/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/entityflags.h"
#include "blackmisc/verify.h"

#include <QStringList>
#include <QtGlobal>
#include <bitset>

namespace BlackMisc
{
    namespace Network
    {
        QString CEntityFlags::flagToString(CEntityFlags::EntityFlag flag)
        {
            switch (flag)
            {
            case AircraftIcaoEntity: return "Aircraft ICAO";
            case AirlineIcaoEntity: return "Airline ICAO";
            case AirportEntity: return "Airport";
            case AllEntities: return "All";
            case AllIcaoAndCountries: return "All ICAO + country";
            case AllIcaoEntities: return "All ICAO";
            case BookingEntity: return "VATSIM bookings";
            case CountryEntity: return "Country";
            case DistributorEntity: return "Distributor";
            case DbInfoObjectEntity: return "Info objects (DB)";
            case SharedInfoObjectEntity: return "Info objects (shared)";
            case LiveryEntity: return "Livery";
            case ModelEntity: return "Model";
            case NoEntity: return "no data";
            case VatsimDataFile: return "VATSIM data file";
            case VatsimStatusFile: return "VATSIM status file";
            default:
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "wrong flags");
                return "wrong flags";
            }
        }

        QString CEntityFlags::flagToString(CEntityFlags::Entity entities)
        {
            return entitiesToString(entities, ", ");
        }

        QStringList CEntityFlags::entitiesToStringList(CEntityFlags::Entity entities)
        {
            QStringList list;
            if (entities.testFlag(AircraftIcaoEntity)) list << "Aircraft ICAO";
            if (entities.testFlag(AirlineIcaoEntity)) list << "Airline ICAO";
            if (entities.testFlag(AirportEntity)) list << "Airport";
            if (entities.testFlag(BookingEntity)) list << "VATSIM bookings";
            if (entities.testFlag(CountryEntity)) list << "Country";
            if (entities.testFlag(DistributorEntity)) list << "Distributor";
            if (entities.testFlag(DbInfoObjectEntity)) list << "Info objects (DB)";
            if (entities.testFlag(SharedInfoObjectEntity)) list << "Info objects (shared)";
            if (entities.testFlag(LiveryEntity)) list << "Livery";
            if (entities.testFlag(ModelEntity)) list << "Model";
            if (entities.testFlag(NoEntity)) list << "no data";
            if (entities.testFlag(VatsimDataFile)) list << "VATSIM data file";
            if (entities.testFlag(VatsimStatusFile)) list << "VATSIM status file";
            return list;
        }

        QString CEntityFlags::entitiesToString(CEntityFlags::Entity entities, const QString &separator)
        {
            return entitiesToStringList(entities).join(separator);
        }

        bool CEntityFlags::isSingleEntity(BlackMisc::Network::CEntityFlags::Entity flag)
        {
            return numberOfEntities(flag) == 1;
        }

        bool CEntityFlags::isFinishedReadState(CEntityFlags::ReadState state)
        {
            return state == ReadFinished || state == ReadFinishedRestricted;
        }

        int CEntityFlags::numberOfEntities(CEntityFlags::Entity entities)
        {
            const int c = static_cast<int>(std::bitset<(sizeof(entities) * 8)>(entities).count());
            return c;
        }

        QString CEntityFlags::flagToString(CEntityFlags::ReadState flag)
        {
            switch (flag)
            {
            case ReadFinished: return "finished";
            case ReadFinishedRestricted: return "finished (restricted)";
            case ReadParsing: return  "parsing";
            case ReadFailed: return "failed";
            case ReadSkipped: return "skipped";
            case ReadStarted: return "read started";
            default:
                BLACK_VERIFY_X(false, Q_FUNC_INFO, "wrong flags");
                return "wrong flags";
            }
        }

        CStatusMessage::StatusSeverity CEntityFlags::flagToSeverity(CEntityFlags::ReadState state)
        {
            switch (state)
            {
            case ReadFinished:
            case ReadFinishedRestricted:
            case ReadParsing:
            case ReadStarted: return CStatusMessage::SeverityInfo;
            case ReadSkipped: return CStatusMessage::SeverityWarning;
            case ReadFailed: return CStatusMessage::SeverityError;
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "Missing state");
                return CStatusMessage::SeverityInfo;
            }
        }

        CEntityFlags::Entity CEntityFlags::entityFlagToEntity(CEntityFlags::EntityFlag entityFlag)
        {
            return static_cast<Entity>(static_cast<int>(entityFlag));
        }

        CEntityFlags::EntityFlag CEntityFlags::entityToEntityFlag(Entity entity)
        {
            return static_cast<EntityFlag>(static_cast<int>(entity));
        }

        CEntityFlags::Entity CEntityFlags::iterateDbEntities(Entity &entities)
        {
            if (entities == NoEntity || entities == DbInfoObjectEntity) { return NoEntity; }
            if (entities.testFlag(AircraftIcaoEntity)) { entities &= ~AircraftIcaoEntity; return AircraftIcaoEntity; }
            if (entities.testFlag(AirlineIcaoEntity))  { entities &= ~AirlineIcaoEntity; return AirlineIcaoEntity; }
            if (entities.testFlag(AirportEntity))      { entities &= ~AirportEntity; return AirportEntity; }
            if (entities.testFlag(LiveryEntity))       { entities &= ~LiveryEntity; return LiveryEntity; }
            if (entities.testFlag(CountryEntity))      { entities &= ~CountryEntity; return CountryEntity; }
            if (entities.testFlag(ModelEntity))        { entities &= ~ModelEntity; return ModelEntity; }
            if (entities.testFlag(DistributorEntity))  { entities &= ~DistributorEntity; return DistributorEntity; }
            return NoEntity;
        }

        bool CEntityFlags::isWarningOrAbove(CEntityFlags::ReadState state)
        {
            const CStatusMessage::StatusSeverity s = flagToSeverity(state);
            switch (s)
            {
            case CStatusMessage::SeverityError:
            case CStatusMessage::SeverityWarning:
                return true;
            default:
                return false;
            }
        }

        bool CEntityFlags::anySwiftDbEntity(Entity entities)
        {
            const Entity dbEntities = entities & AllDbEntitiesNoInfoObjects;
            return dbEntities != CEntityFlags::NoEntity;
        }

        CEntityFlags::Entity CEntityFlags::singleEntityByName(const QString &name)
        {
            // order here is crucial
            if (name.contains("model", Qt::CaseInsensitive)) { return ModelEntity; }
            if (name.contains("aircraft", Qt::CaseInsensitive)) { return AircraftIcaoEntity; }
            if (name.contains("airline", Qt::CaseInsensitive)) { return AirlineIcaoEntity; }
            if (name.contains("airport", Qt::CaseInsensitive)) { return AirportEntity; }
            if (name.contains("distributor", Qt::CaseInsensitive)) { return DistributorEntity; }
            if (name.contains("countr", Qt::CaseInsensitive)) { return CountryEntity; } // singular/plural
            if (name.contains("liver", Qt::CaseInsensitive)) { return LiveryEntity; } // singular/plural
            return NoEntity;
        }

        CEntityFlags::Entity CEntityFlags::multipleEntitiesByNames(const QStringList &names)
        {
            CEntityFlags::Entity entities = NoEntity;
            for (const QString &name : names)
            {
                const CEntityFlags::Entity singleEntity = CEntityFlags::singleEntityByName(name);
                if (singleEntity == NoEntity) { continue; }
                entities |= singleEntity;
            }
            return entities;
        }

        CEntityFlags::EntitySet CEntityFlags::asSingleEntities(Entity entities)
        {
            QSet<CEntityFlags::Entity> s;
            CEntityFlags::Entity currentEntity = iterateDbEntities(entities);
            while (currentEntity != NoEntity)
            {
                s.insert(currentEntity);
                currentEntity = iterateDbEntities(entities);
            }
            return s;
        }

        void CEntityFlags::registerMetadata()
        {
            // this is no value class and I register enums here,
            // that's why I do not use the Mixins
            qRegisterMetaType<CEntityFlags::Entity>();
            qRegisterMetaType<CEntityFlags::EntityFlag>();
            qRegisterMetaType<CEntityFlags::ReadState>();
        }
    } // namespace
} // namespace
