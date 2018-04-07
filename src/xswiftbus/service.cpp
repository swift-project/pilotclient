/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "service.h"
#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMUtilities.h>
#include <fstream>
#include "utils.h"

// clazy:excludeall=reserve-candidates

namespace XSwiftBus
{

    CService::CService(CDBusConnection *connection) : CDBusObject(connection)
    {
        registerDBusObjectPath(XSWIFTBUS_SERVICE_INTERFACENAME, XSWIFTBUS_SERVICE_OBJECTPATH);
        m_messages.addMessage({ "xswiftbus started.", 0, 255, 255 });
        updateAirportsInRange();
    }

    void CService::onAircraftModelChanged()
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        const auto model = extractAcfProperties(path);
        emitAircraftModelChanged(path, filename, getAircraftLivery(), getAircraftIcaoCode(), model.getModelString(), model.getName(), getAircraftDescription());
    }

    void CService::addTextMessage(const std::string &text, double red, double green, double blue)
    {
        if (text.empty()) { return; }
        static const std::string ellipsis = "...";
        const int lineLength = static_cast<int>(m_messages.maxLineLength() - ellipsis.size());
        std::vector<std::string> wrappedLines;
        for (size_t i = 0; i < text.size(); i += lineLength)
        {
//            static const QChar ellipsis = 0x2026;
//            wrappedLines.push_back(QString::fromStdString(text).mid(i, lineLength) + ellipsis);
            wrappedLines.push_back(text.substr(i, lineLength) + ellipsis);
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
        const auto model = extractAcfProperties(path);
        return model.getModelString();
    }

    std::string CService::getAircraftName() const
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        const auto model = extractAcfProperties(path);
        return model.getName();
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

    void CService::readAirportsDatabase()
    {
        auto first = XPLMFindFirstNavAidOfType(xplm_Nav_Airport);
        auto last = XPLMFindLastNavAidOfType(xplm_Nav_Airport);
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
        std::vector<CNavDataReference> closestAirports = findClosestAirports(20, getLatitude(), getLongitude());
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
            if (message.getMethodName() == "addTextMessage")
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
            else if (message.getMethodName() == "getLatitude")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getLatitude());
                });
            }
            else if (message.getMethodName() == "getLongitude")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getLongitude());
                });
            }
            else if (message.getMethodName() == "getAltitudeMSL")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getAltitudeMSL());
                });
            }
            else if (message.getMethodName() == "getHeightAGL")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getHeightAGL());
                });
            }
            else if (message.getMethodName() == "getGroundSpeed")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getGroundSpeed());
                });
            }
            else if (message.getMethodName() == "getIndicatedAirspeed")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getIndicatedAirspeed());
                });
            }
            else if (message.getMethodName() == "getTrueAirspeed")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getTrueAirspeed());
                });
            }
            else if (message.getMethodName() == "getPitch")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getPitch());
                });
            }
            else if (message.getMethodName() == "getRoll")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getRoll());
                });
            }
            else if (message.getMethodName() == "getTrueHeading")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getTrueHeading());
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
            else if (message.getMethodName() == "getCom1Active")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCom1Active());
                });
            }
            else if (message.getMethodName() == "getCom1Standby")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCom1Standby());
                });
            }
            else if (message.getMethodName() == "getCom2Active")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCom2Active());
                });
            }
            else if (message.getMethodName() == "getCom2Standby")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getCom2Standby());
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
            else if (message.getMethodName() == "getQNH")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, getQNH());
                });
            }
            else if (message.getMethodName() == "setCom1Active")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int frequency = 0;
                message.beginArgumentRead();
                message.getArgument(frequency);
                queueDBusCall([ = ]()
                {
                    setCom1Active(frequency);
                });
            }
            else if (message.getMethodName() == "setCom1Standby")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int frequency = 0;
                message.beginArgumentRead();
                message.getArgument(frequency);
                queueDBusCall([ = ]()
                {
                    setCom1Standby(frequency);
                });
            }
            else if (message.getMethodName() == "setCom2Active")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int frequency = 0;
                message.beginArgumentRead();
                message.getArgument(frequency);
                queueDBusCall([ = ]()
                {
                    setCom2Active(frequency);
                });
            }
            else if (message.getMethodName() == "setCom2Standby")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int frequency = 0;
                message.beginArgumentRead();
                message.getArgument(frequency);
                queueDBusCall([ = ]()
                {
                    setCom2Standby(frequency);
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
            else
            {
                // Unknown message. Tell DBus that we cannot handle it
                return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
            }
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    int CService::processDBus()
    {
        invokeQueuedDBusCalls();
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

        auto closestAirports = m_airports;
        std::partial_sort(closestAirports.begin(), closestAirports.begin() + number, closestAirports.end(), compareFunction);
        closestAirports.resize(number);
        return closestAirports;
    }

    //! Qt free version of BlackMisc::Simulation::XPlane::descriptionForFlyableModel()
    std::string descriptionForFlyableModel(const CAircraftModel &model)
    {
        if (!model.getName().empty())
        {
            if (model.getDistributor().hasDescription() && model.getName().find(model.getDistributor().getDescription()) == std::string::npos)
            {
                return std::string("[ACF] ") + model.getName() + " by " + model.getDistributor().getDescription();
            }
            else
            {
                return std::string("[ACF] ") + model.getName();
            }
        }
        else if (model.hasAircraftDesignator())
        {
            if (model.getDistributor().hasDescription())
            {
                return std::string("[ACF] ") + model.getAircraftIcaoCodeDesignator() + " by " + model.getDistributor().getDescription();
            }
            else
            {
                return std::string("[ACF] ") + model.getAircraftIcaoCodeDesignator();
            }
        }
        return std::string("[ACF]");
    }

    //! Qt free version of BlackMisc::Simulation::XPlane::stringForFlyableModel()
    std::string stringForFlyableModel(const CAircraftModel &model, const std::string &acfFile)
    {
        if (model.getDistributor().hasDescription())
        {
            if (!model.getName().empty())
            {
                if (model.getName().find(model.getDistributor().getDescription()) != std::string::npos)
                {
                    return model.getName();
                }
                else
                {
                    return model.getDistributor().getDescription() + ' ' + model.getName();
                }
            }
            else if (model.hasAircraftDesignator())
            {
                return model.getDistributor().getDescription() + ' ' + model.getAircraftIcaoCodeDesignator();
            }
        }
        return getDirName(acfFile) + ' ' + getBaseName(acfFile);
    }

    CAircraftModel CService::extractAcfProperties(const std::string &filePath)
    {
        CAircraftModel model;

        std::ifstream fs(filePath, std::ios::in | std::ios::binary);
        if (!fs.is_open()) { return model; }

        std::string i;
        std::string version;
        std::string acf;
        std::getline(fs, i);
        std::getline(fs, version);
        std::getline(fs, acf);

        if (i == "I" && version.find("version") != std::string::npos && acf == "ACF")
        {
            std::string line;
            while (std::getline(fs, line))
            {
                auto tokens = split(line, 2);
                if (tokens.size() < 3 || tokens.at(0) != "P") { continue; }
                if (tokens.at(1) == "acf/_ICAO")
                {
                    const std::string icao(tokens.at(2));
                    model.setAircraftIcaoCode(icao);
                }
                else if (tokens.at(1) == "acf/_descrip")
                {
                    const std::string desc(tokens.at(2));
                    model.setDescription("[ACF] " + desc);
                }
                else if (tokens.at(1) == "acf/_name")
                {
                    const std::string name(tokens.at(2));
                    model.setName(name);
                }
                else if (tokens.at(1) == "acf/_studio")
                {
                    const CDistributor dist(tokens.at(2));
                    model.setDistributor(dist);
                }
                else if (tokens.at(1) == "acf/_author")
                {
                    if (model.getDistributor().hasDescription()) { continue; }
                    std::string author = tokens.at(2);
                    size_t pos = author.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
                    author = author.substr(0, pos);
                    if (author.empty()) { continue; }
                    const CDistributor dist(author);
                    model.setDistributor(dist);
                }
            }
        }

        fs.close();

        model.setModelString(stringForFlyableModel(model, filePath));
        if (!model.hasDescription()) { model.setDescription(descriptionForFlyableModel(model)); }
        return model;
    }

}
