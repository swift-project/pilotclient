// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTPARTS_H
#define SWIFT_MISC_AVIATION_AIRCRAFTPARTS_H

#include <QMetaType>
#include <QString>

#include "misc/aviation/aircraftenginelist.h"
#include "misc/aviation/aircraftlights.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftParts)

namespace swift::misc::aviation
{
    //! Value object encapsulating information of aircraft's parts
    class SWIFT_MISC_EXPORT CAircraftParts : public CValueObject<CAircraftParts>, public ITimestampWithOffsetBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexLights = CPropertyIndexRef::GlobalIndexCAircraftParts,
            IndexGearDown,
            IndexFlapsPercentage,
            IndexSpoilersOut,
            IndexEngines,
            IndexEnginesAsString,
            IndexIsOnGround
        };

        //! Parts Details
        enum PartsDetails
        {
            NotSet,
            FSDAircraftParts,
            GuessedParts
        };

        //! Default constructor
        CAircraftParts() {}

        //! Default constructor
        CAircraftParts(int flapsPercent);

        //! Constructor
        CAircraftParts(const CAircraftLights &lights, bool gearDown, int flapsPercent, bool spoilersOut,
                       const CAircraftEngineList &engines, bool onGround);

        //! Constructor
        CAircraftParts(const CAircraftLights &lights, bool gearDown, int flapsPercent, bool spoilersOut,
                       const CAircraftEngineList &engines, bool onGround, qint64 timestamp);

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftParts &compareValue) const;

        //! Get aircraft lights
        CAircraftLights getLights() const { return m_lights; }

        //! Lights adjusted depending on engines
        CAircraftLights getAdjustedLights() const;

        //! Reference to lights, meant for easy direct changes of the values
        CAircraftLights &lights() { return m_lights; }

        //! Set aircraft lights
        void setLights(const CAircraftLights &lights) { m_lights = lights; }

        //! Set all lights on
        void setAllLightsOn();

        //! Set all lights off
        void setAllLightsOff();

        //! Is gear down?
        bool isGearDown() const { return m_gearDown; }

        //! Is fixed gear down?
        //! \remark using also other attributes to decide if gear is down
        bool isFixedGearDown() const;

        //! Set gear down
        void setGearDown(bool down) { m_gearDown = down; }

        //! Get flaps position in percent
        int getFlapsPercent() const { return m_flapsPercentage; }

        //! Set flaps position in percent
        void setFlapsPercent(int flapsPercent) { m_flapsPercentage = flapsPercent; }

        //! Are spoilers out?
        bool isSpoilersOut() const { return m_spoilersOut; }

        //! Set spoilers out
        void setSpoilersOut(bool out) { m_spoilersOut = out; }

        //! Get engines
        CAircraftEngineList getEngines() const { return m_engines; }

        //! Direct access to engines, meant for simple value modifications
        CAircraftEngineList &engines() { return m_engines; }

        //! Engine with number
        CAircraftEngine getEngine(int number) const;

        //! Number of engines
        int getEnginesCount() const { return m_engines.size(); }

        //! Is engine with number 1..n on?
        bool isEngineOn(int number) const;

        //! Any engine on?
        bool isAnyEngineOn() const;

        //! Set engines
        void setEngines(const CAircraftEngineList &engines) { m_engines = engines; }

        //! \copydoc CAircraftEngineList::setEngines
        void setEngines(const CAircraftEngine &engine, int engineNumber);

        //! Is aircraft on ground?
        bool isOnGround() const { return m_isOnGround; }

        //! Set aircraft on ground
        void setOnGround(bool onGround) { m_isOnGround = onGround; }

        //! Get parts details
        PartsDetails getPartsDetails() const { return static_cast<PartsDetails>(m_partsDetails); }

        //! Parts details as string
        const QString &getPartsDetailsAsString() const { return partsDetailsToString(this->getPartsDetails()); }

        //! Set parts details
        void setPartsDetails(PartsDetails details) { m_partsDetails = static_cast<int>(details); }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Incremental JSON object
        //! \remark same as toJson, but sets \c is_full_data to \c false
        QJsonObject toIncrementalJson() const;

        //! Full JSON Object
        //! \remark same as toJson, but sets \c is_full_data to \c true
        QJsonObject toFullJson() const;

        //! NULL parts object?
        bool isNull() const;

        //! Equal values, but not comparing timestamp etc.
        bool equalValues(const CAircraftParts &other) const;

        //! NULL parts object
        static const CAircraftParts &null();

        //! Convert to QString
        static const QString &partsDetailsToString(PartsDetails details);

        //! Attribute name
        static const QString &attributeNameIsFullJson();

        //! Number of attributes for full JSON
        static constexpr int attributesCountFullJson = 7;

    private:
        CAircraftLights m_lights;
        CAircraftEngineList m_engines;
        int m_partsDetails = static_cast<int>(NotSet);
        int m_flapsPercentage = 0;
        bool m_gearDown = false;
        bool m_spoilersOut = false;
        bool m_isOnGround = false;
        QString m_guessingDetails; //!< just for debugging, not via DBus ...

        SWIFT_METACLASS(
            CAircraftParts,
            SWIFT_METAMEMBER_NAMED(lights, "lights"),
            SWIFT_METAMEMBER_NAMED(gearDown, "gear_down"),
            SWIFT_METAMEMBER_NAMED(flapsPercentage, "flaps_pct"),
            SWIFT_METAMEMBER_NAMED(spoilersOut, "spoilers_out"),
            SWIFT_METAMEMBER_NAMED(engines, "engines"),
            SWIFT_METAMEMBER_NAMED(isOnGround, "on_ground"),
            SWIFT_METAMEMBER(partsDetails, 0, DisabledForJson | DisabledForComparison),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch, 0, DisabledForJson | DisabledForComparison),
            SWIFT_METAMEMBER(timeOffsetMs, 0, DisabledForJson | DisabledForComparison));
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftParts)

#endif // SWIFT_MISC_AVIATION_AIRCRAFTPARTS_H
