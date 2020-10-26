/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "matchingstatisticsentry.h"
#include "blackmisc/comparefunctions.h"

namespace BlackMisc
{
    namespace Simulation
    {
        CMatchingStatisticsEntry::CMatchingStatisticsEntry() { }

        CMatchingStatisticsEntry::CMatchingStatisticsEntry(EntryType type, const QString &sessionId, const QString &modelSetId, const QString &description, const QString &aircraftDesignator, const QString &airlineDesignator) :
            m_sessionId(sessionId.trimmed()), m_modelSetId(modelSetId.trimmed()),
            m_description(description),
            m_aircraftDesignator(aircraftDesignator.trimmed().toUpper()),
            m_airlineDesignator(airlineDesignator.trimmed().toUpper()),
            m_entryType(type)
        {
            this->setCurrentUtcTime();
        }

        CMatchingStatisticsEntry::EntryType CMatchingStatisticsEntry::getEntryType() const
        {
            return static_cast<CMatchingStatisticsEntry::EntryType>(m_entryType);
        }

        bool CMatchingStatisticsEntry::isMissing() const
        {
            return this->getEntryType() == Missing;
        }

        void CMatchingStatisticsEntry::setEntryType(CMatchingStatisticsEntry::EntryType type)
        {
            m_entryType = static_cast<int>(type);
        }

        int CMatchingStatisticsEntry::getCount() const
        {
            return m_count;
        }

        void CMatchingStatisticsEntry::increaseCount()
        {
            m_count++;
        }

        bool CMatchingStatisticsEntry::matches(CMatchingStatisticsEntry::EntryType type, const QString &sessionId, const QString &aircraftDesignator, const QString &airlineDesignator) const
        {
            return this->getEntryType() == type && sessionId == this->getSessionId() && aircraftDesignator == this->getAircraftDesignator() && airlineDesignator == this->getAirlineDesignator();
        }

        bool CMatchingStatisticsEntry::hasAircraftAirlineCombination() const
        {
            return !m_aircraftDesignator.isEmpty() && !m_airlineDesignator.isEmpty();
        }

        const CIcon &CMatchingStatisticsEntry::entryTypeToIcon(CMatchingStatisticsEntry::EntryType type)
        {
            switch (type)
            {
            case Found: return CIcon::iconByIndex(CIcons::StandardIconTick16);
            case Missing: return CIcon::iconByIndex(CIcons::StandardIconCross16);
            default:
                qFatal("Wrong Type");
                return CIcon::iconByIndex(CIcons::StandardIconUnknown16);
            }
        }

        const QString &CMatchingStatisticsEntry::entryTypeToString(CMatchingStatisticsEntry::EntryType type)
        {
            static const QString f("found");
            static const QString m("missing");
            static const QString x("ups");

            switch (type)
            {
            case Found: return f;
            case Missing: return m;
            default:
                qFatal("Wrong Type");
                return x;
            }
        }

        CVariant CMatchingStatisticsEntry::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexSessionId: return CVariant::from(m_sessionId);
            case IndexModelSetId: return CVariant::from(m_modelSetId);
            case IndexCount: return CVariant::from(m_count);
            case IndexEntryType: return CVariant::from(m_entryType);
            case IndexEntryTypeAsString: return CVariant::from(entryTypeToString(this->getEntryType()));
            case IndexEntryTypeAsIcon: return CVariant::from(entryTypeToIcon(this->getEntryType()));
            case IndexAircraftDesignator: return CVariant::from(m_aircraftDesignator);
            case IndexAirlineDesignator: return CVariant::from(m_airlineDesignator);
            case IndexDescription: return CVariant::from(m_description);
            case IndexHasAircraftAirlineCombination: return CVariant::from(this->hasAircraftAirlineCombination());
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CMatchingStatisticsEntry::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CMatchingStatisticsEntry>(); return; }
            if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexSessionId: this->setSessionId(variant.value<QString>()); break;
            case IndexModelSetId: this->setModelSetId(variant.value<QString>()); break;
            case IndexAircraftDesignator: m_aircraftDesignator = variant.value<QString>(); break;
            case IndexEntryType: this->setEntryType(static_cast<EntryType>(variant.toInt())); break;
            case IndexCount: m_count = variant.toInt(); break;
            case IndexAirlineDesignator: m_airlineDesignator = variant.value<QString>(); break;
            case IndexDescription: m_description = variant.value<QString>(); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CMatchingStatisticsEntry::comparePropertyByIndex(const CPropertyIndex &index, const CMatchingStatisticsEntry &compareValue) const
        {
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexSessionId: return m_sessionId.compare(compareValue.m_sessionId, Qt::CaseInsensitive);
            case IndexModelSetId: return m_modelSetId.compare(compareValue.getModelSetId(), Qt::CaseInsensitive);
            case IndexEntryTypeAsIcon:
            case IndexEntryTypeAsString:
            case IndexEntryType: return Compare::compare(m_entryType, compareValue.m_entryType);
            case IndexDescription: return m_description.compare(compareValue.getDescription(), Qt::CaseInsensitive);
            case IndexCount: return Compare::compare(m_count, compareValue.m_count);
            case IndexAircraftDesignator: return m_aircraftDesignator.compare(compareValue.m_aircraftDesignator, Qt::CaseInsensitive);
            case IndexAirlineDesignator: return m_airlineDesignator.compare(compareValue.m_airlineDesignator, Qt::CaseInsensitive);
            case IndexHasAircraftAirlineCombination: return Compare::compare(this->hasAircraftAirlineCombination(), compareValue.hasAircraftAirlineCombination());
            default: return CValueObject::comparePropertyByIndex(index, compareValue);
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
            return 0;
        }

        QString CMatchingStatisticsEntry::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            static const QString s("%1 Session: '%2' model set: '%3' aircraft: '%4' airline: '%5' description: '%6'");
            return s.arg(entryTypeToString(getEntryType()), m_sessionId, m_modelSetId, m_aircraftDesignator, m_airlineDesignator, m_description);
        }
    } // namespace
} // namespace
