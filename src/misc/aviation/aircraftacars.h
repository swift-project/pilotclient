// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTACARS_H
#define SWIFT_MISC_AVIATION_AIRCRAFTACARS_H

#include <QMetaType>
#include <QString>

// #include "misc/aviation/aircraftenginelist.h"
// #include "misc/aviation/aircraftlights.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

// SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftAcars)

namespace swift::misc::aviation
{
    //! Value object encapsulating information of aircraft's acars
    class SWIFT_MISC_EXPORT CAircraftAcars : public CValueObject<CAircraftAcars>, public ITimestampWithOffsetBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexSimrate,
            IndexFuelTotalQuantity
        };

        //! Acars Details
        enum AcarsDetails
        {
            NotSet,
            FSDAircraftAcars,
            GuessedAcars
        };

        //! Default constructor
        CAircraftAcars() = default;

        //! Default constructor
        CAircraftAcars(int simRate, int fuelTotalquantity);

        //! Constructor
        CAircraftAcars(int simRate, int fuelTotalquantity, qint64 timestamp);
        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftAcars &compareValue) const;

        //! Get simrate
        int getSimrate() const { return m_simRate; }

        //! Set simrate
        // void setSimrate(int simRate) { m_simRate = simRate; }

        //! Get fuel total quantity
        double getFuelTotalquantity() const { return m_fuelTotalquantity; }

        //! Set fuel total quantity
        void setFuelTotalquantity(double fuelTotalquantity) { m_fuelTotalquantity = fuelTotalquantity; }

        //! Get acars details
        AcarsDetails getAcarsDetails() const { return static_cast<AcarsDetails>(m_acarsDetails); }

        //! Acars details as string
        const QString &getAcarsDetailsAsString() const { return acarsDetailsToString(this->getAcarsDetails()); }

        //! Set acars details
        void setAcarsDetails(AcarsDetails details) { m_acarsDetails = static_cast<int>(details); }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Incremental JSON object
        //! \remark same as toJson, but sets \c is_full_data to \c false
        QJsonObject toIncrementalJson() const;

        //! Full JSON Object
        //! \remark same as toJson, but sets \c is_full_data to \c true
        QJsonObject toFullJson() const;

        //! NULL acars object?
        bool isNull() const;

        //! Equal values, but not comparing timestamp etc.
        bool equalValues(const CAircraftAcars &other) const;

        //! NULL acars object
        static const CAircraftAcars &null();

        //! Convert to QString
        static const QString &acarsDetailsToString(AcarsDetails details);

        //! Attribute name
        static const QString &attributeNameIsFullJson();

        //! Number of attributes for full JSON
        static constexpr int attributesCountFullJson = 2;

    private:
        // Fuel availability : Reports on the remaining fuel quantity
        // Error messages : Reports on potential problems or
        // malfunctions in the aircraft.Engine condition
        // slid
        //  simulation rate : Reports on the current simulation rate of the aircraft

        int m_acarsDetails = static_cast<int>(NotSet);
        int m_simRate = 1;
        double m_fuelTotalquantity = -1;

        // QString m_guessingDetails; //!< just for debugging, not via DBus ...

        SWIFT_METACLASS(
                CAircraftAcars,
                SWIFT_METAMEMBER_NAMED(simRate, "simrate"),
                SWIFT_METAMEMBER_NAMED(fuelTotalquantity, "fueltotalquantity"),
                SWIFT_METAMEMBER(acarsDetails, 0, DisabledForJson | DisabledForComparison),
                SWIFT_METAMEMBER(timestampMSecsSinceEpoch, 0, DisabledForJson | DisabledForComparison),
                SWIFT_METAMEMBER(timeOffsetMs, 0, DisabledForJson | DisabledForComparison));
    };

} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftAcars)

#endif // SWIFT_MISC_AVIATION_AIRCRAFTACARS_H