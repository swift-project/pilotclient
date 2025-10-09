// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_MATCHINGSTATISTICSENTRY_H
#define SWIFT_MISC_SIMULATION_MATCHINGSTATISTICSENTRY_H

#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::Simulation, CMatchingStatisticsEntry)

namespace swift::misc::simulation
{
    //! Value object for a matching statistics entry.
    class SWIFT_MISC_EXPORT CMatchingStatisticsEntry :
        public swift::misc::CValueObject<CMatchingStatisticsEntry>,
        public swift::misc::ITimestampBased
    {
    public:
        //! Property indexes
        enum ColumnIndex
        {
            IndexSessionId = CPropertyIndexRef::GlobalIndexCMatchingStatisticsEntry,
            IndexModelSetId,
            IndexEntryType,
            IndexEntryTypeAsString,
            IndexEntryTypeAsIcon,
            IndexCount,
            IndexDescription,
            IndexAircraftDesignator,
            IndexAirlineDesignator,
            IndexHasAircraftAirlineCombination
        };

        //! Represents type of entry
        enum EntryType
        {
            Found,
            Missing
        };

        //! Default constructor.
        CMatchingStatisticsEntry() = default;

        //! Constructor
        CMatchingStatisticsEntry(EntryType type, const QString &sessionId, const QString &modelSetId,
                                 const QString &description, const QString &aircraftDesignator,
                                 const QString &airlineDesignator = {});

        //! Session id
        const QString &getSessionId() const { return m_sessionId; }

        //! Set session id
        void setSessionId(const QString &sessionId) { m_sessionId = sessionId.trimmed(); }

        //! Get model set id
        const QString &getModelSetId() const { return m_modelSetId; }

        //! Set model set id
        void setModelSetId(const QString &modelSetId) { m_modelSetId = modelSetId.trimmed(); }

        //! Get missing aircraft designator
        const QString &getAircraftDesignator() const { return m_aircraftDesignator; }

        //! Set missing aircraft designator
        void setAircraftDesignator(const QString &designator) { m_aircraftDesignator = designator.trimmed().toUpper(); }

        //! Get missing airline designator
        const QString &getAirlineDesignator() const { return m_airlineDesignator; }

        //! Set missing airline designator
        void setAirlineDesignator(const QString &designator) { m_airlineDesignator = designator.trimmed().toUpper(); }

        //! Type of entry
        EntryType getEntryType() const;

        //! Missing entry?
        bool isMissing() const;

        //! Set the entry type
        void setEntryType(EntryType type);

        //! Get description
        const QString &getDescription() const { return m_description; }

        //! Set a description
        void setDescription(const QString &description) { m_description = description; }

        //! Current count
        int getCount() const;

        //! Count increased by one
        void increaseCount();

        //! Matches given value?
        bool matches(EntryType type, const QString &sessionId, const QString &aircraftDesignator,
                     const QString &airlineDesignator) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! Compare by index
        int comparePropertyByIndex(CPropertyIndexRef index, const CMatchingStatisticsEntry &compareValue) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Missing aircraft/airline combination?
        bool hasAircraftAirlineCombination() const;

        //! Convert entry type
        static const QString &entryTypeToString(EntryType type);

        //! Convert entry type
        static const swift::misc::CIcon &entryTypeToIcon(EntryType type);

    private:
        QString m_sessionId; //!< Created in session
        QString m_modelSetId; //!< represents model set
        QString m_description; //!< Arbitrary description
        QString m_aircraftDesignator; //!< missing aircraft designator
        QString m_airlineDesignator; //!< missing airline designator
        int m_entryType = Missing; //!< type
        int m_count = 1; //!< quantity

        SWIFT_METACLASS(
            CMatchingStatisticsEntry,
            SWIFT_METAMEMBER(sessionId),
            SWIFT_METAMEMBER(modelSetId),
            SWIFT_METAMEMBER(aircraftDesignator),
            SWIFT_METAMEMBER(airlineDesignator),
            SWIFT_METAMEMBER(description),
            SWIFT_METAMEMBER(entryType),
            SWIFT_METAMEMBER(count));
    };
} // namespace swift::misc::simulation

Q_DECLARE_METATYPE(swift::misc::simulation::CMatchingStatisticsEntry)
Q_DECLARE_METATYPE(swift::misc::simulation::CMatchingStatisticsEntry::EntryType)

#endif // SWIFT_MISC_SIMULATION_MATCHINGSTATISTICSENTRY_H
