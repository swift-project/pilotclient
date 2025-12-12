// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_SERVICE_H
#define SWIFT_SIM_XSWIFTBUS_SERVICE_H

//! \file

#ifndef NOMINMAX
#    define NOMINMAX
#endif

// clang-format off
#include "dbusobject.h"
#include "datarefs.h"
#include "messages.h"
#include "terrainprobe.h"
#include "custom_datarefs.h"
#include <XPLM/XPLMNavigation.h>
#include <string>
#include <chrono>
#include <limits>
// clang-format on

//! \cond PRIVATE
#define XSWIFTBUS_SERVICE_INTERFACENAME "org.swift_project.xswiftbus.service"
#define XSWIFTBUS_SERVICE_OBJECTPATH "/xswiftbus/service"
//! \endcond

namespace XSwiftBus
{
    /*!
     * xswiftbus service object which is accessible through DBus
     */
    class CService : public CDBusObject
    {
    public:
        //! Constructor
        CService(CSettingsProvider *settingsProvider);

        //! Destructor
        virtual ~CService() override;

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

        //! Called by XPluginReceiveMessage when some scenery is loaded.
        void onSceneryLoaded();

        //! Returns the xswiftbus version number
        std::string getVersionNumber() const;

        //! Returns the SHA1 of the last commit that could influence xswiftbus.
        std::string getCommitHash() const;

        //! Add a text message to the on-screen display, with RGB components in the range [0,1]
        void addTextMessage(const std::string &text, double red, double green, double blue);

        //! Get full path to current aircraft model
        std::string getAircraftModelPath() const;

        //! Get base filename of current aircraft model
        std::string getAircraftModelFilename() const;

        //! Get canonical swift model string of current aircraft model
        std::string getAircraftModelString() const;

        //! Get name of current aircraft model
        std::string getAircraftName() const;

        //! Get current aircraft livery
        std::string getAircraftLivery() const;

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

        //! Frames-per-second, averaged over the last 500 frames,
        //! or since this function was last called, whichever is later.
        //! Second part is the average simulation time ratio during the same period.
        //! Third part is the total track miles over-reported during the same period.
        //! Fourth part is the total minutes behind scedule during the same period.
        //! \return Zero if no samples were collected since this function was last called.
        std::tuple<double, double, double, double> getFrameStats();

        //! Reset the monitoring of total miles and minutes lost due to low frame rate.
        void resetFrameTotals();

        //! Set the current connection state
        void setFlightNetworkConnected(bool connected);

        //! Set the current own callsign
        void setOwnCallsign(const std::string &callsign);

        //! Get aircraft latitude in degrees
        double getLatitudeDeg() const { return m_latitude.get(); }

        //! Get aircraft longitude in degrees
        double getLongitudeDeg() const { return m_longitude.get(); }

        //! Get aircraft altitude in meters
        double getAltitudeMslM() const { return m_elevation.get(); }

        //! Get aircraft pressure altitude in feet in standard atmosphere in X-Plane 12.
        //! NaN in earlier versions of X-Plane.
        double getPressureAltitudeFt() const
        {
            return m_pressureAlt.isValid() ? m_pressureAlt.get() : std::numeric_limits<double>::quiet_NaN();
        }

        //! Get aircraft height in meters
        double getHeightAglM() const { return m_agl.get(); }

        //! Get aircraft groundspeed in meters per second
        double getGroundSpeedMps() const { return m_groundSpeed.get(); }

        //! Get aircraft IAS in knots
        double getIndicatedAirspeedKias() const { return m_indicatedAirspeed.get(); }

        //! Get aircraft TAS in meters per second
        double getTrueAirspeedKias() const { return m_trueAirspeed.get(); }

        //! Get aircraft pitch in degrees above horizon
        double getPitchDeg() const { return m_pitch.get(); }

        //! Get aircraft roll in degrees
        double getRollDeg() const { return m_roll.get(); }

        //! Get aircraft true heading in degrees
        double getTrueHeadingDeg() const { return m_heading.get(); }

        //! @{
        //! Get aircraft local velocity in world coordinates meters per second
        double getLocalXVelocityMps() const { return isPaused() ? 0 : m_velocityX.get(); }
        double getLocalYVelocityMps() const { return isPaused() ? 0 : m_velocityY.get(); }
        double getLocalZVelocityMps() const { return isPaused() ? 0 : m_velocityZ.get(); }
        //! @}

        //! @{
        //! Get aircraft angular velocity in radians per second
        double getPitchRadPerSec() const { return isPaused() ? 0 : m_pitchVelocity.get(); }
        double getRollRadPerSec() const { return isPaused() ? 0 : m_rollVelocity.get(); }
        double getHeadingRadPerSec() const { return isPaused() ? 0 : m_headingVelocity.get(); }
        //! @}

        //! Get whether any wheel is on the ground
        bool getAnyWheelOnGround() const { return m_onGroundAny.get(); }

        //! Get whether all wheels are on the ground
        bool getAllWheelsOnGround() const { return m_onGroundAll.get(); }

        //! Get elevation of ground under the plane in meters
        double getGroundElevation() const
        {
            return m_terrainProbe.getElevation(m_latitude.get(), m_longitude.get(), m_elevation.get())[0];
        }

        //! @{
        //! COM Selection 6/7
        int getComSelection() const { return m_comAudioSelection.get(); }
        bool isCom1Selected() const { return this->getComSelection() == 6; }
        bool isCom2Selected() const { return this->getComSelection() == 7; }
        //! @}

        //! Get the current COM1 active frequency in kHz
        int getCom1ActiveKhz() const { return m_com1Active.get(); }

        //! Get the current COM1 standby frequency in kHz
        int getCom1StandbyKhz() const { return m_com1Standby.get(); }

        //! Get the COM1 power on/off
        bool getCom1Power() const { return m_com1Power.get(); }

        //! Get the COM1 listening yes/no
        bool getCom1Listening() const { return m_com1Listening.get(); }

        //! Get the COM1 volume 0..1
        float getCom1Volume() const { return m_com1Volume.get(); }

        //! Is COM1 receiving?
        bool isCom1Receiving() const { return this->getCom1Power() && this->getCom1Listening(); }

        //! Is COM1 transmitting?
        bool isCom1Transmitting() const { return this->getCom1Power() && this->isCom1Selected(); }

        //! Get the current COM2 active frequency in kHz
        int getCom2ActiveKhz() const { return m_com2Active.get(); }

        //! Get the current COM2 standby frequency in kHz
        int getCom2StandbyKhz() const { return m_com2Standby.get(); }

        //! Get the COM2 power on/off
        bool getCom2Power() const { return m_com2Power.get(); }

        //! Get the COM2 listening yes/no
        bool getCom2Listening() const { return m_com2Listening.get(); }

        //! Get the COM2 volume 0..1
        float getCom2Volume() const { return m_com2Volume.get(); }

        //! Is COM2 receiving?
        bool isCom2Receiving() const { return this->getCom2Power() && this->getCom2Listening(); }

        //! Is COM2 transmitting?
        bool isCom2Transmitting() const { return this->getCom2Power() && this->isCom2Selected(); }

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
        double getQNHInHg() const { return m_qnhInhg.get(); }

        //! Set the current COM1 active frequency in kHz
        void setCom1ActiveKhz(int freq) { m_com1Active.set(freq); }

        //! Set the current COM1 standby frequency in kHz
        void setCom1StandbyKhz(int freq) { m_com1Standby.set(freq); }

        //! Set the current COM2 active frequency in kHz
        void setCom2ActiveKhz(int freq) { m_com2Active.set(freq); }

        //! Set the current COM2 standby frequency in kHz
        void setCom2StandbyKhz(int freq) { m_com2Standby.set(freq); }

        //! Set the current transponder code in decimal
        void setTransponderCode(int code) { m_xpdrCode.set(code); }

        //! Set the current transponder mode (depends on the aircraft, 0 and 1 usually mean standby, >1 active)
        void setTransponderMode(int mode) { m_xpdrMode.set(mode); }

        //! Get flaps deploy ratio, where 0.0 is flaps fully retracted, and 1.0 is flaps fully extended.
        double getFlapsDeployRatio() const { return m_flapsReployRatio.get(); }

        //! Get gear deploy ratio, where 0 is up and 1 is down
        double getGearDeployRatio() const { return m_gearReployRatio.getAt(0); }

        //! Get the number of engines of current aircraft
        int getNumberOfEngines() const { return m_numberOfEngines.get(); }

        //! Get the N1 speed as percent of max (per engine)
        std::vector<double> getEngineN1Percentage() const
        {
            std::vector<double> list;
            const int number = getNumberOfEngines();
            list.reserve(static_cast<std::vector<double>::size_type>(number));
            for (int engineNumber = 0; engineNumber < number; ++engineNumber)
            {
                list.push_back(m_enginesN1Percentage.getAt(engineNumber));
            }
            return list;
        }

        //! Get the ratio how much the speedbrakes surfaces are extended (0.0 is fully retracted, and 1.0 is fully
        //! extended)
        double getSpeedBrakeRatio() const { return m_speedBrakeRatio.get(); }

        //! \copydoc XSwiftBus::CMessageBoxControl::toggle
        void toggleMessageBoxVisibility() { m_messages.toggle(); }

        //! Enable/disable message window popping up for new messages
        void setPopupMessageWindow(bool enabled) { m_popupMessageWindow = enabled; }

        //! Enable/disable message window disappearing again after x ms
        void setDisappearMessageWindow(bool enabled) { m_disappearMessageWindow = enabled; }

        //! Enable/disable message window disappearing after x ms
        void setDisappearMessageWindowTimeMs(int durationMs);

        //! Get settings in JSON format
        std::string getSettingsJson() const;

        //! Set settings
        void setSettingsJson(const std::string &jsonString);

        //! Perform generic processing
        int process();

    protected:
        DBusHandlerResult dbusMessageHandler(const CDBusMessage &message) override;

    private:
        void emitAircraftModelChanged(const std::string &path, const std::string &filename, const std::string &livery,
                                      const std::string &icao, const std::string &modelString, const std::string &name,
                                      const std::string &description);

        void emitSceneryLoaded();

        CMessageBoxControl m_messages { 16, 16, 16 };
        bool m_popupMessageWindow = true;
        bool m_disappearMessageWindow = true;
        int m_disapperMessageWindowTimeMs = 5000;
        std::chrono::system_clock::time_point m_disappearMessageWindowTime;
        CTerrainProbe m_terrainProbe;

        //! Redraw message box after reading from the settings
        void updateMessageBoxFromSettings();

        struct FramePeriodSampler;
        std::unique_ptr<FramePeriodSampler> m_framePeriodSampler;

        DataRef<xplane::data::sim::graphics::scenery::async_scenery_load_in_progress> m_sceneryIsLoading;
        int m_sceneryWasLoading = 0;

        CustomDataRef<TSwiftNetworkConnected> m_swiftNetworkConnected;
        CustomDataRef<TSwiftCallsign> m_swiftCallsign;
        StringDataRef<xplane::data::sim::aircraft::view::acf_livery_path> m_liveryPath;
        StringDataRef<xplane::data::sim::aircraft::view::acf_ICAO> m_icao;
        StringDataRef<xplane::data::sim::aircraft::view::acf_descrip> m_descrip;
        DataRef<xplane::data::sim::time::paused> m_paused;
        DataRef<xplane::data::sim::time::use_system_time> m_useSystemTime;
        DataRef<xplane::data::sim::flightmodel::position::latitude> m_latitude;
        DataRef<xplane::data::sim::flightmodel::position::longitude> m_longitude;
        DataRef<xplane::data::sim::flightmodel::position::elevation> m_elevation;
        DataRef<xplane::data::sim::flightmodel2::position::pressure_altitude> m_pressureAlt;
        DataRef<xplane::data::sim::flightmodel::position::y_agl> m_agl;
        DataRef<xplane::data::sim::flightmodel::position::groundspeed> m_groundSpeed;
        DataRef<xplane::data::sim::flightmodel::position::indicated_airspeed2> m_indicatedAirspeed;
        DataRef<xplane::data::sim::flightmodel::position::true_airspeed> m_trueAirspeed;
        DataRef<xplane::data::sim::flightmodel::position::theta> m_pitch;
        DataRef<xplane::data::sim::flightmodel::position::phi> m_roll;
        DataRef<xplane::data::sim::flightmodel::position::psi> m_heading;
        DataRef<xplane::data::sim::flightmodel::position::local_vx> m_velocityX;
        DataRef<xplane::data::sim::flightmodel::position::local_vy> m_velocityY;
        DataRef<xplane::data::sim::flightmodel::position::local_vz> m_velocityZ;
        DataRef<xplane::data::sim::flightmodel::position::Prad> m_rollVelocity;
        DataRef<xplane::data::sim::flightmodel::position::Qrad> m_pitchVelocity;
        DataRef<xplane::data::sim::flightmodel::position::Rrad> m_headingVelocity;
        DataRef<xplane::data::sim::flightmodel::failures::onground_any> m_onGroundAny;
        DataRef<xplane::data::sim::flightmodel::failures::onground_all> m_onGroundAll;
        DataRef<xplane::data::sim::cockpit2::radios::actuators::com1_frequency_hz_833> m_com1Active;
        DataRef<xplane::data::sim::cockpit2::radios::actuators::com1_standby_frequency_hz_833> m_com1Standby;
        DataRef<xplane::data::sim::cockpit2::radios::actuators::audio_com_selection>
            m_comAudioSelection; // 6==COM1, 7==COM2
        DataRef<xplane::data::sim::cockpit2::radios::actuators::com1_power> m_com1Power;
        DataRef<xplane::data::sim::cockpit2::radios::actuators::audio_volume_com1> m_com1Volume; // 0..1
        DataRef<xplane::data::sim::cockpit2::radios::actuators::audio_selection_com1> m_com1Listening;
        DataRef<xplane::data::sim::cockpit2::radios::actuators::com2_frequency_hz_833> m_com2Active;
        DataRef<xplane::data::sim::cockpit2::radios::actuators::com2_standby_frequency_hz_833> m_com2Standby;
        DataRef<xplane::data::sim::cockpit2::radios::actuators::com2_power> m_com2Power;
        DataRef<xplane::data::sim::cockpit2::radios::actuators::audio_volume_com2> m_com2Volume; // 0..1
        DataRef<xplane::data::sim::cockpit2::radios::actuators::audio_selection_com2> m_com2Listening;
        DataRef<xplane::data::sim::cockpit::radios::transponder_code> m_xpdrCode;
        DataRef<xplane::data::sim::cockpit::radios::transponder_mode> m_xpdrMode;
        DataRef<xplane::data::sim::cockpit::radios::transponder_id> m_xpdrIdent;
        DataRef<xplane::data::sim::cockpit::electrical::beacon_lights_on> m_beaconLightsOn;
        DataRef<xplane::data::sim::cockpit::electrical::landing_lights_on> m_landingLightsOn;
        DataRef<xplane::data::sim::cockpit::electrical::nav_lights_on> m_navLightsOn;
        DataRef<xplane::data::sim::cockpit::electrical::strobe_lights_on> m_strobeLightsOn;
        DataRef<xplane::data::sim::cockpit::electrical::taxi_light_on> m_taxiLightsOn;
        DataRef<xplane::data::sim::flightmodel2::controls::flap_handle_deploy_ratio> m_flapsReployRatio;
        ArrayDataRef<xplane::data::sim::flightmodel2::gear::deploy_ratio> m_gearReployRatio;
        DataRef<xplane::data::sim::aircraft::engine::acf_num_engines> m_numberOfEngines;
        ArrayDataRef<xplane::data::sim::flightmodel::engine::ENGN_N1_> m_enginesN1Percentage;
        DataRef<xplane::data::sim::flightmodel2::controls::speedbrake_ratio> m_speedBrakeRatio;
        DataRef<xplane::data::sim::weather::barometer_sealevel_inhg> m_qnhInhg;
    };
} // namespace XSwiftBus

#endif // SWIFT_SIM_XSWIFTBUS_SERVICE_H
