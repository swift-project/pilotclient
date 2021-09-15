/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE
// without the Doxygen exclusion I get a strange no matching class member found for warning in the gcc build

#include "blackcore/db/databasereaderconfig.h"
#include <QStringBuilder>

using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore::Db
{
    CDatabaseReaderConfig::CDatabaseReaderConfig(CEntityFlags::Entity entities, CDbFlags::DataRetrievalMode retrievalFlags, const CTime &cacheLifetime) :
        m_entities(entities), m_retrievalMode(retrievalFlags), m_cacheLifetime(cacheLifetime)
    {
        // void
    }

    QString CDatabaseReaderConfig::convertToQString(bool i18n) const
    {
        return
            CDbFlags::flagToString(this->getRetrievalMode()) %
            u' ' %
            CEntityFlags::flagToString(this->getEntities()) %
            u' ' %
            m_cacheLifetime.toQString(i18n);
    }

    CEntityFlags::Entity CDatabaseReaderConfig::getEntities() const
    {
        return static_cast<CEntityFlags::Entity>(m_entities);
    }

    bool CDatabaseReaderConfig::supportsEntities(CEntityFlags::Entity entities) const
    {
        const int myEntities = static_cast<int>(this->getEntities());
        const int testEnties = static_cast<int>(entities);
        const int common = myEntities & testEnties;
        return (common == testEnties) || (common == myEntities);
    }

    CDbFlags::DataRetrievalMode CDatabaseReaderConfig::getRetrievalMode() const
    {
        return static_cast<CDbFlags::DataRetrievalMode>(m_retrievalMode);
    }

    void CDatabaseReaderConfig::markAsDbDown()
    {
        CDbFlags::DataRetrievalMode m = this->getRetrievalMode();
        m = CDbFlags::adjustWhenDbIsDown(m);
        m_retrievalMode = static_cast<int>(m);
    }

    void CDatabaseReaderConfig::setCacheLifetime(const CTime &time)
    {
        m_cacheLifetime = time;
    }

    bool CDatabaseReaderConfig::possiblyReadsFromSwiftDb() const
    {
        if (!this->isValid()) { return false; }
        if (!CEntityFlags::anySwiftDbEntity(this->getEntities())) { return false; }
        return (this->getRetrievalMode().testFlag(CDbFlags::DbReading));
    }

    bool CDatabaseReaderConfig::needsSharedInfoFile() const
    {
        if (!this->isValid()) { return false; }
        if (!CEntityFlags::anySwiftDbEntity(this->getEntities())) { return false; }
        return (this->getRetrievalMode().testFlag(CDbFlags::Shared) || this->getRetrievalMode().testFlag(CDbFlags::SharedInfoOnly));
    }

    bool CDatabaseReaderConfig::possiblyWritesToSwiftDb() const
    {
        if (!this->isValid()) { return false; }
        if (!CEntityFlags::anySwiftDbEntity(this->getEntities())) { return false; }
        return (this->getRetrievalMode().testFlag(CDbFlags::DbWriting));
    }

    bool CDatabaseReaderConfig::possiblyReadsFromCache() const
    {
        if (!this->isValid()) { return false; }
        return (this->getRetrievalMode().testFlag(CDbFlags::Cached) || this->getRetrievalMode().testFlag(CDbFlags::CacheThenDb) || this->getRetrievalMode().testFlag(CDbFlags::CacheThenShared));
    }

    bool CDatabaseReaderConfig::isValid() const
    {
        return m_entities != CEntityFlags::NoEntity;
    }

    CDatabaseReaderConfigList::CDatabaseReaderConfigList(const CSequence<CDatabaseReaderConfig> &other) :
        CSequence<CDatabaseReaderConfig>(other)
    { }

    CDatabaseReaderConfig CDatabaseReaderConfigList::findFirstOrDefaultForEntity(const CEntityFlags::Entity entities) const
    {
        const bool single = CEntityFlags::isSingleEntity(entities);
        const CEntityFlags::EntityFlag testFlag = CEntityFlags::entityToEntityFlag(entities); // cannot cast directly
        for (const CDatabaseReaderConfig &config : *this)
        {
            if (single)
            {
                if (config.getEntities().testFlag(testFlag)) { return config; }
            }
            else
            {
                if (config.getEntities() == entities) { return config; }
            }
        }
        return CDatabaseReaderConfig(); // not found
    }

    void CDatabaseReaderConfigList::markAsDbDown()
    {
        for (CDatabaseReaderConfig &config : *this)
        {
            config.markAsDbDown();
        }
    }

    void CDatabaseReaderConfigList::setCacheLifetimes(const CTime &time)
    {
        for (CDatabaseReaderConfig &config : *this)
        {
            config.setCacheLifetime(time);
        }
    }

    bool CDatabaseReaderConfigList::possiblyReadsFromSwiftDb() const
    {
        for (const CDatabaseReaderConfig &config : *this)
        {
            if (config.possiblyReadsFromSwiftDb()) { return true; }
        }
        return false;
    }

    bool CDatabaseReaderConfigList::possiblyWritesToSwiftDb() const
    {
        for (const CDatabaseReaderConfig &config : *this)
        {
            if (config.possiblyWritesToSwiftDb()) { return true; }
        }
        return false;
    }

    bool CDatabaseReaderConfigList::needsSharedInfoObjects(CEntityFlags::Entity entities) const
    {
        for (const CDatabaseReaderConfig &config : *this)
        {
            if (!config.supportsEntities(entities)) { continue; }
            if (config.needsSharedInfoFile()) { return true; }
        }
        return false;
    }

    bool CDatabaseReaderConfigList::needsSharedInfoFile(CEntityFlags::Entity entities) const
    {
        for (const CDatabaseReaderConfig &config : *this)
        {
            if (!config.supportsEntities(entities)) { continue; }
            if (config.needsSharedInfoFile()) { return true; }
        }
        return false;
    }

    bool CDatabaseReaderConfigList::needsSharedInfoObjectsIfCachesEmpty(CEntityFlags::Entity entities, CEntityFlags::Entity cachedEntities) const
    {
        for (const CDatabaseReaderConfig &config : *this)
        {
            if (!config.supportsEntities(entities)) { continue; }
            if (!config.needsSharedInfoFile()) { continue; }
            if (!config.getRetrievalMode().testFlag(CDbFlags::Cached)) { return true; } // does not support caching

            const CEntityFlags::Entity configEntities = config.getEntities();
            const CEntityFlags::Entity configEntitiesNotCached = configEntities & ~cachedEntities;
            if (configEntitiesNotCached != CEntityFlags::NoEntity) { return true; } // we have entities not yet cached
        }
        return false;
    }

    CEntityFlags::Entity CDatabaseReaderConfigList::getEntitesCachedOrReadFromDB() const
    {
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        for (const CDatabaseReaderConfig &config : *this)
        {
            if (config.possiblyReadsFromSwiftDb() || config.possiblyReadsFromCache())
            {
                entities |= config.getEntities();
            }
        }
        return entities;
    }

    CDatabaseReaderConfigList CDatabaseReaderConfigList::forMappingTool()
    {
        const CTime cacheLifetime(5.0, CTimeUnit::min());
        CDbFlags::DataRetrievalMode retrievalFlags = CDbFlags::CacheThenDb;
        retrievalFlags |= CDbFlags::DbWriting;
        CDatabaseReaderConfigList l;
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftIcaoEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AirlineIcaoEntity,  retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AirportEntity, CDbFlags::Ignore, cacheLifetime)); // not needed in mapping tool
        l.push_back(CDatabaseReaderConfig(CEntityFlags::DistributorEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::ModelEntity,   retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::LiveryEntity,  retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::CountryEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftCategoryEntity, retrievalFlags, cacheLifetime));
        return l;
    }

    CDatabaseReaderConfigList CDatabaseReaderConfigList::forPilotClient()
    {
        const CTime cacheLifetime(30.0, CTimeUnit::d());
        const CDbFlags::DataRetrievalMode retrievalFlags = CDbFlags::CacheAndSharedInfo;
        CDbFlags::DataRetrievalMode retrievalFlagsWriting = retrievalFlags;
        retrievalFlagsWriting |= CDbFlags::DbWriting;

        CDatabaseReaderConfigList l;
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftIcaoEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AirlineIcaoEntity,  retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AirportEntity,      retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::DistributorEntity,  retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::ModelEntity,   retrievalFlagsWriting, cacheLifetime)); // for wizard
        l.push_back(CDatabaseReaderConfig(CEntityFlags::LiveryEntity,  retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::CountryEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftCategoryEntity, retrievalFlags, cacheLifetime));
        return l;
    }

    CDatabaseReaderConfigList CDatabaseReaderConfigList::forLauncher()
    {
        const CTime cacheLifetime(30.0, CTimeUnit::d());
        const CDbFlags::DataRetrievalMode retrievalFlags = CDbFlags::CacheThenShared;
        CDatabaseReaderConfigList l;
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftIcaoEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AirlineIcaoEntity, retrievalFlags,  cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AirportEntity, CDbFlags::Ignore,    cacheLifetime)); // not needed in mapping tool
        l.push_back(CDatabaseReaderConfig(CEntityFlags::DistributorEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::ModelEntity, retrievalFlags,   cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::LiveryEntity, retrievalFlags,  cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::CountryEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftCategoryEntity, retrievalFlags, cacheLifetime));
        return l;
    }

    CDatabaseReaderConfigList CDatabaseReaderConfigList::allDirectDbAccess()
    {
        const CTime cacheLifetime(0.0, CTimeUnit::min());
        const CDbFlags::DataRetrievalMode retrievalFlags = CDbFlags::DbReading;
        CDatabaseReaderConfigList l;
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftIcaoEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AirlineIcaoEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AirportEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::DistributorEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::ModelEntity,   retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::LiveryEntity,  retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::CountryEntity, retrievalFlags, cacheLifetime));
        l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftCategoryEntity, retrievalFlags, cacheLifetime));
        return l;
    }
} // ns

//! \endcond
