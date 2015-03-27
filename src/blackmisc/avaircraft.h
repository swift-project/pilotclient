/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AIRCRAFT_H
#define BLACKMISC_AIRCRAFT_H

#include "nwuser.h"
#include "avaircraftsituation.h"
#include "avaircrafticao.h"
#include "avcallsign.h"
#include "avselcal.h"
#include "aviotransponder.h"
#include "aviocomsystem.h"
#include "aviation/aircraftparts.h"
#include "valueobject.h"
#include "namevariantpairlist.h"
#include "propertyindex.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information of an aircraft
        class CAircraft :
            public CValueObject<CAircraft>,
            public BlackMisc::Geo::ICoordinateWithRelativePosition
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexCallsign = BlackMisc::CPropertyIndex::GlobalIndexCAircraft,
                IndexPilot,
                IndexDistanceToOwnAircraft,
                IndexCom1System,
                IndexCom2System,
                IndexTransponder,
                IndexSituation,
                IndexIcao,
                IndexParts,
                IndexIsVtol
            };

            //! Default constructor.
            CAircraft() {}

            //! Constructor.
            CAircraft(const CCallsign &callsign, const BlackMisc::Network::CUser &user, const CAircraftSituation &situation);

            //! \copydoc CValueObject::toIcon()
            virtual BlackMisc::CIcon toIcon() const override { return this->m_callsign.toIcon(); }

            //! Get callsign.
            const CCallsign &getCallsign() const { return m_callsign; }

            //! Get callsign.
            QString getCallsignAsString() const { return m_callsign.asString(); }

            //! Set callsign
            virtual void setCallsign(const CCallsign &callsign);

            //! Get situation.
            const CAircraftSituation &getSituation() const { return m_situation; }

            //! Set situation.
            void setSituation(const CAircraftSituation &situation);

            //! Get user
            const BlackMisc::Network::CUser &getPilot() const { return m_pilot; }

            //! Get user's real name
            QString getPilotRealname() const { return m_pilot.getRealName(); }

            //! Get user's real id
            QString getPilotId() { return m_pilot.getId(); }

            //! Set pilot (user)
            virtual void setPilot(const BlackMisc::Network::CUser &user) { m_pilot = user; this->m_pilot.setCallsign(this->m_callsign);}

            //! Get ICAO info
            const CAircraftIcao &getIcaoInfo() const { return m_icao; }

            //! Set ICAO info
            virtual void setIcaoInfo(const CAircraftIcao &icao) { m_icao = icao; }

            //! Has valid realname?
            bool hasValidRealName() const { return this->m_pilot.hasValidRealName(); }

            //! Has valid id?
            bool hasValidId() const { return this->m_pilot.hasValidId(); }

            //! Valid designator?
            bool hasValidAircraftDesignator() const { return this->m_icao.hasAircraftDesignator(); }

            //! Valid designators?
            bool hasValidAircraftAndAirlineDesignator() const { return this->m_icao.hasAircraftAndAirlineDesignator(); }

            //! Valid callsign
            bool hasValidCallsign() const { return CCallsign::isValidCallsign(this->getCallsign().asString()); }

            //! Get position
            BlackMisc::Geo::CCoordinateGeodetic getPosition() const { return this->m_situation.getPosition(); }

            //! Set position
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_situation.setPosition(position); }

            //! Get altitude
            const BlackMisc::Aviation::CAltitude &getAltitude() const { return this->m_situation.getAltitude(); }

            //! Set altitude
            void setAltitude(const BlackMisc::Aviation::CAltitude &altitude) { this->m_situation.setAltitude(altitude); }

            //! Get groundspeed
            const BlackMisc::PhysicalQuantities::CSpeed &getGroundSpeed() const { return this->m_situation.getGroundSpeed(); }

            //! \copydoc ICoordinateGeodetic::latitude
            virtual const BlackMisc::Geo::CLatitude &latitude() const override { return this->m_situation.latitude(); }

            //! \copydoc ICoordinateGeodetic::longitude
            virtual const BlackMisc::Geo::CLongitude &longitude() const override { return this->m_situation.longitude(); }

            //! \copydoc ICoordinateGeodetic::geodeticHeight
            //! \remarks this should be used for elevation as depicted here: http://en.wikipedia.org/wiki/Altitude#mediaviewer/File:Vertical_distances.svg
            const BlackMisc::PhysicalQuantities::CLength &geodeticHeight() const override { return this->m_situation.geodeticHeight(); }

            //! Elevation
            //! \sa geodeticHeight
            const BlackMisc::PhysicalQuantities::CLength getElevation() const { return this->geodeticHeight(); }

            //! Elevation
            //! \sa setGeodeticHeight
            void setElevation(const BlackMisc::PhysicalQuantities::CLength &elevation) { return this->m_situation.setElevation(elevation); }

            //! Get heading
            const BlackMisc::Aviation::CHeading &getHeading() const { return this->m_situation.getHeading(); }

            //! Get pitch
            const BlackMisc::PhysicalQuantities::CAngle &getPitch() const { return this->m_situation.getPitch(); }

            //! Get bank
            const BlackMisc::PhysicalQuantities::CAngle &getBank() const { return this->m_situation.getBank(); }

            //! Get COM1 system
            const CComSystem &getCom1System() const { return this->m_com1system; }

            //! Get COM2 system
            const CComSystem &getCom2System() const { return this->m_com2system; }

            //! Get COM unit
            const CComSystem getComSystem(CComSystem::ComUnit unit) const;

            //! Set COM unit
            void setComSystem(const CComSystem &com, CComSystem::ComUnit unit);

            //! Set COM1 system
            void setCom1System(const CComSystem &comSystem) { this->m_com1system = comSystem; }

            //! Set COM2 system
            void setCom2System(const CComSystem &comSystem) { this->m_com2system = comSystem; }

            //! Set COM1 frequency
            bool setCom1ActiveFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

            //! Set COM2 frequency
            bool setCom2ActiveFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

            //! Set COM frequency
            bool setComActiveFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, CComSystem::ComUnit unit);

            //! Given SELCAL selected?
            bool isSelcalSelected(const BlackMisc::Aviation::CSelcal &selcal) const { return this->m_selcal == selcal; }

            //! Valid SELCAL?
            bool hasValidSelcal() const { return this->m_selcal.isValid(); }

            //! SELCAL
            const CSelcal getSelcal() const { return m_selcal; }

            //! Cockpit data
            void setCockpit(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder);

            //! Cockpit data
            void setCockpit(const CComSystem &com1, const CComSystem &com2, int transponderCode, CTransponder::TransponderMode mode);

            //! Own SELCAL code
            void setSelcal(const BlackMisc::Aviation::CSelcal &selcal) { this->m_selcal = selcal; }

            //! Changed cockpit data?
            bool hasChangedCockpitData(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder) const;

            //! Identical COM system?
            bool hasSameComData(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder);

            //! Is any (COM1/2) active frequency within 8.3383kHz channel?
            bool isActiveFrequencyWithin8_33kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const
            {
                return this->m_com1system.isActiveFrequencyWithin8_33kHzChannel(comFrequency) ||
                       this->m_com2system.isActiveFrequencyWithin8_33kHzChannel(comFrequency);
            }

            //! Is any (COM1/2) active frequency within 25kHz channel?
            bool isActiveFrequencyWithin25kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const
            {
                return this->m_com1system.isActiveFrequencyWithin25kHzChannel(comFrequency) ||
                       this->m_com2system.isActiveFrequencyWithin25kHzChannel(comFrequency);
            }

            //! Get transponder
            const BlackMisc::Aviation::CTransponder &getTransponder() const { return this->m_transponder; }

            //! Set transponder
            void setTransponder(const CTransponder &transponder) { this->m_transponder = transponder; }

            //! Set transponder mode
            void setTransponderMode(CTransponder::TransponderMode mode) { this->m_transponder.setTransponderMode(mode); }

            //! Set transponder code
            void setTransponderCode(int code) { this->m_transponder.setTransponderCode(code); }

            //! Get transponder code
            QString getTransponderCodeFormatted() const { return this->m_transponder.getTransponderCodeFormatted(); }

            //! Get transponder code
            qint32 getTransponderCode() const { return this->m_transponder.getTransponderCode(); }

            //! Get transponder mode
            BlackMisc::Aviation::CTransponder::TransponderMode getTransponderMode() const { return this->m_transponder.getTransponderMode(); }

            //! Is valid for login?
            bool isValidForLogin() const;

            //! Meaningful default settings for COM Systems
            void initComSystems();

            //! Meaningful default settings for Transponder
            void initTransponder();

            //! Get aircraft parts
            const BlackMisc::Aviation::CAircraftParts &getParts() const { return m_parts; }

            //! Set aircraft parts
            void setParts(const BlackMisc::Aviation::CAircraftParts &parts);

            //! VTOL aircraft?
            bool isVtol() const;

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

        protected:
            //! \copydoc CValueObject::convertToQString()
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraft)
            CCallsign                 m_callsign;
            BlackMisc::Network::CUser m_pilot;
            CAircraftSituation        m_situation;
            CComSystem                m_com1system;
            CComSystem                m_com2system;
            CTransponder              m_transponder;
            CAircraftParts            m_parts;
            CSelcal                   m_selcal;
            CAircraftIcao             m_icao;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAircraft, (
                                   o.m_callsign,
                                   o.m_pilot,
                                   o.m_situation,
                                   o.m_com1system,
                                   o.m_com2system,
                                   o.m_transponder,
                                   o.m_parts,
                                   o.m_icao,
                                   o.m_distanceToOwnAircraft,
                                   o.m_bearingToOwnAircraft))

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraft)

#endif // guard
