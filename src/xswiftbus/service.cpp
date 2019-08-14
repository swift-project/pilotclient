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

#include <algorithm>

// clazy:excludeall=reserve-candidates

using namespace BlackMisc::Simulation::XPlane::QtFreeUtils;

namespace XSwiftBus
{
    CService::CService(CSettingsProvider *settingsProvider) : CDBusObject(settingsProvider)
    {
        updateAirportsInRange();
    }

    void CService::onAircraftModelChanged()
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        AcfProperties acfProperties = extractAcfProperties(path);
        emitAircraftModelChanged(path, filename, getAircraftLivery(), getAircraftIcaoCode(), acfProperties.modelString, acfProperties.modelName, getAircraftDescription());
    }

    std::string CService::getVersionNumber() const
    {
        std::string version(XSWIFTBUS_VERSION);
        const std::string lastCommitTs(GIT_COMMIT_TS);

        const long long lctsll = std::stoll(lastCommitTs); // at least 64bit
        // now we have to converto int
        // max 2147483647 (2^31 - 1)
        //      1MMddHHmm (years since 2010)
        const long long yearOffset = 201000000000;
        const int lctsInt = static_cast<int>(lctsll - yearOffset);
        version = version + "." + std::to_string(lctsInt);
        return version;
    }

    void CService::addTextMessage(const std::string &text, double red, double green, double blue)
    {
        if (text.empty()) { return; }
        static const std::string ellipsis = u8"\u2026";
        const int lineLength = m_messages.maxLineLength() - static_cast<int>(ellipsis.size());
        std::vector<std::string> wrappedLines;
        for (size_t i = 0; i < text.size(); i += static_cast<size_t>(lineLength))
        {
            wrappedLines.push_back(text.substr(i, static_cast<size_t>(lineLength)) + ellipsis);
        }
        wrappedLines.back().erase(wrappedLines.back().size() - 3);
        if (wrappedLines.back().empty()) { wrappedLines.pop_back(); }
        else if (wrappedLines.back().size() == ellipsis.size() && wrappedLines.size() > 1)
        {
            auto secondLastLine = wrappedLines.end() - 2;
            secondLastLine->erase(wrappedLines.back().size() - 3);
            secondLastLine->append(wrappedLines.back());
            wrappedLines.pop_back();
        }
        for (const auto &line : wrappedLines)
        {
            m_messages.addMessage({ line, static_cast<float>(red), static_cast<float>(green), static_cast<float>(blue) });
        }

        if (!m_messages.isVisible() && m_popupMessageWindow) { m_messages.toggle(); }

        if (m_disappearMessageWindow)
        {
            m_disappearMessageWindowTime = std::chrono::system_clock::now() + std::chrono::seconds(5);
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

    std::string CService::getSettingsJson() const
    {
        return this->getSettings().toXSwiftBusJsonString();
    }

    void CService::setSettingsJson(const std::string &jsonString)
    {
        CSettings s;
        s.parseXSwiftBusString(jsonString);
        this->setSettings(s);
        INFO_LOG("Received settings " + s.convertToString());
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
                    double lat = m_latitude.get();
                    double lon = m_longitude.get();
                    double alt = m_elevation.get();
                    double gs = m_groundSpeed.get();
                    double pitch = m_pitch.get();
                    double roll = m_roll.get();
                    double trueHeading = m_heading.get();
                    double qnh = m_qnhInhg.get();
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
            else if (message.getMethodName() == "getTaxiLightsOn")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getTaxiLightsOn());
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
                    std::vector<double> array = getEngineN1Percentage();
                    sendDBusReply(sender, serial, array);
                });
            }
            else if (message.getMethodName() == "getSpeedBrakeRatio")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getSpeedBrakeRatio());
                });
            }
            else if (message.getMethodName() == "toggleMessageBoxVisibility")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                queueDBusCall([ = ]()
                {
                    toggleMessageBoxVisibility();
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
}
