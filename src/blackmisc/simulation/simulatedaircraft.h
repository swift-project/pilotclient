/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATEDAIRCRAFT_H
#define BLACKMISC_SIMULATION_SIMULATEDAIRCRAFT_H

#include "blackmisc/blackmiscexport.h"
#include "aircraftmodel.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/user.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Comprehensive information of an aircraft
        class BLACKMISC_EXPORT CSimulatedAircraft :
            public CValueObject<CSimulatedAircraft>,
            public BlackMisc::Geo::ICoordinateWithRelativePosition
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexCallsign = BlackMisc::CPropertyIndex::GlobalIndexCSimulatedAircraft,
                IndexPilot,
                IndexDistanceToOwnAircraft,
                IndexCom1System,
                IndexCom2System,
                IndexTransponder,
                IndexSituation,
                IndexAircraftIcaoCode,
                IndexLivery,
                IndexParts,
                IndexIsVtol,
                IndexCombinedIcaoLiveryString,
                IndexModel,
                IndexEnabled,
                IndexRendered,
                IndexPartsSynchronized,
                IndexFastPositionUpdates
            };

            //! Default constructor.
            CSimulatedAircraft();

            //! Constructor.
            CSimulatedAircraft(const BlackMisc::Simulation::CAircraftModel &model);

            //! Constructor.
            CSimulatedAircraft(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Network::CUser &user, const BlackMisc::Aviation::CAircraftSituation &situation);

            //! \copydoc BlackMisc::Mixin::Icon::toIcon()
            BlackMisc::CIcon toIcon() const { return this->m_callsign.toIcon(); }

            //! Get callsign.
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return m_callsign; }

            //! Get callsign.
            QString getCallsignAsString() const { return m_callsign.asString(); }

            //! Get situation.
            const BlackMisc::Aviation::CAircraftSituation &getSituation() const { return m_situation; }

            //! Set situation.
            void setSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Get user
            const BlackMisc::Network::CUser &getPilot() const { return m_pilot; }

            //! Get user's real name
            QString getPilotRealname() const { return m_pilot.getRealName(); }

            //! Get user's real id
            QString getPilotId() { return m_pilot.getId(); }

            //! Get aircraft ICAO info
            const BlackMisc::Aviation::CAircraftIcaoCode &getAircraftIcaoCode() const;

            //! Aircraft ICAO code designator
            const QString &getAircraftIcaoCodeDesignator() const;

            //! Aircraft ICAO combined code
            const QString &getAircraftIcaoCombinedType() const;

            //! Set aicraft ICAO code
            bool setAircraftIcaoCode(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode) { return m_model.setAircraftIcaoCode(aircraftIcaoCode);}

            //! Set ICAO info
            bool setIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode);

            //! Get livery
            const BlackMisc::Aviation::CLivery &getLivery() const { return m_livery; }

            //! Airline ICAO code if any
            const BlackMisc::Aviation::CAirlineIcaoCode &getAirlineIcaoCode() const;

            //! Airline ICAO code designator
            const QString &getAirlineIcaoCodeDesignator() const;

            //! Livery
            virtual void setLivery(const BlackMisc::Aviation::CLivery &livery) { this->m_livery = livery; }

            //! Set aircraft ICAO designator
            virtual void setAircraftIcaoDesignator(const QString &designator);

            //! Has valid realname?
            bool hasValidRealName() const { return this->m_pilot.hasValidRealName(); }

            //! Has valid id?
            bool hasValidId() const { return this->m_pilot.hasValidId(); }

            //! Valid designator?
            bool hasAircraftDesignator() const;

            //! Valid airline designator
            bool hasAirlineDesignator() const { return this->m_livery.hasValidAirlineDesignator(); }

            //! Valid designators?
            bool hasAircraftAndAirlineDesignator() const;

            //! Valid callsign?
            bool hasValidCallsign() const { return BlackMisc::Aviation::CCallsign::isValidAircraftCallsign(this->getCallsign().asString()); }

            //! Callsign not empty, no further checks
            bool hasCallsign() const { return !getCallsign().isEmpty(); }

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

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::latitude
            virtual BlackMisc::Geo::CLatitude latitude() const override { return this->m_situation.latitude(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::longitude
            virtual BlackMisc::Geo::CLongitude longitude() const override { return this->m_situation.longitude(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::geodeticHeight
            //! \remarks this should be used for elevation as depicted here: http://en.wikipedia.org/wiki/Altitude#mediaviewer/File:Vertical_distances.svg
            const BlackMisc::PhysicalQuantities::CLength &geodeticHeight() const override { return this->m_situation.geodeticHeight(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override { return this->m_situation.normalVector(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override { return this->m_situation.normalVectorDouble(); }

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
            const BlackMisc::Aviation::CComSystem &getCom1System() const { return this->m_com1system; }

            //! Get COM2 system
            const BlackMisc::Aviation::CComSystem &getCom2System() const { return this->m_com2system; }

            //! Get COM unit
            const BlackMisc::Aviation::CComSystem getComSystem(BlackMisc::Aviation::CComSystem::ComUnit unit) const;

            //! Set COM unit
            void setComSystem(const BlackMisc::Aviation::CComSystem &com, BlackMisc::Aviation::CComSystem::ComUnit unit);

            //! Set COM1 system
            void setCom1System(const BlackMisc::Aviation::CComSystem &comSystem) { this->m_com1system = comSystem; }

            //! Set COM2 system
            void setCom2System(const BlackMisc::Aviation::CComSystem &comSystem) { this->m_com2system = comSystem; }

            //! Set COM1 frequency
            bool setCom1ActiveFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

            //! Set COM2 frequency
            bool setCom2ActiveFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

            //! Set COM frequency
            bool setComActiveFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit unit);

            //! Given SELCAL selected?
            bool isSelcalSelected(const BlackMisc::Aviation::CSelcal &selcal) const { return this->m_selcal == selcal; }

            //! Valid SELCAL?
            bool hasValidSelcal() const { return this->m_selcal.isValid(); }

            //! SELCAL
            const BlackMisc::Aviation::CSelcal getSelcal() const { return m_selcal; }

            //! Cockpit data
            void setCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder);

            //! Cockpit data
            void setCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, int transponderCode, BlackMisc::Aviation::CTransponder::TransponderMode mode);

            //! Own SELCAL code
            void setSelcal(const BlackMisc::Aviation::CSelcal &selcal) { this->m_selcal = selcal; }

            //! Changed cockpit data?
            bool hasChangedCockpitData(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder) const;

            //! Identical COM system?
            bool hasSameComData(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder);

            //! Is any (COM1/2) active frequency within 8.3383kHz channel?
            bool isActiveFrequencyWithin8_33kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const;

            //! Is any (COM1/2) active frequency within 25kHz channel?
            bool isActiveFrequencyWithin25kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const;

            //! Get transponder
            const BlackMisc::Aviation::CTransponder &getTransponder() const { return this->m_transponder; }

            //! Set transponder
            void setTransponder(const BlackMisc::Aviation::CTransponder &transponder) { this->m_transponder = transponder; }

            //! Set transponder mode
            void setTransponderMode(BlackMisc::Aviation::CTransponder::TransponderMode mode) { this->m_transponder.setTransponderMode(mode); }

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

            //! Get aircraft parts
            BlackMisc::Aviation::CAircraftLights getLights() const;

            //! Set aircraft parts
            void setParts(const BlackMisc::Aviation::CAircraftParts &parts);

            //! Set aircraft lights
            void setLights(BlackMisc::Aviation::CAircraftLights &lights);

            //! Set aircraft lights on
            void setAllLightsOn();

            //! Set aircraft lights off
            void setAllLightsOff();

            //! VTOL aircraft?
            bool isVtol() const;

            //! Combined ICAO / color string
            QString getCombinedIcaoLiveryString() const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Compare for index
            int comparePropertyByIndex(const CSimulatedAircraft &compareValue, const CPropertyIndex &index) const;

            //! Get model
            const BlackMisc::Simulation::CAircraftModel &getModel() const { return m_model; }

            //! Get model string
            QString getModelString() const { return m_model.getModelString(); }

            //! Set model
            void setModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! Set model string
            void setModelString(const QString &modelString);

            //! Set callsign
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);

            //! Set pilot
            void setPilot(const BlackMisc::Network::CUser &user);

            //! Enabled?
            bool isEnabled() const { return m_enabled; }

            //! Enabled / disabled
            void setEnabled(bool enabled) { m_enabled = enabled; }

            //! Rendered?
            bool isRendered() const { return m_rendered; }

            //! Support fast position updates
            bool fastPositionUpdates() const {return m_fastPositionUpdates;}

            //! Support fast position updates
            void setFastPositionUpdates(bool useFastPositions) { m_fastPositionUpdates = useFastPositions; }

            //! Rendered?
            void setRendered(bool rendered) { m_rendered = rendered; }

            //! Have parts been synchronized with a remote client?
            bool isPartsSynchronized() const { return m_partsSynchronized; }

            //! Set the synchronisation flag
            void setPartsSynchronized(bool synchronized) { m_partsSynchronized = synchronized; }

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

        private:
            BlackMisc::Aviation::CCallsign          m_callsign;
            BlackMisc::Network::CUser               m_pilot;
            BlackMisc::Aviation::CAircraftSituation m_situation;
            BlackMisc::Aviation::CComSystem         m_com1system;
            BlackMisc::Aviation::CComSystem         m_com2system;
            BlackMisc::Aviation::CTransponder       m_transponder;
            BlackMisc::Aviation::CAircraftParts     m_parts;
            BlackMisc::Aviation::CSelcal            m_selcal;
            BlackMisc::Aviation::CLivery            m_livery;
            BlackMisc::Simulation::CAircraftModel   m_model;
            bool m_enabled = true;              //!< to be displayed in sim
            bool m_rendered = false;            //!< really shown in simulator
            bool m_partsSynchronized = false;   //!< sync.parts
            bool m_fastPositionUpdates = false; //!<use fast position updates

            //! Init, which syncronizes some denormalized values
            void init();

            BLACK_METACLASS(
                CSimulatedAircraft,
                BLACK_METAMEMBER(callsign),
                BLACK_METAMEMBER(pilot),
                BLACK_METAMEMBER(situation),
                BLACK_METAMEMBER(com1system),
                BLACK_METAMEMBER(com2system),
                BLACK_METAMEMBER(transponder),
                BLACK_METAMEMBER(parts),
                BLACK_METAMEMBER(livery),
                BLACK_METAMEMBER(distanceToOwnAircraft),
                BLACK_METAMEMBER(bearingToOwnAircraft),
                BLACK_METAMEMBER(model),
                BLACK_METAMEMBER(enabled),
                BLACK_METAMEMBER(rendered),
                BLACK_METAMEMBER(partsSynchronized),
                BLACK_METAMEMBER(fastPositionUpdates)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatedAircraft)

#endif // guard
