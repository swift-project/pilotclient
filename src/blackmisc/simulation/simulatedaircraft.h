/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <QtGlobal>
#include <array>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Simulation, CSimulatedAircraft)

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
            public Geo::ICoordinateWithRelativePosition
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexCallsign = CPropertyIndexRef::GlobalIndexCSimulatedAircraft,
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
                IndexFastPositionUpdates,
                IndexSupportsGndFlag
            };

            //! Default constructor.
            CSimulatedAircraft();

            //! Constructor.
            explicit CSimulatedAircraft(const CAircraftModel &model);

            //! Constructor.
            CSimulatedAircraft(const Aviation::CCallsign &callsign, const Network::CUser &user, const Aviation::CAircraftSituation &situation);

            //! Constructor.
            CSimulatedAircraft(const Aviation::CCallsign &callsign, const CAircraftModel &model, const Network::CUser &user, const Aviation::CAircraftSituation &situation);

            //! Get callsign.
            const Aviation::CCallsign &getCallsign() const { return m_callsign; }

            //! Get callsign.
            QString getCallsignAsString() const { return m_callsign.asString(); }

            //! Get situation.
            const Aviation::CAircraftSituation &getSituation() const { return m_situation; }

            //! Set situation. Won't overwrite the velocity unless it held the default value.
            void setSituation(const Aviation::CAircraftSituation &situation);

            //! Get 6DOF velocity.
            const Aviation::CAircraftVelocity &getVelocity() const { return m_situation.getVelocity(); }

            //! Set 6DOF velocity.
            void setVelocity(const Aviation::CAircraftVelocity &velocity) { m_situation.setVelocity(velocity); }

            //! Get user
            const Network::CUser &getPilot() const { return m_pilot; }

            //! Get user's real name
            QString getPilotRealName() const { return m_pilot.getRealName(); }

            //! Get user's real id
            QString getPilotId() { return m_pilot.getId(); }

            //! Get aircraft ICAO info
            const Aviation::CAircraftIcaoCode &getAircraftIcaoCode() const;

            //! Aircraft ICAO code designator
            const QString &getAircraftIcaoCodeDesignator() const;

            //! Aircraft and Airline ICAO code designators
            QString getAirlineAndAircraftIcaoCodeDesignators() const;

            //! Aircraft ICAO combined code
            const QString &getAircraftIcaoCombinedType() const;

            //! Set aicraft ICAO code
            bool setAircraftIcaoCode(const Aviation::CAircraftIcaoCode &aircraftIcaoCode) { return m_models[CurrentModel].setAircraftIcaoCode(aircraftIcaoCode);}

            //! Set ICAO info
            //! \note to be compatible with old version I still allow to set airline here, but I should actually set a livery
            bool setIcaoCodes(const Aviation::CAircraftIcaoCode &aircraftIcaoCode, const Aviation::CAirlineIcaoCode &airlineIcaoCode);

            //! Get livery
            const Aviation::CLivery &getLivery() const { return m_models[CurrentModel].getLivery(); }

            //! Airline ICAO code if any
            const Aviation::CAirlineIcaoCode &getAirlineIcaoCode() const;

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
            bool hasValidCallsign() const { return Aviation::CCallsign::isValidAircraftCallsign(this->getCallsign().asString()); }

            //! Callsign not empty, no further checks
            bool hasCallsign() const { return !getCallsign().isEmpty(); }

            //! Get position
            Geo::CCoordinateGeodetic getPosition() const { return m_situation.getPosition(); }

            //! Set position
            void setPosition(const Geo::CCoordinateGeodetic &position) { m_situation.setPosition(position); }

            //! Get altitude
            const Aviation::CAltitude &getAltitude() const { return m_situation.getAltitude(); }

            //! Set altitude
            void setAltitude(const Aviation::CAltitude &altitude) { m_situation.setAltitude(altitude); }

            //! Get pressure altitude
            const Aviation::CAltitude &getPressureAltitude() const { return m_situation.getPressureAltitude(); }

            //! Set pressure altitude
            void setPressureAltitude(const Aviation::CAltitude &altitude) { m_situation.setPressureAltitude(altitude); }

            //! Get groundspeed
            const PhysicalQuantities::CSpeed &getGroundSpeed() const { return m_situation.getGroundSpeed(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::latitude
            virtual Geo::CLatitude latitude() const override { return m_situation.latitude(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::longitude
            virtual Geo::CLongitude longitude() const override { return m_situation.longitude(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::geodeticHeight
            const Aviation::CAltitude &geodeticHeight() const override { return m_situation.geodeticHeight(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override { return m_situation.normalVector(); }

            //! \copydoc BlackMisc::Geo::ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override { return m_situation.normalVectorDouble(); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::getGroundElevation
            const Aviation::CAltitude &getGroundElevation() const { return m_situation.getGroundElevation(); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::setGroundElevation
            void setGroundElevation(const Geo::CElevationPlane &elevation, Aviation::CAircraftSituation::GndElevationInfo info) { m_situation.setGroundElevation(elevation, info); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::setGroundElevation
            void setGroundElevationChecked(const Geo::CElevationPlane &elevation, Aviation::CAircraftSituation::GndElevationInfo info) { m_situation.setGroundElevationChecked(elevation, info); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::getHeading
            const Aviation::CHeading &getHeading() const { return m_situation.getHeading(); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::getPitch
            const PhysicalQuantities::CAngle &getPitch() const { return m_situation.getPitch(); }

            //! \copydoc BlackMisc::Aviation::CAircraftSituation::getBank
            const PhysicalQuantities::CAngle &getBank() const { return m_situation.getBank(); }

            //! Get COM1 system
            const Aviation::CComSystem &getCom1System() const { return m_com1system; }

            //! Get COM2 system
            const Aviation::CComSystem &getCom2System() const { return m_com2system; }

            //! Get COM unit
            Aviation::CComSystem getComSystem(Aviation::CComSystem::ComUnit unit) const;

            //! Set COM unit
            void setComSystem(const Aviation::CComSystem &com, Aviation::CComSystem::ComUnit unit);

            //! Set COM1 system
            void setCom1System(const Aviation::CComSystem &comSystem) { m_com1system = comSystem; }

            //! Set COM2 system
            void setCom2System(const Aviation::CComSystem &comSystem) { m_com2system = comSystem; }

            //! Set COM1 frequency
            bool setCom1ActiveFrequency(const PhysicalQuantities::CFrequency &frequency);

            //! Set COM2 frequency
            bool setCom2ActiveFrequency(const PhysicalQuantities::CFrequency &frequency);

            //! Set COM frequency
            bool setComActiveFrequency(const PhysicalQuantities::CFrequency &frequency, Aviation::CComSystem::ComUnit unit);

            //! Given SELCAL selected?
            bool isSelcalSelected(const Aviation::CSelcal &selcal) const { return m_selcal == selcal; }

            //! Valid SELCAL?
            bool hasValidSelcal() const { return m_selcal.isValid(); }

            //! SELCAL
            const Aviation::CSelcal getSelcal() const { return m_selcal; }

            //! Set COM unit (all values + transponder and SELCAL)
            void setCockpit(const CSimulatedAircraft &aircraft);

            //! Cockpit data
            void setCockpit(const Aviation::CComSystem &com1, const Aviation::CComSystem &com2, const Aviation::CTransponder &transponder);

            //! Cockpit data
            void setCockpit(const Aviation::CComSystem &com1, const Aviation::CComSystem &com2, int transponderCode, Aviation::CTransponder::TransponderMode mode);

            //! Own SELCAL code
            void setSelcal(const Aviation::CSelcal &selcal) { m_selcal = selcal; }

            //! Changed cockpit data?
            bool hasChangedCockpitData(const Aviation::CComSystem &com1, const Aviation::CComSystem &com2, const Aviation::CTransponder &transponder) const;

            //! Identical COM system?
            bool hasSameComData(const Aviation::CComSystem &com1, const Aviation::CComSystem &com2, const Aviation::CTransponder &transponder);

            //! Is comFrequency selected in COM1 or COM2 as active frequency (with 5 kHz spacing for .x20/.x25 and .x70/.x75)
            bool hasComActiveFrequency(const PhysicalQuantities::CFrequency &comFrequency) const;

            //! Get transponder
            const Aviation::CTransponder &getTransponder() const { return m_transponder; }

            //! Set transponder
            void setTransponder(const Aviation::CTransponder &transponder) { m_transponder = transponder; }

            //! Set transponder mode
            bool setTransponderMode(Aviation::CTransponder::TransponderMode mode);

            //! Set transponder code
            void setTransponderCode(int code) { m_transponder.setTransponderCode(code); }

            //! Get transponder code
            QString getTransponderCodeFormatted() const { return m_transponder.getTransponderCodeFormatted(); }

            //! Get transponder code
            qint32 getTransponderCode() const { return m_transponder.getTransponderCode(); }

            //! Get transponder mode
            Aviation::CTransponder::TransponderMode getTransponderMode() const { return m_transponder.getTransponderMode(); }

            //! Is valid for login?
            bool isValidForLogin() const;

            //! Meaningful default settings for COM Systems
            void initComSystems();

            //! Meaningful default settings for Transponder
            void initTransponder();

            //! Get aircraft parts
            const Aviation::CAircraftParts &getParts() const { return m_parts; }

            //! Number of engines
            int getEnginesCount() const;

            //! Get aircraft parts
            Aviation::CAircraftLights getLights() const;

            //! Set aircraft parts
            void setParts(const Aviation::CAircraftParts &parts);

            //! Set aircraft lights
            void setLights(Aviation::CAircraftLights &lights);

            //! Set aircraft lights on
            void setAllLightsOn();

            //! Set aircraft lights off
            void setAllLightsOff();

            //! VTOL aircraft?
            bool isVtol() const;

            //! Is military aircraft
            bool isMilitary() const;

            //! Combined ICAO / color string
            QString getCombinedIcaoLiveryString(bool networkModel = false) const;

            //! Compare for index
            int comparePropertyByIndex(CPropertyIndexRef index, const CSimulatedAircraft &compareValue) const;

            //! Get model (model used for mapping)
            const Simulation::CAircraftModel &getModel() const { return m_models[CurrentModel]; }

            //! Terrain probe?
            bool isTerrainProbe() const { return this->getModel().isTerrainProbe(); }

            //! Get network model
            const Simulation::CAircraftModel &getNetworkModel() const { return m_models[NetworkModel]; }

            //! Get network model or (if not existing) model
            const Simulation::CAircraftModel &getNetworkModelOrModel() const;

            //! Has a network model been set?
            bool hasNetworkModel() const;

            //! Difference of network and (rendered) aircraft ICAO code
            QString getNetworkModelAircraftIcaoDifference() const;

            //! Difference of network and (rendered) airline ICAO code
            QString getNetworkModelAirlineIcaoDifference() const;

            //! Difference of network and (rendered) livery code
            QString getNetworkModelLiveryDifference() const;

            //! \copydoc BlackMisc::Simulation::CAircraftModel::getIconFile
            const QString &getIconFile() const { return m_models[CurrentModel].getIconFile(); }

            //! Get model string
            const QString &getModelString() const { return m_models[CurrentModel].getModelString(); }

            //! Set model string
            void setModelString(const QString &modelString);

            //! Has model string?
            bool hasModelString() const { return m_models[CurrentModel].hasModelString(); }

            //! Set model
            void setModel(const CAircraftModel &model);

            //! Set network model
            void setNetworkModel(const CAircraftModel &model);

            //! Get CG from model
            const PhysicalQuantities::CLength &getCG() const { return m_models[CurrentModel].getCG(); }

            //! Has CG from model?
            bool hasCG() const { return !this->getCG().isNull(); }

            //! Reset to the newtork model
            bool resetToNetworkModel();

            //! Set the center of gravity
            bool setCG(const PhysicalQuantities::CLength &cg);

            //! Set callsign
            void setCallsign(const Aviation::CCallsign &callsign);

            //! Set pilot
            void setPilot(const Network::CUser &user);

            //! Enabled? Enable means it shall be displayed in the simulator
            bool isEnabled() const;

            //! Enabled / disabled
            bool setEnabled(bool enabled);

            //! Rendered?
            bool isRendered() const { return m_rendered; }

            //! Support fast position updates
            bool fastPositionUpdates() const {return m_fastPositionUpdates;}

            //! Support fast position updates
            bool setFastPositionUpdates(bool useFastPositions);

            //! Toggle fast position updates
            bool toggleFastPositionUpdates();

            //! Rendered?
            bool setRendered(bool rendered);

            //! Have parts been synchronized with a remote client?
            bool isPartsSynchronized() const { return m_partsSynchronized; }

            //! Set the synchronisation flag
            void setPartsSynchronized(bool synchronized) { m_partsSynchronized = synchronized; }

            //! Is supporting gnd.flag?
            bool isSupportingGndFlag() const { return m_supportsGndFlag; }

            //! Indicate gnd.flag is supported
            //! \remark normally automatically set from BlackMisc::Aviation::CAircraftSituation::hasInboundGroundDetails
            void setSupportingGndFlag(bool supports) { m_supportsGndFlag = supports; }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Icon::toIcon()
            CIcons::IconIndex toIcon() const { return m_callsign.toIcon(); }

        private:
            static constexpr int CurrentModel = 0; //!< m_models
            static constexpr int NetworkModel = 1; //!< m_models
            Aviation::CCallsign           m_callsign;
            Network::CUser                m_pilot;
            Aviation::CAircraftSituation  m_situation;
            Aviation::CComSystem          m_com1system;
            Aviation::CComSystem          m_com2system;
            Aviation::CTransponder        m_transponder;
            Aviation::CAircraftParts      m_parts;
            Aviation::CSelcal             m_selcal;
            CAircraftModelList m_models = {{ CAircraftModel(), CAircraftModel() }}; //!< Shorter DBus signature: current model, and model received from network
            bool m_enabled = true;              //!< to be displayed in simulator
            bool m_rendered = false;            //!< really shown in simulator
            bool m_partsSynchronized = false;   //!< synchronize parts
            bool m_fastPositionUpdates = false; //!< use fast position updates
            bool m_supportsGndFlag = false;     //!< supports gnd. flag

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
