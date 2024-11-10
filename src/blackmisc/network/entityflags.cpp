// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/network/entityflags.h"
#include "blackmisc/verify.h"
#include "blackmisc/setbuilder.h"

#include <QStringList>
#include <QtGlobal>
#include <bitset>

namespace BlackMisc::Network
{
    QString CEntityFlags::flagToString(CEntityFlags::EntityFlag flag)
    {
        switch (flag)
        {
        case AircraftIcaoEntity: return QStringLiteral("Aircraft ICAO");
        case AircraftCategoryEntity: return QStringLiteral("Aircraft category");
        case AirlineIcaoEntity: return QStringLiteral("Airline ICAO");
        case AirportEntity: return QStringLiteral("Airport");
        case AllEntities: return QStringLiteral("All");
        case AllIcaoAndCountries: return QStringLiteral("All ICAO + country");
        case AllIcaoEntities: return QStringLiteral("All ICAO");
        case CountryEntity: return QStringLiteral("Country");
        case DistributorEntity: return QStringLiteral("Distributor");
        case DbInfoObjectEntity: return QStringLiteral("Info objects (DB)");
        case SharedInfoObjectEntity: return QStringLiteral("Info objects (shared)");
        case LiveryEntity: return QStringLiteral("Livery");
        case ModelEntity: return QStringLiteral("Model");
        case NoEntity: return QStringLiteral("no data");
        case VatsimDataFile: return QStringLiteral("VATSIM data file");
        case VatsimStatusFile: return QStringLiteral("VATSIM status file");
        default:
            SWIFT_VERIFY_X(false, Q_FUNC_INFO, "wrong flags");
            return "wrong flags";
        }
    }

    QStringList CEntityFlags::entitiesToStringList(CEntityFlags::Entity entities)
    {
        QStringList list;
        if (entities.testFlag(AircraftIcaoEntity)) list << QStringLiteral("Aircraft ICAO");
        if (entities.testFlag(AircraftCategoryEntity)) list << QStringLiteral("Aircraft category");
        if (entities.testFlag(AirlineIcaoEntity)) list << QStringLiteral("Airline ICAO");
        if (entities.testFlag(AirportEntity)) list << QStringLiteral("Airport");
        if (entities.testFlag(CountryEntity)) list << QStringLiteral("Country");
        if (entities.testFlag(DistributorEntity)) list << QStringLiteral("Distributor");
        if (entities.testFlag(DbInfoObjectEntity)) list << QStringLiteral("Info objects (DB)");
        if (entities.testFlag(SharedInfoObjectEntity)) list << QStringLiteral("Info objects (shared)");
        if (entities.testFlag(LiveryEntity)) list << QStringLiteral("Livery");
        if (entities.testFlag(ModelEntity)) list << QStringLiteral("Model");
        if (entities.testFlag(NoEntity)) list << QStringLiteral("no data");
        if (entities.testFlag(VatsimDataFile)) list << QStringLiteral("VATSIM data file");
        if (entities.testFlag(VatsimStatusFile)) list << QStringLiteral("VATSIM status file");
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

    bool CEntityFlags::isFinishedReadStateOrFailure(CEntityFlags::ReadState state)
    {
        if (isFinishedReadState(state)) { return true; }
        return state == ReadFailed || state == ReadSkipped;
    }

    int CEntityFlags::numberOfEntities(CEntityFlags::Entity entities)
    {
        const int c = static_cast<int>(std::bitset<(sizeof(entities) * 8)>(entities).count());
        return c;
    }

    const QString &CEntityFlags::stateToString(CEntityFlags::ReadState flag)
    {
        static const QString f("finished");
        static const QString fr("finished (restricted)");
        static const QString p("parsing");
        static const QString fa("failed");
        static const QString s("skipped");
        static const QString st("read started");
        static const QString x("wrong flags");

        switch (flag)
        {
        case ReadFinishedRestricted: return fr;
        case ReadFinished: return f;
        case ReadParsing: return p;
        case ReadFailed: return fa;
        case ReadSkipped: return s;
        case ReadStarted: return st;
        default:
            SWIFT_VERIFY_X(false, Q_FUNC_INFO, "wrong flags");
            return x;
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
        if (entities.testFlag(AircraftIcaoEntity))
        {
            entities &= ~AircraftIcaoEntity;
            return AircraftIcaoEntity;
        }
        if (entities.testFlag(AirlineIcaoEntity))
        {
            entities &= ~AirlineIcaoEntity;
            return AirlineIcaoEntity;
        }
        if (entities.testFlag(AirportEntity))
        {
            entities &= ~AirportEntity;
            return AirportEntity;
        }
        if (entities.testFlag(LiveryEntity))
        {
            entities &= ~LiveryEntity;
            return LiveryEntity;
        }
        if (entities.testFlag(CountryEntity))
        {
            entities &= ~CountryEntity;
            return CountryEntity;
        }
        if (entities.testFlag(ModelEntity))
        {
            entities &= ~ModelEntity;
            return ModelEntity;
        }
        if (entities.testFlag(DistributorEntity))
        {
            entities &= ~DistributorEntity;
            return DistributorEntity;
        }
        if (entities.testFlag(AircraftCategoryEntity))
        {
            entities &= ~AircraftCategoryEntity;
            return AircraftCategoryEntity;
        }
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
        if (name.contains("category", Qt::CaseInsensitive)) { return AircraftCategoryEntity; }
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
        CSetBuilder<CEntityFlags::Entity> s;
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
