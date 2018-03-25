/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_SERVICE_H
#define BLACKSIM_XSWIFTBUS_SERVICE_H

//! \file

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "dbusobject.h"
#include "datarefs.h"
#include "messages.h"
#include "navdatareference.h"
#include <XPLM/XPLMNavigation.h>
#include <string>

//! \cond PRIVATE
#define XSWIFTBUS_SERVICE_INTERFACENAME "org.swift_project.xswiftbus.service"
#define XSWIFTBUS_SERVICE_OBJECTPATH "/xswiftbus/service"
//! \endcond

namespace XSwiftBus
{

    //! Simplified implementation of \sa BlackMisc::Simulation::CDistributor
    class CDistributor
    {
    public:
        //! Default constructor
        CDistributor() = default;

        //! Constructor
        CDistributor(const std::string &description) : m_description(description) {}

        //! \copydoc BlackMisc::Simulation::CDistributor::hasDescription
        bool hasDescription() const { return !m_description.empty(); }

        //! \copydoc BlackMisc::Simulation::CDistributor::getDescription
        std::string getDescription() const { return m_description; }

    private:
        std::string m_description;
    };

    //! Simplified implementation of \sa BlackMisc::Simulation::CAircraftModel
    class CAircraftModel
    {
    public:
        CAircraftModel() = default;

        //! \copydoc BlackMisc::Simulation::CAircraftModel::hasDescription
        bool hasDescription() const { return !m_description.empty(); }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::hasAircraftDesignator
        bool hasAircraftDesignator() const { return !m_icao.empty(); }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::getName
        std::string getName() const { return m_name; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::getDistributor
        CDistributor getDistributor() const { return m_distributor; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::getAircraftIcaoCodeDesignator
        std::string getAircraftIcaoCodeDesignator() const { return m_icao; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::getModelString
        std::string getModelString() const { return m_modelString; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::setAircraftIcaoCode
        void setAircraftIcaoCode(const std::string &icao) { m_icao = icao; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::setDescription
        void setDescription(const std::string &description) { m_description = description; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::setName
        void setName(const std::string &name) { m_name = name; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::setDistributor
        void setDistributor(const CDistributor &distributor) { m_distributor = distributor; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::setModelString
        void setModelString(const std::string &modelString) { m_modelString = modelString; }

    private:
        std::string m_name;
        std::string m_icao;
        std::string m_description;
        CDistributor m_distributor;
        std::string m_modelString;
    };

    /*!
     * XSwiftBus service object which is accessible through DBus
     */
    class CService : public CDBusObject
    {
    public:
        //! Constructor
        CService(CDBusConnection *connection);

        //! Destructor
        ~CService() override = default;

        //! DBus interface name
        static const std::string &InterfaceName()
        {
            static const std::string s(XSWIFTBUS_SERVICE_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const std::string &ObjectPath()
        {
            static const std::string s(XSWIFTBUS_SERVICE_OBJECTPATH);
            return s;
        }

        //! Called by XPluginReceiveMessage when the model changes.
        void onAircraftModelChanged();

        //! Add a text message to the on-screen display, with RGB components in the range [0,1]
        void addTextMessage(const std::string &text, double red, double green, double blue);

        //! Called by newly connected client to cause airportsInRangeUpdated to be emitted.
        void updateAirportsInRange();

        //! Get full path to current aircraft model
        std::string getAircraftModelPath() const;

        //! Get base filename of current aircraft model
        std::string getAircraftModelFilename() const;

        //! Get canonical swift model string of current aircraft model
        std::string getAircraftModelString() const;

        //! Get name of current aircraft model
        std::string getAircraftName() const;

        //! Get path to current aircraft livery
        std::string getAircraftLivery() const { return m_liveryPath.get(); }

        //! Get the ICAO code of the current aircraft model
        std::string getAircraftIcaoCode() const { return m_icao.get(); }

        //! Get the description of the current aircraft model
        std::string getAircraftDescription() const { return m_descrip.get(); }

        //! Get major version number
        int getXPlaneVersionMajor() const;

        //! Get minor version number
        int getXPlaneVersionMinor() const;

        //! Get root of X-Plane install path
        std::string getXPlaneInstallationPath() const;

        //! Get full path to X-Plane preferences file
        std::string getXPlanePreferencesPath() const;

        //! True if sim is paused
        bool isPaused() const { return m_paused.get(); }

        //! True if sim time is tracking operating system time
        bool isUsingRealTime() const { return m_useSystemTime.get(); }

        //! Get aircraft latitude in degrees
        double getLatitude() const { return m_latitude.get(); }

        //! Get aircraft longitude in degrees
        double getLongitude() const { return m_longitude.get(); }

        //! Get aircraft altitude in meters
        double getAltitudeMSL() const { return m_elevation.get(); }

        //! Get aircraft height in meters
        double getHeightAGL() const { return m_agl.get(); }

        //! Get aircraft groundspeed in meters per second
        double getGroundSpeed() const { return m_groundSpeed.get(); }

        //! Get aircraft IAS in knots
        double getIndicatedAirspeed() const { return m_indicatedAirspeed.get(); }

        //! Get aircraft TAS in meters per second
        double getTrueAirspeed() const { return m_trueAirspeed.get(); }

        //! Get aircraft pitch in degrees above horizon
        double getPitch() const { return m_pitch.get(); }

        //! Get aircraft roll in degrees
        double getRoll() const { return m_roll.get(); }

        //! Get aircraft true heading in degrees
        double getTrueHeading() const { return m_heading.get(); }

        //! Get whether any wheel is on the ground
        bool getAnyWheelOnGround() const { return m_onGroundAny.get(); }

        //! Get whether all wheels are on the ground
        bool getAllWheelsOnGround() const { return m_onGroundAll.get(); }

        //! Get the current COM1 active frequency in kHz
        int getCom1Active() const { return m_com1Active.get() * 10; }

        //! Get the current COM1 standby frequency in kHz
        int getCom1Standby() const { return m_com1Standby.get() * 10; }

        //! Get the current COM2 active frequency in kHz
        int getCom2Active() const { return m_com2Active.get() * 10; }

        //! Get the current COM2 standby frequency in kHz
        int getCom2Standby() const { return m_com2Standby.get() * 10; }

        //! Get the current transponder code in decimal
        int getTransponderCode() const { return m_xpdrCode.get(); }

        //! Get the current transponder mode (depends on the aircraft, 0 and 1 usually mean standby, >1 active)
        int getTransponderMode() const { return m_xpdrMode.get(); }

        //! Get whether we are currently squawking ident
        bool getTransponderIdent() const { return m_xpdrIdent.get(); }

        //! Get whether beacon lights are on
        bool getBeaconLightsOn() const { return m_beaconLightsOn.get(); }

        //! Get whether landing lights are on
        bool getLandingLightsOn() const { return m_landingLightsOn.get(); }

        //! Get whether nav lights are on
        bool getNavLightsOn() const { return m_navLightsOn.get(); }

        //! Get whether strobe lights are on
        bool getStrobeLightsOn() const { return m_strobeLightsOn.get(); }

        //! Get whether taxi lights are on
        bool getTaxiLightsOn() const { return m_taxiLightsOn.get(); }

        //! Get barometric pressure at sea level in inches of mercury.
        double getQNH() const { return m_qnhInhg.get(); }

        //! Set the current COM1 active frequency in kHz
        void setCom1Active(int freq) { m_com1Active.set(freq / 10); }

        //! Set the current COM1 standby frequency in kHz
        void setCom1Standby(int freq) { m_com1Standby.set(freq / 10); }

        //! Set the current COM2 active frequency in kHz
        void setCom2Active(int freq) { m_com2Active.set(freq / 10); }

        //! Set the current COM2 standby frequency in kHz
        void setCom2Standby(int freq) { m_com2Standby.set(freq / 10); }

        //! Set the current transponder code in decimal
        void setTransponderCode(int code) { m_xpdrCode.set(code); }

        //! Set the current transponder mode (depends on the aircraft, 0 and 1 usually mean standby, >1 active)
        void setTransponderMode(int mode) { m_xpdrMode.set(mode); }

        //! Get flaps deploy ratio, where 0.0 is flaps fully retracted, and 1.0 is flaps fully extended.
        double getFlapsDeployRatio() const { return m_flapsReployRatio.get(); }

        //! Get gear deploy ratio, where 0 is up and 1 is down
        double getGearDeployRatio() const { return m_gearReployRatio.get(); }

        //! Get the number of engines of current aircraft
        int getNumberOfEngines() const { return m_numberOfEngines.get(); }

        //! Get the N1 speed as percent of max (per engine)
        std::vector<double> getEngineN1Percentage() const
        {
            std::vector<double> list;
            const auto number = static_cast<unsigned int>(getNumberOfEngines());
            list.reserve(number);
            for (unsigned int engineNumber = 0; engineNumber < number; ++engineNumber)
            {
                list.push_back(m_enginesN1Percentage.getAt(engineNumber));
            }
            return list;
        }

        //! Get the ratio how much the speedbrakes surfaces are extended (0.0 is fully retracted, and 1.0 is fully extended)
        double getSpeedBrakeRatio() const { return m_speedBrakeRatio.get(); }

        //! \copydoc XSwiftBus::CMessageBoxControl::toggle
        void toggleMessageBoxVisibility() { m_messages.toggle(); }

        int processDBus() override;

    protected:
        DBusHandlerResult dbusMessageHandler(const CDBusMessage &message) override;

    private:
        void emitAircraftModelChanged(const std::string &path, const std::string &filename, const std::string &livery,
                                      const std::string &icao, const std::string &modelString, const std::string &name,
                                      const std::string &description);

        void emitAirportsInRangeUpdated(const std::vector<std::string> &icaoCodes, const std::vector<std::string> &names,
                                        const std::vector<double> &lats, const std::vector<double> &lons, const std::vector<double> &alts);

        CMessageBoxControl m_messages { 128, 128, 16 };
        std::vector<CNavDataReference> m_airports;
        void readAirportsDatabase();

        std::vector<CNavDataReference> findClosestAirports(int number, double latitude, double longitude);

        static CAircraftModel extractAcfProperties(const std::string &filePath);

        StringDataRef<xplane::data::sim::aircraft::view::acf_livery_path> m_liveryPath;
        StringDataRef<xplane::data::sim::aircraft::view::acf_ICAO> m_icao;
        StringDataRef<xplane::data::sim::aircraft::view::acf_descrip> m_descrip;
        DataRef<xplane::data::sim::time::paused> m_paused;
        DataRef<xplane::data::sim::time::use_system_time> m_useSystemTime;
        DataRef<xplane::data::sim::flightmodel::position::latitude> m_latitude;
        DataRef<xplane::data::sim::flightmodel::position::longitude> m_longitude;
        DataRef<xplane::data::sim::flightmodel::position::elevation> m_elevation;
        DataRef<xplane::data::sim::flightmodel::position::y_agl> m_agl;
        DataRef<xplane::data::sim::flightmodel::position::groundspeed> m_groundSpeed;
        DataRef<xplane::data::sim::flightmodel::position::indicated_airspeed2> m_indicatedAirspeed;
        DataRef<xplane::data::sim::flightmodel::position::true_airspeed> m_trueAirspeed;
        DataRef<xplane::data::sim::flightmodel::position::theta> m_pitch;
        DataRef<xplane::data::sim::flightmodel::position::phi> m_roll;
        DataRef<xplane::data::sim::flightmodel::position::psi> m_heading;
        DataRef<xplane::data::sim::flightmodel::failures::onground_any> m_onGroundAny;
        DataRef<xplane::data::sim::flightmodel::failures::onground_all> m_onGroundAll;
        DataRef<xplane::data::sim::cockpit::radios::com1_freq_hz> m_com1Active;
        DataRef<xplane::data::sim::cockpit::radios::com1_stdby_freq_hz> m_com1Standby;
        DataRef<xplane::data::sim::cockpit::radios::com2_freq_hz> m_com2Active;
        DataRef<xplane::data::sim::cockpit::radios::com2_stdby_freq_hz> m_com2Standby;
        DataRef<xplane::data::sim::cockpit::radios::transponder_code> m_xpdrCode;
        DataRef<xplane::data::sim::cockpit::radios::transponder_mode> m_xpdrMode;
        DataRef<xplane::data::sim::cockpit::radios::transponder_id> m_xpdrIdent;
        DataRef<xplane::data::sim::cockpit::electrical::beacon_lights_on> m_beaconLightsOn;
        DataRef<xplane::data::sim::cockpit::electrical::landing_lights_on> m_landingLightsOn;
        DataRef<xplane::data::sim::cockpit::electrical::nav_lights_on> m_navLightsOn;
        DataRef<xplane::data::sim::cockpit::electrical::strobe_lights_on> m_strobeLightsOn;
        DataRef<xplane::data::sim::cockpit::electrical::taxi_light_on> m_taxiLightsOn;
        DataRef<xplane::data::sim::flightmodel2::controls::flap_handle_deploy_ratio> m_flapsReployRatio;
        DataRef<xplane::data::sim::flightmodel2::gear::deploy_ratio> m_gearReployRatio;
        DataRef<xplane::data::sim::aircraft::engine::acf_num_engines> m_numberOfEngines;
        ArrayDataRef<xplane::data::sim::flightmodel::engine::ENGN_N1_> m_enginesN1Percentage;
        DataRef<xplane::data::sim::flightmodel2::controls::speedbrake_ratio> m_speedBrakeRatio;
        DataRef<xplane::data::sim::weather::barometer_sealevel_inhg> m_qnhInhg;
    };
}

#endif // guard

