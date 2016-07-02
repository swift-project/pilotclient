/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE
// without the Doxygen exclusion I get a strange no matching class member found for warning in the gcc build

#include "blackcore/db/databasereaderconfig.h"

using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
{
    namespace Db
    {
        CDatabaseReaderConfig::CDatabaseReaderConfig(CEntityFlags::Entity entities, CDbFlags::DataRetrievalMode retrievalFlags, const CTime &cacheLifetime) :
            m_entities(entities), m_retrievalFlags(retrievalFlags), m_cacheLifetime(cacheLifetime)
        {
            // void
        }

        QString CDatabaseReaderConfig::convertToQString(bool i18n) const
        {
            QString s(CDbFlags::flagToString(this->getRetrievalMode()));
            s.append(" ");
            s.append(CEntityFlags::flagToString(this->getEntities()));
            s.append(" ");
            s.append(this->m_cacheLifetime.toFormattedQString(i18n));
            return s;
        }

        CEntityFlags::Entity CDatabaseReaderConfig::getEntities() const
        {
            return static_cast<CEntityFlags::Entity>(this->m_entities);
        }

        CDbFlags::DataRetrievalMode CDatabaseReaderConfig::getRetrievalMode() const
        {
            return static_cast<CDbFlags::DataRetrievalMode>(this->m_retrievalFlags);
        }

        void CDatabaseReaderConfig::markAsDbDown()
        {
            CDbFlags::DataRetrievalMode m = this->getRetrievalMode();
            m = CDbFlags::adjustWhenDbIsDown(m);
            this->m_retrievalFlags = static_cast<int>(m);
        }

        void CDatabaseReaderConfig::setCacheLifetime(const CTime &time)
        {
            this->m_cacheLifetime = time;
        }

        bool CDatabaseReaderConfig::possiblyReadsFromSwiftDb() const
        {
            if (!this->isValid()) { return false; }
            if (!CEntityFlags::anySwiftDbEntity(this->getEntities())) { return false; }
            return (this->getRetrievalMode().testFlag(CDbFlags::DbDirect) || this->getRetrievalMode().testFlag(CDbFlags::Shared));
        }

        bool CDatabaseReaderConfig::isValid() const
        {
            return this->m_entities != BlackMisc::Network::CEntityFlags::NoEntity;
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

        CDatabaseReaderConfigList CDatabaseReaderConfigList::forMappingTool()
        {
            const CTime timeout(5.0, CTimeUnit::min());
            const CDbFlags::DataRetrievalMode retrievalFlags = CDbFlags::CacheThenDb;
            CDatabaseReaderConfigList l;
            l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftIcaoEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::AirlineIcaoEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::DistributorEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::ModelEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::LiveryEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::CountryEntity, retrievalFlags, timeout));
            return l;
        }

        CDatabaseReaderConfigList CDatabaseReaderConfigList::forPilotClient()
        {
            const CTime timeout(24.0, CTimeUnit::h());
            const CDbFlags::DataRetrievalMode retrievalFlags = CDbFlags::CacheThenDb;
            CDatabaseReaderConfigList l;
            l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftIcaoEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::AirlineIcaoEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::DistributorEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::ModelEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::LiveryEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::CountryEntity, retrievalFlags, timeout));
            return l;
        }

        CDatabaseReaderConfigList CDatabaseReaderConfigList::allDirectDbAccess()
        {
            const CTime timeout(0.0, CTimeUnit::min());
            const CDbFlags::DataRetrievalMode retrievalFlags = CDbFlags::DbDirect;
            CDatabaseReaderConfigList l;
            l.push_back(CDatabaseReaderConfig(CEntityFlags::AircraftIcaoEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::AirlineIcaoEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::DistributorEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::ModelEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::LiveryEntity, retrievalFlags, timeout));
            l.push_back(CDatabaseReaderConfig(CEntityFlags::CountryEntity, retrievalFlags, timeout));
            return l;
        }
    } // ns
} // ns

//! \endcond
