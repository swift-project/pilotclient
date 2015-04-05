/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTSITUATION_H
#define BLACKMISC_AVIATION_AIRCRAFTSITUATION_H

#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/valueobject.h"
#include <QDateTime>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information of an aircraft's situation
        class CAircraftSituation :
            public CValueObject<CAircraftSituation>,
            public Geo::ICoordinateGeodetic, public BlackMisc::ITimestampBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexPosition = BlackMisc::CPropertyIndex::GlobalIndexCAircraftSituation,
                IndexLatitude,
                IndexLongitude,
                IndexAltitude,
                IndexHeading,
                IndexBank,
                IndexPitch,
                IndexGroundspeed,
                IndexCallsign
            };

            //! Default constructor.
            CAircraftSituation() {}

            //! Comprehensive constructor
            CAircraftSituation(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude,
                               const BlackMisc::Aviation::CHeading &heading = {},
                               const BlackMisc::PhysicalQuantities::CAngle &pitch = {},
                               const BlackMisc::PhysicalQuantities::CAngle &bank = {},
                               const BlackMisc::PhysicalQuantities::CSpeed &gs = {})
                : m_position(position), m_altitude(altitude), m_heading(heading), m_pitch(pitch),
                  m_bank(bank), m_groundspeed(gs) {}

            //! Comprehensive constructor
            CAircraftSituation(const BlackMisc::Aviation::CCallsign &correspondingCallsign,
                               const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude,
                               const BlackMisc::Aviation::CHeading &heading = {},
                               const BlackMisc::PhysicalQuantities::CAngle &pitch = {},
                               const BlackMisc::PhysicalQuantities::CAngle &bank = {},
                               const BlackMisc::PhysicalQuantities::CSpeed &gs = {})
                : m_correspondingCallsign(correspondingCallsign),
                  m_position(position), m_altitude(altitude), m_heading(heading), m_pitch(pitch),
                  m_bank(bank), m_groundspeed(gs) {}


            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Get position
            const BlackMisc::Geo::CCoordinateGeodetic &getPosition() const { return this->m_position; }

            //! Set position
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_position = position; }

            //! \copydoc ICoordinateGeodetic::latitude()
            virtual const BlackMisc::Geo::CLatitude &latitude() const override { return this->m_position.latitude(); }

            //! \copydoc ICoordinateGeodetic::longitude()
            virtual const BlackMisc::Geo::CLongitude &longitude() const override { return this->m_position.longitude(); }

            //! Guess if aircraft is "on ground"
            virtual bool isOnGroundGuessed() const;

            //! \copydoc ICoordinateGeodetic::geodeticHeight
            //! \remarks this should be used for elevation as depicted here: http://en.wikipedia.org/wiki/Altitude#mediaviewer/File:Vertical_distances.svg
            const BlackMisc::PhysicalQuantities::CLength &geodeticHeight() const override { return this->m_position.geodeticHeight(); }

            //! Elevation
            //! \sa geodeticHeight
            const BlackMisc::PhysicalQuantities::CLength getElevation() const { return this->geodeticHeight(); }

            //! Elevation
            //! \sa setGeodeticHeight
            void setElevation(const BlackMisc::PhysicalQuantities::CLength &elevation) { return this->m_position.setGeodeticHeight(elevation); }

            //! Height above ground.
            //! Do not confuse with elevation (=geodeticHeight) as in \sa geodeticHeight() / \sa getElevation()
            BlackMisc::PhysicalQuantities::CLength getHeightAboveGround() const;

            //! Get heading
            const BlackMisc::Aviation::CHeading &getHeading() const { return this->m_heading; }

            //! Set heading
            void setHeading(const BlackMisc::Aviation::CHeading &heading) { this->m_heading = heading; }

            //! Get altitude (true)
            const BlackMisc::Aviation::CAltitude &getAltitude() const { return this->m_altitude; }

            //! Set altitude
            void setAltitude(const BlackMisc::Aviation::CAltitude &altitude) { this->m_altitude = altitude; }

            //! Get pitch
            const BlackMisc::PhysicalQuantities::CAngle &getPitch() const { return this->m_pitch; }

            //! Set pitch
            void setPitch(const BlackMisc::PhysicalQuantities::CAngle &pitch) { this->m_pitch = pitch; }

            //! Get bank (angle)
            const BlackMisc::PhysicalQuantities::CAngle &getBank() const { return this->m_bank; }

            //! Set bank (angle)
            void setBank(const BlackMisc::PhysicalQuantities::CAngle &bank) { this->m_bank = bank; }

            //! Get groundspeed
            const BlackMisc::PhysicalQuantities::CSpeed &getGroundSpeed() const { return this->m_groundspeed; }

            //! Set groundspeed
            void setGroundspeed(const BlackMisc::PhysicalQuantities::CSpeed &groundspeed) { this->m_groundspeed = groundspeed; }

            //! Corresponding callsign
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return this->m_correspondingCallsign; }

            //! Corresponding callsign
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) { this->m_correspondingCallsign = callsign; }

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftSituation)
            CCallsign m_correspondingCallsign;
            BlackMisc::Geo::CCoordinateGeodetic m_position;
            BlackMisc::Aviation::CAltitude m_altitude;
            BlackMisc::Aviation::CHeading  m_heading;
            BlackMisc::PhysicalQuantities::CAngle m_pitch;
            BlackMisc::PhysicalQuantities::CAngle m_bank;
            BlackMisc::PhysicalQuantities::CSpeed m_groundspeed;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAircraftSituation, (
                                   o.m_correspondingCallsign,
                                   o.m_position,
                                   o.m_altitude,
                                   o.m_heading,
                                   o.m_pitch,
                                   o.m_bank,
                                   o.m_groundspeed,
                                   o.m_timestampMSecsSinceEpoch
                                   ))

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation)

#endif // guard
