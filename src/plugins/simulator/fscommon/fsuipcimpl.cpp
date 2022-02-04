/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#if defined(SWIFT_USING_FSUIPC32) || defined(SWIFT_USING_FSUIPC64)

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "fsuipc.h"
#include <Windows.h>
// bug in FSUIPC_User.h, windows.h not included, so we have to import it first

#ifdef SWIFT_USING_FSUIPC32
extern "C" {
#include "../fsuipc32/IPCuser.h"
#include "../fsuipc32/FSUIPC_User.h"
#include "../fsuipc32/NewWeather.h"
}
#elif SWIFT_USING_FSUIPC64
extern "C" {
#include "../fsuipc64/IPCuser64.h"
#include "../fsuipc64/FSUIPC_User64.h"
#include "../fsuipc64/NewWeather.h"
}
#endif

#include "blackmisc/simulation/fscommon/bcdconversions.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/logmessage.h"

#include <QDebug>
#include <QLatin1Char>
#include <QDateTime>

using namespace BlackMisc;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Weather;

namespace BlackSimPlugin::FsCommon
{
    //! Fsuipc weather message
    struct CFsuipc::FsuipcWeatherMessage
    {
        FsuipcWeatherMessage() = default;
        FsuipcWeatherMessage(unsigned int offset, const QByteArray &data, int leftTrials);
        int m_offset = 0;
        QByteArray m_messageData;
        int m_leftTrials = 0;
    };

    CFsuipc::CFsuipc(QObject *parent) : QObject(parent)
    {
        startTimer(100);
    }

    CFsuipc::~CFsuipc()
    {
        this->close();
    }

    bool CFsuipc::open(bool force)
    {
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Open not threadsafe");
        DWORD dwResult;
        m_lastErrorMessage = "";
        m_lastErrorIndex = 0;
        if (!force && m_opened) { return m_opened; } // already connected

        if (FSUIPC_Open(SIM_ANY, &dwResult))
        {
            m_opened = true; // temp status
            m_openCount++;

            if (this->isOpen())
            {
                const int simIndex = static_cast<int>(FSUIPC_FS_Version);
                const QString sim = CFsuipc::simulator(simIndex);
                const QString ver = QStringLiteral("%1.%2.%3.%4%5")
                                    .arg(QLatin1Char(48 + (0x0f & (FSUIPC_Version >> 28))))
                                    .arg(QLatin1Char(48 + (0x0f & (FSUIPC_Version >> 24))))
                                    .arg(QLatin1Char(48 + (0x0f & (FSUIPC_Version >> 20))))
                                    .arg(QLatin1Char(48 + (0x0f & (FSUIPC_Version >> 16))))
                                    .arg((FSUIPC_Version & 0xffff) ? QString(QLatin1Char('a' + static_cast<char>(FSUIPC_Version & 0xff) - 1)) : "");
                m_fsuipcVersion = QStringLiteral("FSUIPC %1 (%2)").arg(ver, sim);
                CLogMessage(this).info(u"FSUIPC connected: %1") << m_fsuipcVersion;
            }
            else
            {
                CLogMessage(this).warning(u"FSUIPC opened, but verification failed");
                m_opened = false;
                FSUIPC_Close(); // under any circumstances close
            }
        }
        else
        {
            const int index = static_cast<int>(dwResult);
            m_lastErrorIndex = index;
            m_lastErrorMessage = CFsuipc::errorMessages().at(index);
            CLogMessage(this).warning(u"FSUIPC not connected: %1") << m_lastErrorMessage;
            m_opened = false;
            FSUIPC_Close(); // under any circumstances close
        }

        return m_opened;
    }

    void CFsuipc::close()
    {
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Open not threadsafe");
        if (m_opened)
        {
            CLogMessage(this).info(u"Closing FSUIPC: %1") << m_fsuipcVersion;
        }
        FSUIPC_Close(); // Closing when it wasn't open is okay, so this is safe here
        m_closeCount++;
        m_opened = false;
    }

    bool CFsuipc::isOpened() const
    {
        return m_opened;
    }

    bool CFsuipc::isOpen() const
    {
        if (!this->isOpened()) { return false; }

        // test read
        DWORD dwResult;
        char localFsTimeRaw[3];
        if (FSUIPC_Read(0x0238, 3, localFsTimeRaw, &dwResult) && FSUIPC_Process(&dwResult))
        {
            return dwResult == 0;
        }
        return false;
    }

    bool CFsuipc::write(const CSimulatedAircraft &aircraft)
    {
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Open not threadsafe");
        if (!this->isOpened()) { return false; }

        quint16 com1ActiveRaw  = static_cast<quint16>(aircraft.getCom1System().getFrequencyActive().value(CFrequencyUnit::MHz())  * 100);
        quint16 com2ActiveRaw  = static_cast<quint16>(aircraft.getCom2System().getFrequencyActive().value(CFrequencyUnit::MHz())  * 100);
        quint16 com1StandbyRaw = static_cast<quint16>(aircraft.getCom1System().getFrequencyStandby().value(CFrequencyUnit::MHz()) * 100);
        quint16 com2StandbyRaw = static_cast<quint16>(aircraft.getCom2System().getFrequencyStandby().value(CFrequencyUnit::MHz()) * 100);
        com1ActiveRaw  = static_cast<quint16>(CBcdConversions::dec2Bcd(com1ActiveRaw - 10000));
        com2ActiveRaw  = static_cast<quint16>(CBcdConversions::dec2Bcd(com2ActiveRaw - 10000));
        com1StandbyRaw = static_cast<quint16>(CBcdConversions::dec2Bcd(com1StandbyRaw - 10000));
        com2StandbyRaw = static_cast<quint16>(CBcdConversions::dec2Bcd(com2StandbyRaw - 10000));
        quint16 transponderCodeRaw = static_cast<quint16>(aircraft.getTransponderCode());
        transponderCodeRaw = static_cast<quint16>(CBcdConversions::dec2Bcd(transponderCodeRaw));

        DWORD dwResult = 0;
        bool ok =
            FSUIPC_Write(0x034e, 2, &com1ActiveRaw,  &dwResult) &&
            FSUIPC_Write(0x3118, 2, &com2ActiveRaw,  &dwResult) &&
            FSUIPC_Write(0x311a, 2, &com1StandbyRaw, &dwResult) &&
            FSUIPC_Write(0x311c, 2, &com2StandbyRaw, &dwResult) &&
            FSUIPC_Write(0x0354, 2, &transponderCodeRaw, &dwResult);
        if (ok) { FSUIPC_Process(&dwResult); }

        ok = ok && write(aircraft.getTransponder());

        return ok && dwResult == 0;
    }

    bool CFsuipc::write(const CTransponder &xpdr)
    {
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Open not threadsafe");
        if (!this->isOpened()) { return false; }

        // should be the same as writing via SimConnect data area
        DWORD dwResult;
        byte xpdrModeSb3Raw  = xpdr.isInStandby()   ? 1U : 0U;
        byte xpdrIdentSb3Raw = xpdr.isIdentifying() ? 1U : 0U;
        const bool ok =
            FSUIPC_Write(0x7b91, 1, &xpdrModeSb3Raw,  &dwResult) &&
            FSUIPC_Write(0x7b93, 1, &xpdrIdentSb3Raw, &dwResult);
        if (ok) { FSUIPC_Process(&dwResult); }
        return ok && dwResult == 0;
    }

    bool CFsuipc::write(const CWeatherGrid &weatherGrid)
    {
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Open not threadsafe");
        if (!this->isOpened()) { return false; }

        if (weatherGrid.isEmpty()) { return false; }
        this->clearAllWeather();
        CLogMessage(this).debug(u"FSUIPC cleared weather");

        const CGridPoint gridPoint = weatherGrid.front();
        NewWeather nw;

        // Clear new weather
        nw.uCommand    = NW_SET;
        nw.uFlags      = 0;
        nw.ulSignature = 0;
        nw.uDynamics   = 0;
        for (std::size_t i = 0; i < sizeof(nw.uSpare) / sizeof(nw.uSpare[0]); i++) { nw.uSpare[i] = 0; }

        nw.dLatitude    = 0.0;
        nw.dLongitude   = 0.0;
        nw.nElevation   = 0;
        nw.ulTimeStamp  = 0;
        nw.nTempCtr     = 0;
        nw.nWindsCtr    = 0;
        nw.nCloudsCtr   = 0;
        nw.nElevation   = 0; // meters * 65536;
        nw.nUpperVisCtr = 0;

        // todo: Take station from weather grid
        memcpy(nw.chICAO, "GLOB", 4);

        const CVisibilityLayerList visibilityLayers = gridPoint.getVisibilityLayers().sortedBy(&CVisibilityLayer::getBase);
        const auto surfaceVisibility = visibilityLayers.frontOrDefault();
        NewVis vis;
        vis.LowerAlt = static_cast<short>(surfaceVisibility.getBase().valueInteger(CLengthUnit::m()));
        vis.UpperAlt = static_cast<ushort>(surfaceVisibility.getTop().valueInteger(CLengthUnit::m()));
        // Range is measured in: 1/100ths sm
        vis.Range = static_cast<ushort>(surfaceVisibility.getVisibility().value(CLengthUnit::SM()) * 100);
        vis.Spare = 0;
        nw.Vis = vis;

        for (const auto &visibilityLayer : visibilityLayers)
        {
            vis.LowerAlt = static_cast<short>(visibilityLayer.getBase().valueInteger(CLengthUnit::m()));
            vis.UpperAlt = static_cast<ushort>(visibilityLayer.getTop().valueInteger(CLengthUnit::m()));
            vis.Range    = static_cast<ushort>(visibilityLayer.getVisibility().value(CLengthUnit::SM()) * 100);
            vis.Spare = 0;
            nw.UpperVis[nw.nUpperVisCtr++] = vis;
        }
        CLogMessage(this).debug(u"FSUIPC updated %1 visibility layers") << visibilityLayers.size();

        const CTemperatureLayerList temperatureLayers = gridPoint.getTemperatureLayers().sortedBy(&CTemperatureLayer::getLevel);
        for (const auto &temperatureLayer : temperatureLayers)
        {
            NewTemp temp;
            temp.Alt = static_cast<ushort>(temperatureLayer.getLevel().valueInteger(CLengthUnit::m()));
            temp.Day = static_cast<short>(temperatureLayer.getTemperature().valueInteger(CTemperatureUnit::C()));
            temp.DayNightVar = 3;
            temp.DewPoint = static_cast<short>(temperatureLayer.getDewPoint().value(CTemperatureUnit::C()));
            nw.Temp[nw.nTempCtr++] = temp;
        }
        CLogMessage(this).debug(u"FSUIPC updated %1 temperature layers") << temperatureLayers.size();

        const CCloudLayerList cloudLayers = gridPoint.getCloudLayers().sortedBy(&CCloudLayer::getBase);
        for (const auto &cloudLayer : cloudLayers)
        {
            NewCloud cloud;
            switch (cloudLayer.getCoverage())
            {
            case CCloudLayer::None:      cloud.Coverage = 0; break;
            case CCloudLayer::Few:       cloud.Coverage = 2; break;
            case CCloudLayer::Scattered: cloud.Coverage = 4; break;
            case CCloudLayer::Broken:    cloud.Coverage = 6; break;
            case CCloudLayer::Overcast:  cloud.Coverage = 8; break;
            default:                     cloud.Coverage = 0; break;
            }

            cloud.Deviation = 0;
            cloud.Icing = 0;
            cloud.LowerAlt = static_cast<ushort>(cloudLayer.getBase().valueInteger(CLengthUnit::m()));
            cloud.PrecipBase = 0;

            // Light rain - when the precipitation rate is < 2.5 mm (0.098 in) per hour
            // Moderate rain - when the precipitation rate is between 2.5 mm (0.098 in) - 7.6 mm (0.30 in) or 10 mm (0.39 in) per hour
            // Heavy rain - when the precipitation rate is > 7.6 mm (0.30 in) per hour, or between 10 mm (0.39 in) and 50 mm (2.0 in) per hour
            // Violent rain - when the precipitation rate is > 50 mm (2.0 in) per hour

            cloud.PrecipRate = 2 * static_cast<unsigned char>(cloudLayer.getPrecipitationRate());
            cloud.PrecipType = static_cast<unsigned char>(cloudLayer.getPrecipitation());
            cloud.TopShape   = 0;
            cloud.Turbulence = 0;

            switch (cloudLayer.getClouds())
            {
            case CCloudLayer::NoClouds: cloud.Type = 0; break;
            case CCloudLayer::Cirrus:   cloud.Type = 1; break;
            case CCloudLayer::Stratus:  cloud.Type = 8; break;
            case CCloudLayer::Cumulus:  cloud.Type = 9; break;
            case CCloudLayer::Thunderstorm: cloud.Type = 10; break;
            default: cloud.Type = 0;
            }

            cloud.UpperAlt = static_cast<ushort>(cloudLayer.getTop().valueInteger(CLengthUnit::m()));
            nw.Cloud[nw.nCloudsCtr++] = cloud;
        }
        CLogMessage(this).debug(u"FSUIPC updated %1 cloud layers") << cloudLayers.size();

        const CWindLayerList windLayers = gridPoint.getWindLayers().sortedBy(&CWindLayer::getLevel);
        for (const auto &windLayer : windLayers)
        {
            NewWind wind;
            wind.Direction = static_cast<ushort>(windLayer.getDirection().value(CAngleUnit::deg()) * 65536 / 360.0);
            wind.GapAbove = 0;
            wind.Gust = static_cast<ushort>(windLayer.getGustSpeed().valueInteger(CSpeedUnit::kts()));
            wind.Shear = 0;
            wind.Speed = static_cast<ushort>(windLayer.getSpeed().valueInteger(CSpeedUnit::kts()));
            wind.SpeedFract = 0;
            wind.Turbulence = 0;
            wind.UpperAlt = static_cast<ushort>(windLayer.getLevel().valueInteger(CLengthUnit::m()));
            wind.Variance = 0;
            nw.Wind[nw.nWindsCtr++] = wind;
        }
        CLogMessage(this).debug(u"FSUIPC updated %1 wind layers") << windLayers.size();

        NewPress press;
        press.Drift = 0;
        // Pressure is measured in: 16 x mb
        press.Pressure = static_cast<ushort>(gridPoint.getPressureAtMsl().value(CPressureUnit::mbar()) * 16);
        nw.Press = press;

        // writing will take place in
        QByteArray weatherData(reinterpret_cast<const char *>(&nw), sizeof(NewWeather));
        m_weatherMessageQueue.append(FsuipcWeatherMessage(0xC800, weatherData, 5));
        return true;
    }

    bool CFsuipc::setSimulatorTime(int hour, int minute)
    {
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Open not threadsafe");
        if (!this->isOpened()) { return false; }

        // should be the same as writing via SimConnect data area
        DWORD dwResult;
        quint8 hourRaw   = static_cast<quint8>(hour);
        quint8 minuteRaw = static_cast<quint8>(minute);

        const bool ok =
            FSUIPC_Write(0x023b, 1, &hourRaw,  &dwResult) &&
            FSUIPC_Write(0x023c, 1, &minuteRaw, &dwResult);
        if (ok) { FSUIPC_Process(&dwResult); }
        return ok && dwResult == 0;
    }

    QString CFsuipc::getVersion() const
    {
        return m_fsuipcVersion;
    }

    bool CFsuipc::read(CSimulatedAircraft &aircraft, bool cockpit, bool situation, bool aircraftParts)
    {
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Open not threadsafe");
        DWORD dwResult = 0;
        char localFsTimeRaw[3];
        char modelNameRaw[256];
        qint16 com1ActiveRaw = 0, com2ActiveRaw = 0, com1StandbyRaw = 0, com2StandbyRaw = 0;
        qint16 transponderCodeRaw = 0;
        byte xpdrModeSb3Raw = 1, xpdrIdentSb3Raw = 1;
        qint32 groundspeedRaw = 0, pitchRaw = 0, bankRaw = 0, headingRaw = 0;
        qint64 altitudeRaw = 0;
        double pressureAltitudeRaw = 0; // 34B0
        qint32 groundAltitudeRaw = 0;
        qint64 latitudeRaw = 0, longitudeRaw = 0;
        qint16 lightsRaw = 0;
        qint16 onGroundRaw = 0;
        qint32 flapsControlRaw = 0, gearControlRaw = 0, spoilersControlRaw = 0;
        qint16 numberOfEngines = 0;
        qint16 engine1CombustionFlag = 0, engine2CombustionFlag = 0, engine3CombustionFlag = 0, engine4CombustionFlag = 0;
        double velocityWorld[3];
        double rotationVelocityBody[3];

        // http://www.projectmagenta.com/all-fsuipc-offsets/
        // https://www.ivao.aero/softdev/ivap/fsuipc_sdk.asp
        // http://squawkbox.ca/doc/sdk/fsuipc.php

        if (!this->isOpened()) { return false; }
        if (!(aircraftParts || situation || cockpit)) { return false; }

        bool read = false;
        bool cockpitN = !cockpit;
        bool situationN = !situation;
        bool aircraftPartsN = !aircraftParts;

        /**  KB 2019-03 disabled as this should be fixed the whole block can be removed as soon this has been tested
        //! \todo KB 2018-11 BUG fix for broken connection, needs to go as soon as issue is fixed
        //! Seems to be fixed with
        //! a) https://dev.swift-project.org/T471
        //! b) https://dev.swift-project.org/T444
        //! Remove after 2019-03 if issue is resolved
        if (!(FSUIPC_Read(0x0238, 3, localFsTimeRaw, &dwResult) && FSUIPC_Process(&dwResult)))
        {
            FSUIPC_Close();
            FSUIPC_Open(SIM_ANY, &dwResult);
            m_closeCount++;
            m_openCount++;
            if (m_openCount < 10)
            {
                CLogMessage(this).warning(u"Used FSUIPC open/close workaround");
            }
        }
        **/

        if (
            FSUIPC_Read(0x0238, 3, localFsTimeRaw, &dwResult) &&

            // COM settings
            (cockpitN || FSUIPC_Read(0x034e, 2, &com1ActiveRaw,  &dwResult)) &&
            (cockpitN || FSUIPC_Read(0x3118, 2, &com2ActiveRaw,  &dwResult)) &&
            (cockpitN || FSUIPC_Read(0x311a, 2, &com1StandbyRaw, &dwResult)) &&
            (cockpitN || FSUIPC_Read(0x311c, 2, &com2StandbyRaw, &dwResult)) &&
            (cockpitN || FSUIPC_Read(0x0354, 2, &transponderCodeRaw, &dwResult)) &&

            // COM Settings, transponder, SB3
            (cockpitN || FSUIPC_Read(0x7b91, 1, &xpdrModeSb3Raw,  &dwResult)) &&
            (cockpitN || FSUIPC_Read(0x7b93, 1, &xpdrIdentSb3Raw, &dwResult)) &&

            // Speeds, situation
            (situationN || FSUIPC_Read(0x02b4, 4, &groundspeedRaw, &dwResult)) &&
            (situationN || FSUIPC_Read(0x0578, 4, &pitchRaw, &dwResult)) &&
            (situationN || FSUIPC_Read(0x057c, 4, &bankRaw,  &dwResult)) &&
            (situationN || FSUIPC_Read(0x0580, 4, &headingRaw,  &dwResult)) &&
            (situationN || FSUIPC_Read(0x0570, 8, &altitudeRaw, &dwResult)) &&

            (situationN || FSUIPC_Read(0x3198, 8, &velocityWorld[0], &dwResult)) &&
            (situationN || FSUIPC_Read(0x31a0, 8, &velocityWorld[1], &dwResult)) &&
            (situationN || FSUIPC_Read(0x3190, 8, &velocityWorld[2], &dwResult)) &&

            (situationN || FSUIPC_Read(0x30A8, 8, &rotationVelocityBody[0], &dwResult)) &&
            (situationN || FSUIPC_Read(0x30B0, 8, &rotationVelocityBody[1], &dwResult)) &&
            (situationN || FSUIPC_Read(0x30B8, 8, &rotationVelocityBody[2], &dwResult)) &&

            // Position
            (situationN || FSUIPC_Read(0x0560, 8, &latitudeRaw,  &dwResult)) &&
            (situationN || FSUIPC_Read(0x0568, 8, &longitudeRaw, &dwResult)) &&
            (situationN || FSUIPC_Read(0x0020, 4, &groundAltitudeRaw,   &dwResult)) &&
            (situationN || FSUIPC_Read(0x34B0, 8, &pressureAltitudeRaw, &dwResult)) &&

            // model name
            FSUIPC_Read(0x3d00, 256, &modelNameRaw, &dwResult) &&

            // aircraft parts
            (aircraftPartsN || FSUIPC_Read(0x0D0C, 2, &lightsRaw,   &dwResult)) &&
            (aircraftPartsN || FSUIPC_Read(0x0366, 2, &onGroundRaw, &dwResult)) &&
            (aircraftPartsN || FSUIPC_Read(0x0BDC, 4, &flapsControlRaw, &dwResult)) &&
            (aircraftPartsN || FSUIPC_Read(0x0BE8, 4, &gearControlRaw,  &dwResult)) &&
            (aircraftPartsN || FSUIPC_Read(0x0BD0, 4, &spoilersControlRaw, &dwResult)) &&

            // engines
            (aircraftPartsN || FSUIPC_Read(0x0AEC, 2, &numberOfEngines, &dwResult)) &&
            (aircraftPartsN || FSUIPC_Read(0x0894, 2, &engine1CombustionFlag, &dwResult)) &&
            (aircraftPartsN || FSUIPC_Read(0x092C, 2, &engine2CombustionFlag, &dwResult)) &&
            (aircraftPartsN || FSUIPC_Read(0x09C4, 2, &engine3CombustionFlag, &dwResult)) &&
            (aircraftPartsN || FSUIPC_Read(0x0A5C, 2, &engine4CombustionFlag, &dwResult)) &&

            // If we wanted other reads/writes at the same time, we could put them here
            FSUIPC_Process(&dwResult))
        {
            read = true;

            if (cockpit)
            {
                // COMs
                CComSystem com1 = aircraft.getCom1System();
                CComSystem com2 = aircraft.getCom2System();
                CTransponder xpdr = aircraft.getTransponder();

                // 2710 => 12710 => / 100.0 => 127.1
                com1ActiveRaw  = static_cast<short>(10000 + CBcdConversions::bcd2Dec(com1ActiveRaw));
                com2ActiveRaw  = static_cast<short>(10000 + CBcdConversions::bcd2Dec(com2ActiveRaw));
                com1StandbyRaw = static_cast<short>(10000 + CBcdConversions::bcd2Dec(com1StandbyRaw));
                com2StandbyRaw = static_cast<short>(10000 + CBcdConversions::bcd2Dec(com2StandbyRaw));
                com1.setFrequencyActiveMHz(com1ActiveRaw / 100.0);
                com2.setFrequencyActiveMHz(com2ActiveRaw / 100.0);
                com1.setFrequencyStandbyMHz(com1StandbyRaw / 100.0);
                com2.setFrequencyStandbyMHz(com2StandbyRaw / 100.0);

                transponderCodeRaw = static_cast<qint16>(CBcdConversions::bcd2Dec(transponderCodeRaw));
                xpdr.setTransponderCode(transponderCodeRaw);

                // Mode by SB3
                xpdr.setTransponderMode(xpdrModeSb3Raw == 0 ? CTransponder::ModeC : CTransponder::StateStandby);
                if (xpdrIdentSb3Raw != 0)
                {
                    // will be reset in CFsuipc::write
                    xpdr.setTransponderMode(CTransponder::StateIdent);
                }
                aircraft.setCockpit(com1, com2, xpdr);
            } // cockpit

            if (situation)
            {
                // position
                const double latCorrectionFactor = 90.0 / (10001750.0 * 65536.0 * 65536.0);
                const double lonCorrectionFactor = 360.0 / (65536.0 * 65536.0 * 65536.0 * 65536.0);
                // cppcheck-suppress shadowArgument
                CAircraftSituation situation = aircraft.getSituation();
                CCoordinateGeodetic position = situation.getPosition();
                CLatitude  lat(latitudeRaw  * latCorrectionFactor, CAngleUnit::deg());
                CLongitude lon(longitudeRaw * lonCorrectionFactor, CAngleUnit::deg());
                CAltitude  groundAltitude(groundAltitudeRaw / 256.0, CAltitude::MeanSeaLevel, CLengthUnit::m());
                position.setLatitude(lat);
                position.setLongitude(lon);
                position.setGeodeticHeight(groundAltitude);
                situation.setPosition(position);

                const double angleCorrectionFactor = 360.0 / 65536.0 / 65536.0; // see FSUIPC docu
                pitchRaw = qRound(std::floor(pitchRaw * angleCorrectionFactor));
                bankRaw  = qRound(std::floor(bankRaw * angleCorrectionFactor));

                // MSFS has inverted pitch and bank angles
                pitchRaw = ~pitchRaw;
                bankRaw  = ~bankRaw;
                if (pitchRaw < -90 || pitchRaw > 89) { CLogMessage(this).warning(u"FSUIPC: Pitch value out of limits: %1") << pitchRaw; }

                // speeds, situation
                CAngle pitch = CAngle(pitchRaw, CAngleUnit::deg());
                CAngle bank  = CAngle(bankRaw, CAngleUnit::deg());
                CHeading heading = CHeading(headingRaw * angleCorrectionFactor, CHeading::True, CAngleUnit::deg());
                CSpeed groundspeed(groundspeedRaw / 65536.0, CSpeedUnit::m_s());
                CAltitude altitude(altitudeRaw / (65536.0 * 65536.0), CAltitude::MeanSeaLevel, CLengthUnit::m());
                CAltitude pressureAltitude(pressureAltitudeRaw, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::m());
                situation.setBank(bank);
                situation.setHeading(heading);
                situation.setPitch(pitch);
                situation.setGroundSpeed(groundspeed);
                situation.setAltitude(altitude);
                situation.setPressureAltitude(pressureAltitude);
                situation.setVelocity({velocityWorld[0], velocityWorld[1], velocityWorld[2], CSpeedUnit::ft_s(), rotationVelocityBody[0],
                                        rotationVelocityBody[1], rotationVelocityBody[2], CAngleUnit::rad(), CTimeUnit::s()});
                situation.setGroundElevation(groundAltitude, CAircraftSituation::FromProvider);
                aircraft.setSituation(situation);
                //aircraft.setCG(altitude - groundAltitude); // calculate the CG
            } // situation

            // model
            const QString modelName = QString(modelNameRaw); // to be used to distinguish offsets for different models
            aircraft.setModelString(modelName);

            if (aircraftParts)
            {
                const CAircraftLights lights(lightsRaw & (1 << 4), lightsRaw & (1 << 2), lightsRaw & (1 << 3), lightsRaw & (1 << 1),
                                                lightsRaw & (1 << 0), lightsRaw & (1 << 8));

                const QList<bool> helperList { engine1CombustionFlag != 0, engine2CombustionFlag != 0,
                                                engine3CombustionFlag != 0, engine4CombustionFlag != 0
                                                };

                CAircraftEngineList engines;
                for (int index = 0; index < numberOfEngines; ++index)
                {
                    engines.push_back(CAircraftEngine(index + 1, helperList.at(index)));
                }

                CAircraftParts parts(lights, gearControlRaw == 16383, flapsControlRaw * 100 / 16383,
                                        spoilersControlRaw == 16383, engines, onGroundRaw == 1);

                aircraft.setParts(parts);
            } // parts
        } // read

        const int result = static_cast<int>(dwResult);
        if (m_lastErrorIndex != result && result > 0)
        {
            m_lastErrorIndex = result;
            m_lastErrorMessage = CFsuipc::errorMessage(result);
            CLogMessage(this).warning(u"FSUIPC read error '%1'") << m_lastErrorMessage;
        }
        return read;
    }

    void CFsuipc::timerEvent(QTimerEvent *event)
    {
        Q_UNUSED(event)
        processWeatherMessages();
    }

    CFsuipc::FsuipcWeatherMessage::FsuipcWeatherMessage(unsigned int offset, const QByteArray &data, int leftTrials) :
        m_offset(static_cast<int>(offset)), m_messageData(data), m_leftTrials(leftTrials)
    { }


    void CFsuipc::clearAllWeather()
    {
        if (!this->isOpened()) { return; }

        // clear all weather
        NewWeather nw;

        // Clear new weather
        nw.uCommand    = NW_CLEAR;
        nw.uFlags      = 0;
        nw.ulSignature = 0;
        nw.uDynamics   = 0;
        for (std::size_t i = 0; i < sizeof(nw.uSpare) / sizeof(nw.uSpare[0]); i++) { nw.uSpare[i] = 0; }

        nw.dLatitude   = 0.0;
        nw.dLongitude  = 0.0;
        nw.nElevation  = 0;
        nw.ulTimeStamp = 0;
        nw.nTempCtr    = 0;
        nw.nWindsCtr   = 0;
        nw.nCloudsCtr  = 0;
        const QByteArray clearWeather(reinterpret_cast<const char *>(&nw), sizeof(NewWeather));
        m_weatherMessageQueue.append(FsuipcWeatherMessage(0xC800, clearWeather, 1));
    }

    void CFsuipc::processWeatherMessages()
    {
        if (m_weatherMessageQueue.isEmpty()) { return; }
        if (!m_opened) { return; }
        FsuipcWeatherMessage &weatherMessage = m_weatherMessageQueue.first();

        DWORD dwResult;
        weatherMessage.m_leftTrials--;
        FSUIPC_Write(static_cast<DWORD>(weatherMessage.m_offset), static_cast<DWORD>(weatherMessage.m_messageData.size()), reinterpret_cast<void *>(weatherMessage.m_messageData.data()), &dwResult);

        unsigned int timeStamp = 0;
        FSUIPC_Read(0xC824, sizeof(timeStamp), &timeStamp, &dwResult);
        FSUIPC_Process(&dwResult);
        if (timeStamp > m_lastTimestamp)
        {
            m_weatherMessageQueue.removeFirst();
            m_lastTimestamp = timeStamp;
            return;
        }

        if (weatherMessage.m_leftTrials < 1)
        {
            CLogMessage(this).debug() << "Number of trials reached for weather message. Dropping it.";
            m_weatherMessageQueue.removeFirst();
        }
    }

    double CFsuipc::intToFractional(double fractional)
    {
        const double f = fractional / 10.0;
        if (f < 1.0) { return f; }
        return intToFractional(f);
    }
} // namespace

#endif // defined(SWIFT_USING_FSUIPC32) || defined(SWIFT_USING_FSUIPC64)
