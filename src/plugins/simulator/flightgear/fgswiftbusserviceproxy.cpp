/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "fgswiftbusserviceproxy.h"
#include "simulatorflightgear.h"

#include <QLatin1String>
#include <QPointer>

class QDBusConnection;

#define FGSWIFTBUS_SERVICE_SERVICENAME "org.swift-project.fgswiftbus"

namespace BlackSimPlugin
{
    namespace Flightgear
    {
        CFGSwiftBusServiceProxy::CFGSwiftBusServiceProxy(QDBusConnection &connection, QObject *parent, bool dummy) : QObject(parent)
        {
            m_dbusInterface = new BlackMisc::CGenericDBusInterface(FGSWIFTBUS_SERVICE_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
            if (!dummy)
            {
                bool s;
                s = connection.connect(QString(), "/fgswiftbus/service", "org.swift_project.fgswiftbus.service",
                                       "aircraftModelChanged", this,
                                       SIGNAL(aircraftModelChanged(QString, QString, QString, QString, QString, QString, QString)));
                Q_ASSERT(s);

                s = connection.connect(QString(), "/fgswiftbus/service", "org.swift_project.fgswiftbus.service",
                                       "airportsInRangeUpdated", this,
                                       SIGNAL(airportsInRangeUpdated(QStringList, QStringList, QList<double>, QList<double>, QList<double>)));
                Q_ASSERT(s);
            }
        }

        int CFGSwiftBusServiceProxy::getVersionNumber()
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getVersionNumber"));
        }

        void CFGSwiftBusServiceProxy::getOwnAircraftSituationData(FlightgearData *o_flightgearData)
        {
            if (!o_flightgearData) { return; }
            QPointer<CFGSwiftBusServiceProxy> myself(this);
            std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
            {
                if (!myself) { return; }
                QDBusPendingReply<double, double, double, double, double, double, double, double> reply = *watcher;
                if (!reply.isError())
                {
                    o_flightgearData->latitudeDeg = reply.argumentAt<0>();
                    o_flightgearData->longitudeDeg = reply.argumentAt<1>();
                    o_flightgearData->altitudeFt = reply.argumentAt<2>();
                    o_flightgearData->groundspeedKts = reply.argumentAt<3>();
                    o_flightgearData->pitchDeg = reply.argumentAt<4>();
                    o_flightgearData->rollDeg = reply.argumentAt<5>();
                    o_flightgearData->trueHeadingDeg = reply.argumentAt<6>();
                    o_flightgearData->pressureAltitudeFt = reply.argumentAt<7>();
                }
                watcher->deleteLater();
            };
            m_dbusInterface->callDBusAsync(QLatin1String("getOwnAircraftSituationData"), callback);
        }

        void CFGSwiftBusServiceProxy::getOwnAircraftVelocityData(FlightgearData *o_flightgearData)
        {
            if (!o_flightgearData) { return; }
            QPointer<CFGSwiftBusServiceProxy> myself(this);
            std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
            {
                if (!myself) { return; }
                QDBusPendingReply<double, double, double, double, double, double> reply = *watcher;
                if (!reply.isError())
                {
                    o_flightgearData->velocityXMs = reply.argumentAt<0>();
                    o_flightgearData->velocityYMs = reply.argumentAt<1>();
                    o_flightgearData->velocityZMs = reply.argumentAt<2>();
                    o_flightgearData->pitchRateRadPerSec = reply.argumentAt<3>();
                    o_flightgearData->rollRateRadPerSec = reply.argumentAt<4>();
                    o_flightgearData->yawRateRadPerSec = reply.argumentAt<5>();
                }
                watcher->deleteLater();
            };
            m_dbusInterface->callDBusAsync(QLatin1String("getOwnAircraftVelocityData"), callback);

        }

        void CFGSwiftBusServiceProxy::addTextMessage(const QString &text)
        {
            m_dbusInterface->callDBus(QLatin1String("addTextMessage"), text);
        }

        QString CFGSwiftBusServiceProxy::getAircraftModelPath() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelPath"));
        }
        void CFGSwiftBusServiceProxy::getAircraftModelPathAsync(QString *o_modelPath)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftModelPath"), setterCallback(o_modelPath));
        }

        QString CFGSwiftBusServiceProxy::getAircraftModelFilename() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelFilename"));
        }
        void CFGSwiftBusServiceProxy::getAircraftModelFilenameAsync(QString *o_modelFilename)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftModelFilename"), setterCallback(o_modelFilename));
        }

        QString CFGSwiftBusServiceProxy::getAircraftLivery() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftLivery"));
        }
        void CFGSwiftBusServiceProxy::getAircraftLiveryAsync(QString *o_modelLivery)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftLivery"), setterCallback(o_modelLivery));
        }

        QString CFGSwiftBusServiceProxy::getAircraftIcaoCode() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftIcaoCode"));
        }
        void CFGSwiftBusServiceProxy::getAircraftIcaoCodeAsync(QString *o_icaoCode)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftIcaoCode"), setterCallback(o_icaoCode));
        }

        QString CFGSwiftBusServiceProxy::getAircraftDescription() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftDescription"));
        }
        void CFGSwiftBusServiceProxy::getAircraftDescriptionAsync(QString *o_description)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftDescription"), setterCallback(o_description));
        }

        QString CFGSwiftBusServiceProxy::getAircraftModelString() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelString"));
        }
        void CFGSwiftBusServiceProxy::getAircraftModelStringAsync(QString *o_modelString)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftModelString"), setterCallback(o_modelString));
        }

        QString CFGSwiftBusServiceProxy::getAircraftName() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftName"));
        }
        void CFGSwiftBusServiceProxy::getAircraftNameAsync(QString *o_name)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftName"), setterCallback(o_name));
        }

        bool CFGSwiftBusServiceProxy::isPaused() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("isPaused"));
        }
        void CFGSwiftBusServiceProxy::isPausedAsync(bool *o_paused)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("isPaused"), setterCallback(o_paused));
        }

        double CFGSwiftBusServiceProxy::getLatitudeDeg() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getLatitudeDeg"));
        }
        void CFGSwiftBusServiceProxy::getLatitudeDegAsync(double *o_latitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLatitudeDeg"), setterCallback(o_latitude));
        }

        double CFGSwiftBusServiceProxy::getLongitudeDeg() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getLongitudeDeg"));
        }
        void CFGSwiftBusServiceProxy::getLongitudeDegAsync(double *o_longitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLongitudeDeg"), setterCallback(o_longitude));
        }

        double CFGSwiftBusServiceProxy::getAltitudeMslFt() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getAltitudeMslFt"));
        }
        void CFGSwiftBusServiceProxy::getAltitudeMslFtAsync(double *o_altitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAltitudeMslFt"), setterCallback(o_altitude));
        }

        double CFGSwiftBusServiceProxy::getHeightAglFt() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getHeightAglFt"));
        }
        void CFGSwiftBusServiceProxy::getHeightAglFtAsync(double *o_height)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getHeightAglFt"), setterCallback(o_height));
        }

        double CFGSwiftBusServiceProxy::getGroundSpeedKts() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getGroundSpeedKts"));
        }
        void CFGSwiftBusServiceProxy::getGroundSpeedKtsAsync(double *o_groundspeed)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getGroundSpeedKts"), setterCallback(o_groundspeed));
        }

        double CFGSwiftBusServiceProxy::getPitchDeg() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getPitchDeg"));
        }
        void CFGSwiftBusServiceProxy::getPitchDegAsync(double *o_pitch)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getPitchDeg"), setterCallback(o_pitch));
        }

        double CFGSwiftBusServiceProxy::getRollDeg() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getRollDeg"));
        }
        void CFGSwiftBusServiceProxy::getRollDegAsync(double *o_roll)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getRollDeg"), setterCallback(o_roll));
        }

        bool CFGSwiftBusServiceProxy::getAllWheelsOnGround() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getAllWheelsOnGround"));
        }
        void CFGSwiftBusServiceProxy::getAllWheelsOnGroundAsync(bool *o_allWheels)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAllWheelsOnGround"), setterCallback(o_allWheels));
        }

        int CFGSwiftBusServiceProxy::getCom1ActiveKhz() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom1ActiveKhz"));
        }
        void CFGSwiftBusServiceProxy::getCom1ActiveKhzAsync(int *o_com1Active)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom1ActiveKhz"), setterCallback(o_com1Active));
        }

        int CFGSwiftBusServiceProxy::getCom1StandbyKhz() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom1StandbyKhz"));
        }
        void CFGSwiftBusServiceProxy::getCom1StandbyKhzAsync(int *o_com1Standby)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom1StandbyKhz"), setterCallback(o_com1Standby));
        }

        int CFGSwiftBusServiceProxy::getCom2ActiveKhz() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom2ActiveKhz"));
        }
        void CFGSwiftBusServiceProxy::getCom2ActiveKhzAsync(int *o_com2Active)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom2ActiveKhz"), setterCallback(o_com2Active));
        }

        int CFGSwiftBusServiceProxy::getCom2StandbyKhz() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom2StandbyKhz"));
        }
        void CFGSwiftBusServiceProxy::getCom2StandbyKhzAsync(int *o_com2Standby)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom2StandbyKhz"), setterCallback(o_com2Standby));
        }

        int CFGSwiftBusServiceProxy::getTransponderCode() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getTransponderCode"));
        }
        void CFGSwiftBusServiceProxy::getTransponderCodeAsync(int *o_xpdrCode)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTransponderCode"), setterCallback(o_xpdrCode));
        }

        int CFGSwiftBusServiceProxy::getTransponderMode() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getTransponderMode"));
        }
        void CFGSwiftBusServiceProxy::getTransponderModeAsync(int *o_xpdrMode)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTransponderMode"), setterCallback(o_xpdrMode));
        }

        bool CFGSwiftBusServiceProxy::getTransponderIdent() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getTransponderIdent"));
        }
        void CFGSwiftBusServiceProxy::getTransponderIdentAsync(bool *o_ident)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTransponderIdent"), setterCallback(o_ident));
        }

        bool CFGSwiftBusServiceProxy::getBeaconLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getBeaconLightsOn"));
        }

        void CFGSwiftBusServiceProxy::getBeaconLightsOnAsync(bool *o_beaconLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getBeaconLightsOn"), setterCallback(o_beaconLightsOn));
        }

        bool CFGSwiftBusServiceProxy::getLandingLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getLandingLightsOn"));
        }

        void CFGSwiftBusServiceProxy::getLandingLightsOnAsync(bool *o_landingLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLandingLightsOn"), setterCallback(o_landingLightsOn));
        }

        bool CFGSwiftBusServiceProxy::getNavLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getNavLightsOn"));
        }

        void CFGSwiftBusServiceProxy::getNavLightsOnAsync(bool *o_navLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getNavLightsOn"), setterCallback(o_navLightsOn));
        }

        bool CFGSwiftBusServiceProxy::getStrobeLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getStrobeLightsOn"));
        }

        void CFGSwiftBusServiceProxy::getStrobeLightsOnAsync(bool *o_strobeLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getStrobeLightsOn"), setterCallback(o_strobeLightsOn));
        }

        bool CFGSwiftBusServiceProxy::getTaxiLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getTaxiLightsOn"));
        }

        void CFGSwiftBusServiceProxy::getTaxiLightsOnAsync(bool *o_taxiLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTaxiLightsOn"), setterCallback(o_taxiLightsOn));
        }

        double CFGSwiftBusServiceProxy::getPressureAltitudeFt() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getPressureAltitudeFt"));
        }

        void CFGSwiftBusServiceProxy::getPressureAltitudeFtAsync(double *o_qnh)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getPressureAltitudeFt"), setterCallback(o_qnh));
        }


        void CFGSwiftBusServiceProxy::setCom1ActiveKhz(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom1ActiveKhz"), freq);
        }

        void CFGSwiftBusServiceProxy::setCom1StandbyKhz(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom1StandbyKhz"), freq);
        }

        void CFGSwiftBusServiceProxy::setCom2ActiveKhz(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom2ActiveKhz"), freq);
        }

        void CFGSwiftBusServiceProxy::setCom2StandbyKhz(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom2StandbyKhz"), freq);
        }

        void CFGSwiftBusServiceProxy::setTransponderCode(int code)
        {
            m_dbusInterface->callDBus(QLatin1String("setTransponderCode"), code);
        }

        void CFGSwiftBusServiceProxy::setTransponderMode(int mode)
        {
            m_dbusInterface->callDBus(QLatin1String("setTransponderMode"), mode);
        }
        double CFGSwiftBusServiceProxy::getFlapsDeployRatio() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getFlapsDeployRatio"));
        }

        void CFGSwiftBusServiceProxy::getFlapsDeployRatioAsync(double *o_flapsDeployRatio)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getFlapsDeployRatio"), setterCallback(o_flapsDeployRatio));
        }

        double CFGSwiftBusServiceProxy::getGearDeployRatio() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getGearDeployRatio"));
        }

        void CFGSwiftBusServiceProxy::getGearDeployRatioAsync(double *o_gearDeployRatio)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getGearDeployRatio"), setterCallback(o_gearDeployRatio));
        }

        QList<double> CFGSwiftBusServiceProxy::getEngineN1Percentage() const
        {
            return m_dbusInterface->callDBusRet<QList<double>>(QLatin1String("getEngineN1Percentage"));
        }

        void CFGSwiftBusServiceProxy::getEngineN1PercentageAsync(QList<double> *o_engineN1Percentage)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getEngineN1Percentage"), setterCallback(o_engineN1Percentage));
        }

        double CFGSwiftBusServiceProxy::getSpeedBrakeRatio() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getSpeedBrakeRatio"));
        }

        void CFGSwiftBusServiceProxy::getSpeedBrakeRatioAsync(double *o_speedBrakeRatio)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getSpeedBrakeRatio"), setterCallback(o_speedBrakeRatio));
        }

        double CFGSwiftBusServiceProxy::getGroundElevation() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getGroundElevation"));
        }

        void CFGSwiftBusServiceProxy::getGroundElevationAsync(double *o_groundElevation)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getGroundElevation"),setterCallback(o_groundElevation));
        }
    } // ns
} // ns
