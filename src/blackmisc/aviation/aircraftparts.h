/* Copyright (c) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTPARTS_H
#define BLACKMISC_AVIATION_AIRCRAFTPARTS_H

#include "blackmisc/aviation/aircraftenginelist.h"
#include "blackmisc/aviation/aircraftlights.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CAircraftParts)

namespace BlackMisc::Aviation
{
    //! Value object encapsulating information of aircraft's parts
    class BLACKMISC_EXPORT CAircraftParts :
        public CValueObject<CAircraftParts>,
        public ITimestampWithOffsetBased
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

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
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

        //! \copydoc BlackMisc::Mixin::String::toQString
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

        BLACK_METACLASS(
            CAircraftParts,
            BLACK_METAMEMBER_NAMED(lights, "lights"),
            BLACK_METAMEMBER_NAMED(gearDown, "gear_down"),
            BLACK_METAMEMBER_NAMED(flapsPercentage, "flaps_pct"),
            BLACK_METAMEMBER_NAMED(spoilersOut, "spoilers_out"),
            BLACK_METAMEMBER_NAMED(engines, "engines"),
            BLACK_METAMEMBER_NAMED(isOnGround, "on_ground"),
            BLACK_METAMEMBER(partsDetails, 0, DisabledForJson | DisabledForComparison),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch, 0, DisabledForJson | DisabledForComparison),
            BLACK_METAMEMBER(timeOffsetMs, 0, DisabledForJson | DisabledForComparison)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftParts)

#endif // guard
