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

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/aviation/aircraftlights.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/network/user.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/compare.h"
#include "blackmisc/icon.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <QtGlobal>
#include <array>

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftIcaoCode;
        class CAltitude;
        class CHeading;
    }
    namespace PhysicalQuantities
    {
        class CAngle;
        class CSpeed;
    }

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
                IndexRelativeDistance,
                IndexCom1System,
                IndexCom2System,
                IndexTransponder,
                IndexSituation,
                IndexAircraftIcaoCode,
                IndexLivery,
                IndexParts,
                IndexIsVtol,
                IndexCombinedIcaoLiveryString,
                IndexCombinedIcaoLiveryStringNetworkModel,
                IndexModel,
                IndexNetworkModel,
                IndexNetworkModelAircraftIcaoDifference,
                IndexNetworkModelAirlineIcaoDifference,
                IndexNetworkModelLiveryDifference,
                IndexEnabled,
                IndexRendered,
                IndexPartsSynchronized,
                IndexFastPositionUpdates
            };

            //! Default constructor.
            CSimulatedAircraft();

            //! Constructor.
            explicit CSimulatedAircraft(const BlackMisc::Simulation::CAircraftModel &model);

            //! Constructor.
            CSimulatedAircraft(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Network::CUser &user, const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Constructor.
            CSimulatedAircraft(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::Network::CUser &user, const BlackMisc::Aviation::CAircraftSituation &situation);

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
            QString getPilotRealName() const { return m_pilot.getRealName(); }

            //! Get user's real id
            QString getPilotId() { return m_pilot.getId(); }

            //! Get aircraft ICAO info
            const BlackMisc::Aviation::CAircraftIcaoCode &getAircraftIcaoCode() const;

            //! Aircraft ICAO code designator
            const QString &getAircraftIcaoCodeDesignator() const;

            //! Aircraft ICAO combined code
            const QString &getAircraftIcaoCombinedType() const;

            //! Set aicraft ICAO code
            bool setAircraftIcaoCode(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode) { return m_models[CurrentModel].setAircraftIcaoCode(aircraftIcaoCode);}

            //! Set ICAO info
            //! \note to be compatible with old version I still allow to set airline here, but I should actually set a livery
            bool setIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode);

            //! Get livery
            const BlackMisc::Aviation::CLivery &getLivery() const { return m_models[CurrentModel].getLivery(); }

            //! Airline ICAO code if any
            const BlackMisc::Aviation::CAirlineIcaoCode &getAirlineIcaoCode() const;

            //! Airline ICAO code designator
            const QString &getAirlineIcaoCodeDesignator() const;

            //! Set aircraft ICAO designator
            void setAircraftIcaoDesignator(const QString &designator);

            //! Has valid realname?
            bool hasRealName() const { return m_pilot.hasRealName(); }

            //! Has valid id?
            bool hasId() const { return m_pilot.hasId(); }

            //! Valid designator?
            bool hasAircraftDesignator() const;

            //! Valid airline designator
            bool hasAirlineDesignator() const { return m_models[CurrentModel].getLivery().hasValidAirlineDesignator(); }

            //! Valid designators?
            bool hasAircraftAndAirlineDesignator() const;

            //! Valid callsign?
            bool hasValidCallsign() const { return BlackMisc::Aviation::CCallsign::isValidAircraftCallsign(this->getCallsign().asString()); }

            //! Callsign not empty, no further checks
            bool hasCallsign() const { return !getCallsign().isEmpty(); }

            //! Get position
            BlackMisc::Geo::CCoordinateGeodetic getPosition() const { return m_situation.getPosition(); }

            //! Set position
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { m_situation.setPosition(position); }

            //! Get altitude
            const BlackMisc::Aviation::CAltitude &getAltitude() const { return m_situation.getAltitude(); }

            //! Set altitude
            void setAltitude(const BlackMisc::Aviation::CAltitude &altitude) { m_situation.setAltitude(altitude); }

            //! Get pressure altitude
            const BlackMisc::Aviation::CAltitude &getPressureAltitude() const { return m_situation.getPressureAltitude(); }

            //! Set pressure altitude
            void setPressureAltitude(const BlackMisc::Aviation::CAltitude &altitude) { m_situation.setPressureAltitude(altitude); }

            //! Get groundspeed
            const BlackMisc::PhysicalQuantities::CSpeed &getGroundSpeed() const { return m_situation.getGroundSpeed(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::latitude
            virtual BlackMisc::Geo::CLatitude latitude() const override { return m_situation.latitude(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::longitude
            virtual BlackMisc::Geo::CLongitude longitude() const override { return m_situation.longitude(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::geodeticHeight
            const BlackMisc::Aviation::CAltitude &geodeticHeight() const override { return m_situation.geodeticHeight(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override { return m_situation.normalVector(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override { return m_situation.normalVectorDouble(); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::getGroundElevation
            const BlackMisc::Aviation::CAltitude &getGroundElevation() const { return m_situation.getGroundElevation(); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::setGroundElevation
            void setGroundElevation(const BlackMisc::Aviation::CAltitude &elevation) { m_situation.setGroundElevation(elevation); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::getHeading
            const BlackMisc::Aviation::CHeading &getHeading() const { return m_situation.getHeading(); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::getPitch
            const BlackMisc::PhysicalQuantities::CAngle &getPitch() const { return m_situation.getPitch(); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::getBank
            const BlackMisc::PhysicalQuantities::CAngle &getBank() const { return m_situation.getBank(); }

            //! Get COM1 system
            const BlackMisc::Aviation::CComSystem &getCom1System() const { return m_com1system; }

            //! Get COM2 system
            const BlackMisc::Aviation::CComSystem &getCom2System() const { return m_com2system; }

            //! Get COM unit
            const BlackMisc::Aviation::CComSystem getComSystem(BlackMisc::Aviation::CComSystem::ComUnit unit) const;

            //! Set COM unit
            void setComSystem(const BlackMisc::Aviation::CComSystem &com, BlackMisc::Aviation::CComSystem::ComUnit unit);

            //! Set COM1 system
            void setCom1System(const BlackMisc::Aviation::CComSystem &comSystem) { m_com1system = comSystem; }

            //! Set COM2 system
            void setCom2System(const BlackMisc::Aviation::CComSystem &comSystem) { m_com2system = comSystem; }

            //! Set COM1 frequency
            bool setCom1ActiveFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

            //! Set COM2 frequency
            bool setCom2ActiveFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

            //! Set COM frequency
            bool setComActiveFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit unit);

            //! Given SELCAL selected?
            bool isSelcalSelected(const BlackMisc::Aviation::CSelcal &selcal) const { return m_selcal == selcal; }

            //! Valid SELCAL?
            bool hasValidSelcal() const { return m_selcal.isValid(); }

            //! SELCAL
            const BlackMisc::Aviation::CSelcal getSelcal() const { return m_selcal; }

            //! Set COM unit (all values + transponder and SELCAL)
            void setCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Cockpit data
            void setCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder);

            //! Cockpit data
            void setCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, int transponderCode, BlackMisc::Aviation::CTransponder::TransponderMode mode);

            //! Own SELCAL code
            void setSelcal(const BlackMisc::Aviation::CSelcal &selcal) { m_selcal = selcal; }

            //! Changed cockpit data?
            bool hasChangedCockpitData(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder) const;

            //! Identical COM system?
            bool hasSameComData(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder);

            //! Is any (COM1/2) active frequency within 8.3383kHz channel?
            bool isActiveFrequencyWithin8_33kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const;

            //! Is any (COM1/2) active frequency within 25kHz channel?
            bool isActiveFrequencyWithin25kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const;

            //! Get transponder
            const BlackMisc::Aviation::CTransponder &getTransponder() const { return m_transponder; }

            //! Set transponder
            void setTransponder(const BlackMisc::Aviation::CTransponder &transponder) { m_transponder = transponder; }

            //! Set transponder mode
            void setTransponderMode(BlackMisc::Aviation::CTransponder::TransponderMode mode) { m_transponder.setTransponderMode(mode); }

            //! Set transponder code
            void setTransponderCode(int code) { m_transponder.setTransponderCode(code); }

            //! Get transponder code
            QString getTransponderCodeFormatted() const { return m_transponder.getTransponderCodeFormatted(); }

            //! Get transponder code
            qint32 getTransponderCode() const { return m_transponder.getTransponderCode(); }

            //! Get transponder mode
            BlackMisc::Aviation::CTransponder::TransponderMode getTransponderMode() const { return m_transponder.getTransponderMode(); }

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
            QString getCombinedIcaoLiveryString(bool networkModel = false) const;

            //! Compare for index
            int comparePropertyByIndex(const CPropertyIndex &index, const CSimulatedAircraft &compareValue) const;

            //! Get model (model used for mapping)
            const BlackMisc::Simulation::CAircraftModel &getModel() const { return m_models[CurrentModel]; }

            //! Get network model
            const BlackMisc::Simulation::CAircraftModel &getNetworkModel() const { return m_models[NetworkModel]; }

            //! Get network model or (if not existing) model
            const BlackMisc::Simulation::CAircraftModel &getNetworkModelOrModel() const;

            //! Has a network model been set?
            bool hasNetworkModel() const;

            //! Difference of network and (rendered) aircraft ICAO code
            QString getNetworkModelAircraftIcaoDifference() const;

            //! Difference of network and (rendered) airline ICAO code
            QString getNetworkModelAirlineIcaoDifference() const;

            //! Difference of network and (rendered) livery code
            QString getNetworkModelLiveryDifference() const;

            //! \copydoc BlackMisc::Simulation::CAircraftModel::getIconPath
            const QString &getIconPath() const { return m_models[CurrentModel].getIconPath(); }

            //! Get model string
            const QString &getModelString() const { return m_models[CurrentModel].getModelString(); }

            //! Set model string
            void setModelString(const QString &modelString);

            //! Has model string?
            bool hasModelString() const { return m_models[CurrentModel].hasModelString(); }

            //! Set model
            void setModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! Set network model
            void setNetworkModel(const BlackMisc::Simulation::CAircraftModel &model);

            //! Set callsign
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);

            //! Set pilot
            void setPilot(const BlackMisc::Network::CUser &user);

            //! Enabled? Enable means it shall be displayed in the simulator
            bool isEnabled() const;

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

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Icon::toIcon()
            BlackMisc::CIcon toIcon() const { return m_callsign.toIcon(); }

        private:
            static constexpr int CurrentModel = 0; //!< m_models
            static constexpr int NetworkModel = 1; //!< m_models
            BlackMisc::Aviation::CCallsign            m_callsign;
            BlackMisc::Network::CUser                 m_pilot;
            BlackMisc::Aviation::CAircraftSituation   m_situation;
            BlackMisc::Aviation::CComSystem           m_com1system;
            BlackMisc::Aviation::CComSystem           m_com2system;
            BlackMisc::Aviation::CTransponder         m_transponder;
            BlackMisc::Aviation::CAircraftParts       m_parts;
            BlackMisc::Aviation::CSelcal              m_selcal;
            BlackMisc::Simulation::CAircraftModelList m_models = { { CAircraftModel(), CAircraftModel() } }; //!< Shorter DBus signature: current model, and model received from network
            bool m_enabled = true;              //!< to be displayed in simulator
            bool m_rendered = false;            //!< really shown in simulator
            bool m_partsSynchronized = false;   //!< synchronize parts
            bool m_fastPositionUpdates = false; //!< use fast position updates

            //! Init, which synchronizes some denormalized values
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
                BLACK_METAMEMBER(selcal),
                BLACK_METAMEMBER(models),
                BLACK_METAMEMBER(enabled),
                BLACK_METAMEMBER(rendered),
                BLACK_METAMEMBER(partsSynchronized),
                BLACK_METAMEMBER(fastPositionUpdates),
                // ICoordinateWithRelativePosition
                BLACK_METAMEMBER(relativeDistance),
                BLACK_METAMEMBER(relativeBearing)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatedAircraft)

#endif // guard
