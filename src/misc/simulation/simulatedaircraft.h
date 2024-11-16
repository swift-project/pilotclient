// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SIMULATEDAIRCRAFT_H
#define SWIFT_MISC_SIMULATION_SIMULATEDAIRCRAFT_H

#include <array>

#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <QtGlobal>

#include "misc/aviation/aircraftlights.h"
#include "misc/aviation/aircraftparts.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/livery.h"
#include "misc/aviation/selcal.h"
#include "misc/aviation/transponder.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/metaclass.h"
#include "misc/mixin/mixincompare.h"
#include "misc/network/user.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/propertyindexref.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation, CSimulatedAircraft)

namespace swift::misc
{
    namespace aviation
    {
        class CAircraftIcaoCode;
        class CAltitude;
        class CHeading;
    } // namespace aviation
    namespace physical_quantities
    {
        class CAngle;
        class CSpeed;
    } // namespace physical_quantities

    namespace simulation
    {
        //! Comprehensive information of an aircraft
        class SWIFT_MISC_EXPORT CSimulatedAircraft :
            public CValueObject<CSimulatedAircraft>,
            public geo::ICoordinateWithRelativePosition
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
            CSimulatedAircraft(const aviation::CCallsign &callsign, const network::CUser &user,
                               const aviation::CAircraftSituation &situation);

            //! Constructor.
            CSimulatedAircraft(const aviation::CCallsign &callsign, const CAircraftModel &model,
                               const network::CUser &user, const aviation::CAircraftSituation &situation);

            //! Get callsign.
            const aviation::CCallsign &getCallsign() const { return m_callsign; }

            //! Get callsign.
            QString getCallsignAsString() const { return m_callsign.asString(); }

            //! Get situation.
            const aviation::CAircraftSituation &getSituation() const { return m_situation; }

            //! Set situation. Won't overwrite the velocity unless it held the default value.
            void setSituation(const aviation::CAircraftSituation &situation);

            //! Get 6DOF velocity.
            const aviation::CAircraftVelocity &getVelocity() const { return m_situation.getVelocity(); }

            //! Set 6DOF velocity.
            void setVelocity(const aviation::CAircraftVelocity &velocity) { m_situation.setVelocity(velocity); }

            //! Get user
            const network::CUser &getPilot() const { return m_pilot; }

            //! Get user's real name
            QString getPilotRealName() const { return m_pilot.getRealName(); }

            //! Get user's real id
            QString getPilotId() { return m_pilot.getId(); }

            //! Get aircraft ICAO info
            const aviation::CAircraftIcaoCode &getAircraftIcaoCode() const;

            //! Aircraft ICAO code designator
            const QString &getAircraftIcaoCodeDesignator() const;

            //! Aircraft and Airline ICAO code designators
            QString getAirlineAndAircraftIcaoCodeDesignators() const;

            //! Aircraft ICAO combined code
            const QString &getAircraftIcaoCombinedType() const;

            //! Set aicraft ICAO code
            bool setAircraftIcaoCode(const aviation::CAircraftIcaoCode &aircraftIcaoCode)
            {
                return m_models[CurrentModel].setAircraftIcaoCode(aircraftIcaoCode);
            }

            //! Set ICAO info
            //! \note to be compatible with old version I still allow to set airline here, but I should actually set a
            //! livery
            bool setIcaoCodes(const aviation::CAircraftIcaoCode &aircraftIcaoCode,
                              const aviation::CAirlineIcaoCode &airlineIcaoCode);

            //! Get livery
            const aviation::CLivery &getLivery() const { return m_models[CurrentModel].getLivery(); }

            //! Airline ICAO code if any
            const aviation::CAirlineIcaoCode &getAirlineIcaoCode() const;

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
            bool hasValidCallsign() const
            {
                return aviation::CCallsign::isValidAircraftCallsign(this->getCallsign().asString());
            }

            //! Callsign not empty, no further checks
            bool hasCallsign() const { return !getCallsign().isEmpty(); }

            //! Get position
            geo::CCoordinateGeodetic getPosition() const { return m_situation.getPosition(); }

            //! Set position
            void setPosition(const geo::CCoordinateGeodetic &position) { m_situation.setPosition(position); }

            //! Get altitude
            const aviation::CAltitude &getAltitude() const { return m_situation.getAltitude(); }

            //! Set altitude
            void setAltitude(const aviation::CAltitude &altitude) { m_situation.setAltitude(altitude); }

            //! Get pressure altitude
            const aviation::CAltitude &getPressureAltitude() const { return m_situation.getPressureAltitude(); }

            //! Set pressure altitude
            void setPressureAltitude(const aviation::CAltitude &altitude) { m_situation.setPressureAltitude(altitude); }

            //! Get groundspeed
            const physical_quantities::CSpeed &getGroundSpeed() const { return m_situation.getGroundSpeed(); }

            //! \copydoc swift::misc::geo::ICoordinateGeodetic::latitude
            virtual geo::CLatitude latitude() const override { return m_situation.latitude(); }

            //! \copydoc swift::misc::geo::ICoordinateGeodetic::longitude
            virtual geo::CLongitude longitude() const override { return m_situation.longitude(); }

            //! \copydoc swift::misc::geo::ICoordinateGeodetic::geodeticHeight
            const aviation::CAltitude &geodeticHeight() const override { return m_situation.geodeticHeight(); }

            //! \copydoc swift::misc::geo::ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override { return m_situation.normalVector(); }

            //! \copydoc swift::misc::geo::ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override
            {
                return m_situation.normalVectorDouble();
            }

            //! \copydoc swift::misc::aviation::CAircraftSituation::getGroundElevation
            const aviation::CAltitude &getGroundElevation() const { return m_situation.getGroundElevation(); }

            //! \copydoc swift::misc::aviation::CAircraftSituation::setGroundElevation
            void setGroundElevation(const geo::CElevationPlane &elevation,
                                    aviation::CAircraftSituation::GndElevationInfo info)
            {
                m_situation.setGroundElevation(elevation, info);
            }

            //! \copydoc swift::misc::aviation::CAircraftSituation::setGroundElevation
            void setGroundElevationChecked(const geo::CElevationPlane &elevation,
                                           aviation::CAircraftSituation::GndElevationInfo info)
            {
                m_situation.setGroundElevationChecked(elevation, info);
            }

            //! \copydoc swift::misc::aviation::CAircraftSituation::getHeading
            const aviation::CHeading &getHeading() const { return m_situation.getHeading(); }

            //! \copydoc swift::misc::aviation::CAircraftSituation::getPitch
            const physical_quantities::CAngle &getPitch() const { return m_situation.getPitch(); }

            //! \copydoc swift::misc::aviation::CAircraftSituation::getBank
            const physical_quantities::CAngle &getBank() const { return m_situation.getBank(); }

            //! Get COM1 system
            const aviation::CComSystem &getCom1System() const { return m_com1system; }

            //! Get COM2 system
            const aviation::CComSystem &getCom2System() const { return m_com2system; }

            //! Get COM unit
            aviation::CComSystem getComSystem(aviation::CComSystem::ComUnit unit) const;

            //! Set COM unit
            void setComSystem(const aviation::CComSystem &com, aviation::CComSystem::ComUnit unit);

            //! Set COM1 system
            void setCom1System(const aviation::CComSystem &comSystem) { m_com1system = comSystem; }

            //! Set COM2 system
            void setCom2System(const aviation::CComSystem &comSystem) { m_com2system = comSystem; }

            //! Set COM1 frequency
            bool setCom1ActiveFrequency(const physical_quantities::CFrequency &frequency);

            //! Set COM2 frequency
            bool setCom2ActiveFrequency(const physical_quantities::CFrequency &frequency);

            //! Set COM frequency
            bool setComActiveFrequency(const physical_quantities::CFrequency &frequency,
                                       aviation::CComSystem::ComUnit unit);

            //! Given SELCAL selected?
            bool isSelcalSelected(const aviation::CSelcal &selcal) const { return m_selcal == selcal; }

            //! Valid SELCAL?
            bool hasValidSelcal() const { return m_selcal.isValid(); }

            //! SELCAL
            const aviation::CSelcal getSelcal() const { return m_selcal; }

            //! Set COM unit (all values + transponder and SELCAL)
            void setCockpit(const CSimulatedAircraft &aircraft);

            //! Cockpit data
            void setCockpit(const aviation::CComSystem &com1, const aviation::CComSystem &com2,
                            const aviation::CTransponder &transponder);

            //! Cockpit data
            void setCockpit(const aviation::CComSystem &com1, const aviation::CComSystem &com2, int transponderCode,
                            aviation::CTransponder::TransponderMode mode);

            //! Own SELCAL code
            void setSelcal(const aviation::CSelcal &selcal) { m_selcal = selcal; }

            //! Changed cockpit data?
            bool hasChangedCockpitData(const aviation::CComSystem &com1, const aviation::CComSystem &com2,
                                       const aviation::CTransponder &transponder) const;

            //! Identical COM system?
            bool hasSameComData(const aviation::CComSystem &com1, const aviation::CComSystem &com2,
                                const aviation::CTransponder &transponder);

            //! Is comFrequency selected in COM1 or COM2 as active frequency (with 5 kHz spacing for .x20/.x25 and
            //! .x70/.x75)
            bool hasComActiveFrequency(const physical_quantities::CFrequency &comFrequency) const;

            //! Get transponder
            const aviation::CTransponder &getTransponder() const { return m_transponder; }

            //! Set transponder
            void setTransponder(const aviation::CTransponder &transponder) { m_transponder = transponder; }

            //! Set transponder mode
            bool setTransponderMode(aviation::CTransponder::TransponderMode mode);

            //! Set transponder code
            void setTransponderCode(int code) { m_transponder.setTransponderCode(code); }

            //! Get transponder code
            QString getTransponderCodeFormatted() const { return m_transponder.getTransponderCodeFormatted(); }

            //! Get transponder code
            qint32 getTransponderCode() const { return m_transponder.getTransponderCode(); }

            //! Get transponder mode
            aviation::CTransponder::TransponderMode getTransponderMode() const
            {
                return m_transponder.getTransponderMode();
            }

            //! Is valid for login?
            bool isValidForLogin() const;

            //! Meaningful default settings for COM Systems
            void initComSystems();

            //! Meaningful default settings for Transponder
            void initTransponder();

            //! Get aircraft parts
            const aviation::CAircraftParts &getParts() const { return m_parts; }

            //! Number of engines
            int getEnginesCount() const;

            //! Get aircraft parts
            aviation::CAircraftLights getLights() const;

            //! Set aircraft parts
            void setParts(const aviation::CAircraftParts &parts);

            //! Set aircraft lights
            void setLights(aviation::CAircraftLights &lights);

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
            const simulation::CAircraftModel &getModel() const { return m_models[CurrentModel]; }

            //! Terrain probe?
            bool isTerrainProbe() const { return this->getModel().isTerrainProbe(); }

            //! Get network model
            const simulation::CAircraftModel &getNetworkModel() const { return m_models[NetworkModel]; }

            //! Get network model or (if not existing) model
            const simulation::CAircraftModel &getNetworkModelOrModel() const;

            //! Has a network model been set?
            bool hasNetworkModel() const;

            //! Difference of network and (rendered) aircraft ICAO code
            QString getNetworkModelAircraftIcaoDifference() const;

            //! Difference of network and (rendered) airline ICAO code
            QString getNetworkModelAirlineIcaoDifference() const;

            //! Difference of network and (rendered) livery code
            QString getNetworkModelLiveryDifference() const;

            //! \copydoc swift::misc::simulation::CAircraftModel::getIconFile
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
            const physical_quantities::CLength &getCG() const { return m_models[CurrentModel].getCG(); }

            //! Has CG from model?
            bool hasCG() const { return !this->getCG().isNull(); }

            //! Reset to the newtork model
            bool resetToNetworkModel();

            //! Set the center of gravity
            bool setCG(const physical_quantities::CLength &cg);

            //! Set callsign
            void setCallsign(const aviation::CCallsign &callsign);

            //! Set pilot
            void setPilot(const network::CUser &user);

            //! Enabled? Enable means it shall be displayed in the simulator
            bool isEnabled() const;

            //! Enabled / disabled
            bool setEnabled(bool enabled);

            //! Rendered?
            bool isRendered() const { return m_rendered; }

            //! Support fast position updates
            bool fastPositionUpdates() const { return m_fastPositionUpdates; }

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
            //! \remark normally automatically set from
            //! swift::misc::aviation::CAircraftSituation::hasInboundGroundDetails
            void setSupportingGndFlag(bool supports) { m_supportsGndFlag = supports; }

            //! \copydoc swift::misc::mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc swift::misc::mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

            //! \copydoc swift::misc::mixin::Icon::toIcon()
            CIcons::IconIndex toIcon() const { return m_callsign.toIcon(); }

        private:
            static constexpr int CurrentModel = 0; //!< m_models
            static constexpr int NetworkModel = 1; //!< m_models
            aviation::CCallsign m_callsign;
            network::CUser m_pilot;
            aviation::CAircraftSituation m_situation;
            aviation::CComSystem m_com1system;
            aviation::CComSystem m_com2system;
            aviation::CTransponder m_transponder;
            aviation::CAircraftParts m_parts;
            aviation::CSelcal m_selcal;
            CAircraftModelList m_models = {
                { CAircraftModel(), CAircraftModel() }
            }; //!< Shorter DBus signature: current model, and model received from network
            bool m_enabled = true; //!< to be displayed in simulator
            bool m_rendered = false; //!< really shown in simulator
            bool m_partsSynchronized = false; //!< synchronize parts
            bool m_fastPositionUpdates = false; //!< use fast position updates
            bool m_supportsGndFlag = false; //!< supports gnd. flag

            //! Init, which synchronizes some denormalized values
            void init();

            SWIFT_METACLASS(
                CSimulatedAircraft,
                SWIFT_METAMEMBER(callsign),
                SWIFT_METAMEMBER(pilot),
                SWIFT_METAMEMBER(situation),
                SWIFT_METAMEMBER(com1system),
                SWIFT_METAMEMBER(com2system),
                SWIFT_METAMEMBER(transponder),
                SWIFT_METAMEMBER(parts),
                SWIFT_METAMEMBER(selcal),
                SWIFT_METAMEMBER(models),
                SWIFT_METAMEMBER(enabled),
                SWIFT_METAMEMBER(rendered),
                SWIFT_METAMEMBER(partsSynchronized),
                SWIFT_METAMEMBER(fastPositionUpdates),
                // ICoordinateWithRelativePosition
                SWIFT_METAMEMBER(relativeDistance),
                SWIFT_METAMEMBER(relativeBearing));
        };
    } // namespace simulation
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::simulation::CSimulatedAircraft)

#endif // guard
