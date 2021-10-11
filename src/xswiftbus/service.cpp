/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "plugin.h"
#include "service.h"
#include "utils.h"
#include "blackmisc/simulation/xplane/qtfreeutils.h"

#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMUtilities.h>

#include <cmath>
#include <cstring>
#include <algorithm>

// clazy:excludeall=reserve-candidates

using namespace BlackMisc::Simulation::XPlane::QtFreeUtils;

namespace XSwiftBus
{
    //! \private
    struct CService::FramePeriodSampler : public CDrawable
    {
        DataRef<xplane::data::sim::operation::misc::frame_rate_period> m_thisFramePeriod;
        DataRef<xplane::data::sim::time::framerate_period> m_thisFramePeriodXP11;
        DataRef<xplane::data::sim::time::total_flight_time_sec> m_secondsSinceReset;
        DataRef<xplane::data::sim::flightmodel::position::groundspeed> m_groundSpeed;

        std::vector<float> m_samples;
        float m_total = 0;
        float m_totalOverBudget  = 0;
        float m_totalMetersShort = 0;
        float m_totalSecondsLate = 0;
        size_t m_lastSampleIndex = 0;
        static constexpr size_t c_maxSampleCount   = 500;
        static constexpr float c_framePeriodBudget = 0.05f;

        FramePeriodSampler() : CDrawable(xplm_Phase_Window, true) {}

        std::tuple<float, float, float, float> getFrameStats()
        {
            if (m_total < 0.001f) { return {}; } // no DIV by 0
            const float fps = m_samples.size() / m_total;
            const float ratio = 1 - m_totalOverBudget / m_total;
            const float miles = m_totalMetersShort / 1852.0f;
            const float minutes = m_totalSecondsLate / 60.0f;
            m_total = 0;
            m_totalOverBudget = 0;
            m_samples.clear();
            m_lastSampleIndex = 0;
            return std::make_tuple(fps, ratio, miles, minutes);
        }

    protected:
        virtual void draw() override // called once per frame
        {
            const float current = m_thisFramePeriodXP11.isValid() ? m_thisFramePeriodXP11.get() : m_thisFramePeriod.get();

            ++m_lastSampleIndex %= c_maxSampleCount;
            if (m_samples.size() == c_maxSampleCount)
            {
                auto &oldSample = m_samples[m_lastSampleIndex];
                m_total -= oldSample;
                if (oldSample > c_framePeriodBudget)
                {
                    m_totalOverBudget -= oldSample - c_framePeriodBudget;
                }
                oldSample = current;
            }
            else { m_samples.push_back(current); }

            m_total += current;
            if (current > c_framePeriodBudget)
            {
                m_totalOverBudget += current - c_framePeriodBudget;

                if (m_secondsSinceReset.get() > 10)
                {
                    const float metersShort = m_groundSpeed.get() * std::max(0.0f, current - c_framePeriodBudget);
                    m_totalMetersShort += metersShort;
                    if (m_groundSpeed.get() > 1.0f) { m_totalSecondsLate += std::max(0.0f, current - c_framePeriodBudget); }
                }
            }
        }
    };

    CService::CService(CSettingsProvider *settingsProvider) :
        CDBusObject(settingsProvider),
        m_framePeriodSampler(std::make_unique<FramePeriodSampler>())
    {
        this->updateAirportsInRange();
        this->updateMessageBoxFromSettings();
        m_framePeriodSampler->show();
    }

    CService::~CService() = default;

    void CService::onAircraftModelChanged()
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        if (std::strlen(filename) < 1 || std::strlen(path) < 1)
        {
            WARNING_LOG("Aircraft changed, but NO path or file name");
            return;
        }
        const AcfProperties acfProperties = extractAcfProperties(path);
        emitAircraftModelChanged(path, filename, getAircraftLivery(), getAircraftIcaoCode(), acfProperties.modelString, acfProperties.modelName, getAircraftDescription());
    }

    void CService::onSceneryLoaded()
    {
        emitSceneryLoaded();
    }

    std::string CService::getVersionNumber() const
    {
        return XSWIFTBUS_VERSION;
    }

    std::string CService::getCommitHash() const
    {
        return XSWIFTBUS_COMMIT;
    }

    std::tuple<double, double, double, double> CService::getFrameStats()
    {
        if (!m_framePeriodSampler) { return {}; }
        const auto result = m_framePeriodSampler->getFrameStats();
        return std::make_tuple(static_cast<double>(std::get<0>(result)), static_cast<double>(std::get<1>(result)), static_cast<double>(std::get<2>(result)), static_cast<double>(std::get<3>(result)));
    }

    void CService::resetFrameTotals()
    {
        if (m_framePeriodSampler)
        {
            m_framePeriodSampler->m_totalMetersShort = 0;
            m_framePeriodSampler->m_totalSecondsLate = 0;
        }
    }

    void CService::addTextMessage(const std::string &text, double red, double green, double blue)
    {
        if (text.empty()) { return; }
        static const CMessage::string ellipsis = u8"\u2026";
        const unsigned lineLength = m_messages.maxLineLength() - 1;

        using U8It = Utf8Iterator<std::string::const_iterator>;
        U8It begin(text.begin(), text.end());
        auto characters = std::distance(begin, U8It(text.end(), text.end()));
        std::vector<CMessage::string> wrappedLines;

        for (; characters > lineLength; characters -= lineLength)
        {
            auto end = std::next(begin, lineLength);
            wrappedLines.emplace_back(begin.base, end.base);
            wrappedLines.back() += ellipsis;
            begin = end;
        }
        if (characters > 0)
        {
            wrappedLines.emplace_back(begin.base, text.end());
        }
        for (const auto &line : wrappedLines)
        {
            m_messages.addMessage({ line, static_cast<float>(red), static_cast<float>(green), static_cast<float>(blue) });
        }

        if (!m_messages.isVisible() && m_popupMessageWindow) { m_messages.toggle(); }

        if (m_disappearMessageWindow)
        {
            m_disappearMessageWindowTime = std::chrono::system_clock::now() + std::chrono::milliseconds(std::max(m_disapperMessageWindowTimeMs, 1500));
        }
    }

    std::string CService::getAircraftModelPath() const
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        return path;
    }

    std::string CService::getAircraftModelFilename() const
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        return filename;
    }

    std::string CService::getAircraftModelString() const
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        const AcfProperties acfProperties = extractAcfProperties(path);
        return acfProperties.modelString;
    }

    std::string CService::getAircraftName() const
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        const AcfProperties acfProperties = extractAcfProperties(path);
        return acfProperties.modelName;
    }

    std::string CService::getAircraftLivery() const
    {
        std::string liveryPath = m_liveryPath.get();
        if (liveryPath.empty()) { return {}; }

        // liveryPath end with / and we need to get rid of it
        liveryPath.pop_back();
        return getFileName(liveryPath);
    }

    int CService::getXPlaneVersionMajor() const
    {
        int version;
        XPLMGetVersions(&version, nullptr, nullptr);
        if (version > 5000) { version /= 10; }
        return version / 100;
    }

    int CService::getXPlaneVersionMinor() const
    {
        int version;
        XPLMGetVersions(&version, nullptr, nullptr);
        if (version > 5000) { version /= 10; }
        return version % 100;
    }

    std::string CService::getXPlaneInstallationPath() const
    {
        char path[512];
        XPLMGetSystemPath(path);
        return path;
    }

    std::string CService::getXPlanePreferencesPath() const
    {
        char path[512];
        XPLMGetPrefsPath(path);
        return path;
    }

    void CService::setDisappearMessageWindowTimeMs(int durationMs)
    {
        m_disapperMessageWindowTimeMs = durationMs;
    }

    std::string CService::getSettingsJson() const
    {
        return this->getSettings().toXSwiftBusJsonString();
    }

    void CService::setSettingsJson(const std::string &jsonString)
    {
        CSettings s;
        s.parseXSwiftBusString(jsonString);
        this->setSettings(s);
        const bool w = this->writeConfig(s.isTcasEnabled(), s.isLogRenderPhases());
        this->updateMessageBoxFromSettings();
        INFO_LOG("Received settings " + s.convertToString());
        if (w) { INFO_LOG("Written new config file"); }
    }

    void CService::readAirportsDatabase()
    {
        auto first = XPLMFindFirstNavAidOfType(xplm_Nav_Airport);
        auto last  = XPLMFindLastNavAidOfType(xplm_Nav_Airport);
        if (first != XPLM_NAV_NOT_FOUND)
        {
            for (auto i = first; i <= last; ++i)
            {
                float lat, lon;
                char icao[32];
                XPLMGetNavAidInfo(i, nullptr, &lat, &lon, nullptr, nullptr, nullptr, icao, nullptr, nullptr);
                if (icao[0] != 0)
                {
                    m_airports.emplace_back(i, lat, lon);
                }
            }
        }
    }

    void CService::updateAirportsInRange()
    {
        if (m_airports.empty())
        {
            readAirportsDatabase();
        }
        std::vector<std::string> icaos, names;
        std::vector<double> lats, lons, alts;
        std::vector<CNavDataReference> closestAirports = findClosestAirports(20, getLatitudeDeg(), getLongitudeDeg());
        for (const auto &navref : closestAirports)
        {
            float lat, lon, alt;
            char icao[32], name[256];
            XPLMGetNavAidInfo(navref.id(), nullptr, &lat, &lon, &alt, nullptr, nullptr, icao, name, nullptr);
            icaos.emplace_back(icao);
            names.emplace_back(name);
            lats.push_back(lat);
            lons.push_back(lon);
            alts.push_back(alt);
        }
        emitAirportsInRangeUpdated(icaos, names, lats, lons, alts);
    }

    static const char *introspection_service =
        DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE
#include "org.swift_project.xswiftbus.service.xml"
        ;

    DBusHandlerResult CService::dbusMessageHandler(const CDBusMessage &message_)
    {
        CDBusMessage message(message_);
        const std::string sender = message.getSender();
        const dbus_uint32_t serial = message.getSerial();
        const bool wantsReply = message.wantsReply();

        if (message.getInterfaceName() == DBUS_INTERFACE_INTROSPECTABLE)
        {
            if (message.getMethodName() == "Introspect")
            {
                sendDBusReply(sender, serial, introspection_service);
            }
        }
        else if (message.getInterfaceName() == XSWIFTBUS_SERVICE_INTERFACENAME)
        {
            if (message.getMethodName() == "getVersionNumber")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getVersionNumber());
                });
            }
            else if (message.getMethodName() == "getCommitHash")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCommitHash());
                });
            }
            else if (message.getMethodName() == "addTextMessage")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::string text;
                double red = 0;
                double green = 0;
                double blue = 0;
                message.beginArgumentRead();
                message.getArgument(text);
                message.getArgument(red);
                message.getArgument(green);
                message.getArgument(blue);

                queueDBusCall([ = ]()
                {
                    addTextMessage(text, red, green, blue);
                });
            }
            else if (message.getMethodName() == "getOwnAircraftSituationData")
            {
                queueDBusCall([ = ]()
                {
                    const double lat = m_latitude.get();
                    const double lon = m_longitude.get();
                    const double alt = m_elevation.get();
                    const double gs = m_groundSpeed.get();
                    const double pitch = m_pitch.get();
                    const double roll = m_roll.get();
                    const double trueHeading = m_heading.get();
                    const double qnh = m_qnhInhg.get();
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(lat);
                    reply.appendArgument(lon);
                    reply.appendArgument(alt);
                    reply.appendArgument(gs);
                    reply.appendArgument(pitch);
                    reply.appendArgument(roll);
                    reply.appendArgument(trueHeading);
                    reply.appendArgument(qnh);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "getOwnAircraftVelocityData")
            {
                queueDBusCall([ = ]()
                {
                    const double velocityX = m_velocityX.get();
                    const double velocityY = m_velocityY.get();
                    const double velocityZ = m_velocityZ.get();
                    const double pitchVelocity = m_pitchVelocity.get();
                    const double rollVelocity = m_rollVelocity.get();
                    const double headingVelocity = m_headingVelocity.get();
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(velocityX);
                    reply.appendArgument(velocityY);
                    reply.appendArgument(velocityZ);
                    reply.appendArgument(pitchVelocity);
                    reply.appendArgument(rollVelocity);
                    reply.appendArgument(headingVelocity);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "getOwnAircraftCom1Data")
            {
                queueDBusCall([ = ]()
                {
                    const int active  = m_com1Active.get();
                    const int standby = m_com1Standby.get();
                    const double volume = m_com1Volume.get();
                    const bool rec = this->isCom1Receiving();
                    const bool tx  = this->isCom1Transmitting();
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(active);
                    reply.appendArgument(standby);
                    reply.appendArgument(volume);
                    reply.appendArgument(rec);
                    reply.appendArgument(tx);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "getOwnAircraftCom2Data")
            {
                queueDBusCall([ = ]()
                {
                    const int active  = m_com2Active.get();
                    const int standby = m_com2Standby.get();
                    const double volume = m_com2Volume.get();
                    const bool rec = this->isCom2Receiving();
                    const bool tx  = this->isCom2Transmitting();
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(active);
                    reply.appendArgument(standby);
                    reply.appendArgument(volume);
                    reply.appendArgument(rec);
                    reply.appendArgument(tx);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "getOwnAircraftXpdr")
            {
                queueDBusCall([ = ]()
                {
                    const int code = m_xpdrCode.get();
                    const int mode = m_xpdrMode.get();
                    const bool id  = m_xpdrIdent.get();
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(code);
                    reply.appendArgument(mode);
                    reply.appendArgument(id);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "getOwnAircraftLights")
            {
                queueDBusCall([ = ]()
                {
                    const bool beaconLightsOn  = m_beaconLightsOn.get();
                    const bool landingLightsOn = m_landingLightsOn.get();
                    const bool navLightsOn     = m_navLightsOn.get();
                    const bool strobeLightsOn  = m_strobeLightsOn.get();
                    const bool taxiLightsOn    = m_taxiLightsOn.get();
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(beaconLightsOn);
                    reply.appendArgument(landingLightsOn);
                    reply.appendArgument(navLightsOn);
                    reply.appendArgument(strobeLightsOn);
                    reply.appendArgument(taxiLightsOn);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "getOwnAircraftParts")
            {
                queueDBusCall([ = ]()
                {
                    const double flapsReployRatio = m_flapsReployRatio.get();
                    const double gearReployRatio  = m_gearReployRatio.getAt(0);
                    const double speedBrakeRatio  = m_speedBrakeRatio.get();
                    const std::vector<double> enginesN1Percentage = this->getEngineN1Percentage();
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(flapsReployRatio);
                    reply.appendArgument(gearReployRatio);
                    reply.appendArgument(speedBrakeRatio);
                    reply.appendArgument(enginesN1Percentage);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "getOwnAircraftModelData")
            {
                queueDBusCall([ = ]()
                {
                    const std::string aircraftModelPath = this->getAircraftModelPath();
                    const std::string aircraftIcaoCode  = this->getAircraftIcaoCode();
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(aircraftModelPath);
                    reply.appendArgument(aircraftIcaoCode);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "updateAirportsInRange")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                queueDBusCall([ = ]()
                {
                    updateAirportsInRange();
                });
            }
            else if (message.getMethodName() == "getAircraftModelPath")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAircraftModelPath());
                });
            }
            else if (message.getMethodName() == "getAircraftModelFilename")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAircraftModelFilename());
                });
            }
            else if (message.getMethodName() == "getAircraftModelString")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAircraftModelString());
                });
            }
            else if (message.getMethodName() == "getAircraftName")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAircraftName());
                });
            }
            else if (message.getMethodName() == "getAircraftLivery")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAircraftLivery());
                });
            }
            else if (message.getMethodName() == "getAircraftIcaoCode")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAircraftIcaoCode());
                });
            }
            else if (message.getMethodName() == "getAircraftDescription")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAircraftDescription());
                });
            }
            else if (message.getMethodName() == "getXPlaneVersionMajor")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getXPlaneVersionMajor());
                });
            }
            else if (message.getMethodName() == "getXPlaneVersionMinor")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getXPlaneVersionMinor());
                });
            }
            else if (message.getMethodName() == "getXPlaneInstallationPath")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getXPlaneInstallationPath());
                });
            }
            else if (message.getMethodName() == "getXPlanePreferencesPath")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getXPlanePreferencesPath());
                });
            }
            else if (message.getMethodName() == "isPaused")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, isPaused());
                });
            }
            else if (message.getMethodName() == "isUsingRealTime")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, isUsingRealTime());
                });
            }
            else if (message.getMethodName() == "getFrameStats")
            {
                queueDBusCall([ = ]()
                {
                    const auto stats = getFrameStats();
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(std::get<0>(stats));
                    reply.appendArgument(std::get<1>(stats));
                    reply.appendArgument(std::get<2>(stats));
                    reply.appendArgument(std::get<3>(stats));
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "resetFrameTotals")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                queueDBusCall([=]()
                {
                    resetFrameTotals();
                });
            }
            else if (message.getMethodName() == "getLatitudeDeg")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getLatitudeDeg());
                });
            }
            else if (message.getMethodName() == "getLongitudeDeg")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getLongitudeDeg());
                });
            }
            else if (message.getMethodName() == "getAltitudeMslM")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAltitudeMslM());
                });
            }
            else if (message.getMethodName() == "getHeightAglM")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getHeightAglM());
                });
            }
            else if (message.getMethodName() == "getGroundSpeedMps")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getGroundSpeedMps());
                });
            }
            else if (message.getMethodName() == "getIndicatedAirspeedKias")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getIndicatedAirspeedKias());
                });
            }
            else if (message.getMethodName() == "getTrueAirspeedKias")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getTrueAirspeedKias());
                });
            }
            else if (message.getMethodName() == "getPitchDeg")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getPitchDeg());
                });
            }
            else if (message.getMethodName() == "getRollDeg")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getRollDeg());
                });
            }
            else if (message.getMethodName() == "getTrueHeadingDeg")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getTrueHeadingDeg());
                });
            }
            else if (message.getMethodName() == "getLocalXVelocityXMps")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getLocalXVelocityMps());
                });
            }
            else if (message.getMethodName() == "getLocalYVelocityYMps")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getLocalYVelocityMps());
                });
            }
            else if (message.getMethodName() == "getLocalZVelocityZMps")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getLocalZVelocityMps());
                });
            }
            else if (message.getMethodName() == "getPitchRadPerSec")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getPitchRadPerSec());
                });
            }
            else if (message.getMethodName() == "getRollRadPerSec")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getRollRadPerSec());
                });
            }
            else if (message.getMethodName() == "getHeadingRadPerSec")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getHeadingRadPerSec());
                });
            }
            else if (message.getMethodName() == "getAnyWheelOnGround")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAnyWheelOnGround());
                });
            }
            else if (message.getMethodName() == "getAllWheelsOnGround")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAllWheelsOnGround());
                });
            }
            else if (message.getMethodName() == "getCom1ActiveKhz")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCom1ActiveKhz());
                });
            }
            else if (message.getMethodName() == "getCom1StandbyKhz")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCom1StandbyKhz());
                });
            }
            else if (message.getMethodName() == "getCom2ActiveKhz")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCom2ActiveKhz());
                });
            }
            else if (message.getMethodName() == "getCom2StandbyKhz")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCom2StandbyKhz());
                });
            }
            else if (message.getMethodName() == "isCom1Receiving")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, isCom1Receiving());
                });
            }
            else if (message.getMethodName() == "isCom1Transmitting")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, isCom1Transmitting());
                });
            }
            else if (message.getMethodName() == "getCom1Volume")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCom1Volume());
                });
            }
            else if (message.getMethodName() == "isCom2Receiving")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, isCom2Receiving());
                });
            }
            else if (message.getMethodName() == "isCom2Transmitting")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, isCom2Transmitting());
                });
            }
            else if (message.getMethodName() == "getCom2Volume")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCom2Volume());
                });
            }
            else if (message.getMethodName() == "getTransponderCode")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getTransponderCode());
                });
            }
            else if (message.getMethodName() == "getTransponderMode")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getTransponderMode());
                });
            }
            else if (message.getMethodName() == "getTransponderIdent")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getTransponderIdent());
                });
            }
            else if (message.getMethodName() == "getBeaconLightsOn")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getBeaconLightsOn());
                });
            }
            else if (message.getMethodName() == "getLandingLightsOn")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getLandingLightsOn());
                });
            }
            else if (message.getMethodName() == "getTaxiLightsOn")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getTaxiLightsOn());
                });
            }
            else if (message.getMethodName() == "getNavLightsOn")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getNavLightsOn());
                });
            }
            else if (message.getMethodName() == "getStrobeLightsOn")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getStrobeLightsOn());
                });
            }
            else if (message.getMethodName() == "getQNHInHg")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getQNHInHg());
                });
            }
            else if (message.getMethodName() == "setCom1ActiveKhz")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int frequency = 0;
                message.beginArgumentRead();
                message.getArgument(frequency);
                queueDBusCall([ = ]()
                {
                    setCom1ActiveKhz(frequency);
                });
            }
            else if (message.getMethodName() == "setCom1StandbyKhz")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int frequency = 0;
                message.beginArgumentRead();
                message.getArgument(frequency);
                queueDBusCall([ = ]()
                {
                    setCom1StandbyKhz(frequency);
                });
            }
            else if (message.getMethodName() == "setCom2ActiveKhz")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int frequency = 0;
                message.beginArgumentRead();
                message.getArgument(frequency);
                queueDBusCall([ = ]()
                {
                    setCom2ActiveKhz(frequency);
                });
            }
            else if (message.getMethodName() == "setCom2StandbyKhz")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int frequency = 0;
                message.beginArgumentRead();
                message.getArgument(frequency);
                queueDBusCall([ = ]()
                {
                    setCom2StandbyKhz(frequency);
                });
            }
            else if (message.getMethodName() == "setTransponderCode")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int code = 0;
                message.beginArgumentRead();
                message.getArgument(code);
                queueDBusCall([ = ]()
                {
                    setTransponderCode(code);
                });
            }
            else if (message.getMethodName() == "setTransponderMode")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int mode = 0;
                message.beginArgumentRead();
                message.getArgument(mode);
                queueDBusCall([ = ]()
                {
                    setTransponderMode(mode);
                });
            }
            else if (message.getMethodName() == "getFlapsDeployRatio")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getFlapsDeployRatio());
                });
            }
            else if (message.getMethodName() == "getGearDeployRatio")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getGearDeployRatio());
                });
            }
            else if (message.getMethodName() == "getNumberOfEngines")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getNumberOfEngines());
                });
            }
            else if (message.getMethodName() == "getEngineN1Percentage")
            {
                queueDBusCall([ = ]()
                {
                    const std::vector<double> enginesN1Percentage = getEngineN1Percentage();
                    sendDBusReply(sender, serial, enginesN1Percentage);
                });
            }
            else if (message.getMethodName() == "getSpeedBrakeRatio")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getSpeedBrakeRatio());
                });
            }
            else if (message.getMethodName() == "getSettingsJson")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getSettingsJson());
                });
            }
            else if (message.getMethodName() == "setSettingsJson")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::string json;
                message.beginArgumentRead();
                message.getArgument(json);
                queueDBusCall([ = ]()
                {
                    setSettingsJson(json);
                });
            }
            else
            {
                // Unknown message. Tell DBus that we cannot handle it
                return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
            }
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    int CService::process()
    {
        if (m_sceneryIsLoading.get() != m_sceneryWasLoading)
        {
            if (!m_sceneryIsLoading.get()) { onSceneryLoaded(); }
            m_sceneryWasLoading = m_sceneryIsLoading.get();
        }

        invokeQueuedDBusCalls();

        if (m_disappearMessageWindowTime != std::chrono::system_clock::time_point()
                && std::chrono::system_clock::now() > m_disappearMessageWindowTime
                && m_messages.isVisible())
        {
            m_messages.toggle();
            m_disappearMessageWindowTime = std::chrono::system_clock::time_point();
        }

        return 1;
    }

    void CService::emitAircraftModelChanged(const std::string &path, const std::string &filename, const std::string &livery,
                                            const std::string &icao, const std::string &modelString, const std::string &name,
                                            const std::string &description)
    {
        CDBusMessage signalAircraftModelChanged = CDBusMessage::createSignal(XSWIFTBUS_SERVICE_OBJECTPATH, XSWIFTBUS_SERVICE_INTERFACENAME, "aircraftModelChanged");
        signalAircraftModelChanged.beginArgumentWrite();
        signalAircraftModelChanged.appendArgument(path);
        signalAircraftModelChanged.appendArgument(filename);
        signalAircraftModelChanged.appendArgument(livery);
        signalAircraftModelChanged.appendArgument(icao);
        signalAircraftModelChanged.appendArgument(modelString);
        signalAircraftModelChanged.appendArgument(name);
        signalAircraftModelChanged.appendArgument(description);
        sendDBusMessage(signalAircraftModelChanged);
    }

    void CService::emitAirportsInRangeUpdated(const std::vector<std::string> &icaoCodes, const std::vector<std::string> &names,
            const std::vector<double> &lats, const std::vector<double> &lons, const std::vector<double> &alts)
    {
        CDBusMessage signalAirportsInRangeUpdated = CDBusMessage::createSignal(XSWIFTBUS_SERVICE_OBJECTPATH, XSWIFTBUS_SERVICE_INTERFACENAME, "airportsInRangeUpdated");
        signalAirportsInRangeUpdated.beginArgumentWrite();
        signalAirportsInRangeUpdated.appendArgument(icaoCodes);
        signalAirportsInRangeUpdated.appendArgument(names);
        signalAirportsInRangeUpdated.appendArgument(lats);
        signalAirportsInRangeUpdated.appendArgument(lons);
        signalAirportsInRangeUpdated.appendArgument(alts);
        sendDBusMessage(signalAirportsInRangeUpdated);
    }

    void CService::emitSceneryLoaded()
    {
        CDBusMessage signal = CDBusMessage::createSignal(XSWIFTBUS_SERVICE_OBJECTPATH, XSWIFTBUS_SERVICE_INTERFACENAME, "sceneryLoaded");
        sendDBusMessage(signal);
    }

    std::vector<CNavDataReference> CService::findClosestAirports(int number, double latitude, double longitude)
    {
        CNavDataReference ref(0, latitude, longitude);
        auto compareFunction = [ & ](const CNavDataReference & a, const CNavDataReference & b)
        {
            return calculateGreatCircleDistance(a, ref) < calculateGreatCircleDistance(b, ref);
        };

        number = std::min(static_cast<int>(m_airports.size()), number);

        std::vector<CNavDataReference> closestAirports = m_airports;
        std::partial_sort(closestAirports.begin(), closestAirports.begin() + number, closestAirports.end(), compareFunction);
        closestAirports.resize(static_cast<std::vector<CNavDataReference>::size_type>(number));
        return closestAirports;
    }

    void CService::updateMessageBoxFromSettings()
    {
        // left, top, right, bottom, height size percentage
        const std::vector<int> values = this->getSettings().getMessageBoxValuesVector();
        if (values.size() >= 6)
        {
            m_messages.setValues(values[0], values[1], values[2], values[3], values[4], values[5]);
            this->setDisappearMessageWindowTimeMs(values[5]);
        }
    }
}
