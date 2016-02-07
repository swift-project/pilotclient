/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "xbusserviceproxy.h"
#include "blackmisc/dbusserver.h"
#include <QMetaMethod>

#define XBUS_SERVICE_SERVICENAME "org.swift-project.xbus"

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CXBusServiceProxy::CXBusServiceProxy(QDBusConnection &connection, QObject *parent, bool dummy) : QObject(parent)
        {
            m_dbusInterface = new BlackMisc::CGenericDBusInterface(XBUS_SERVICE_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
            if (! dummy) { m_dbusInterface->relayParentSignals(); }
        }

        void CXBusServiceProxy::addTextMessage(const QString &text, double red, double green, double blue)
        {
            m_dbusInterface->callDBus(QLatin1String("addTextMessage"), text, red, green, blue);
        }

        void CXBusServiceProxy::updateAirportsInRange()
        {
            m_dbusInterface->callDBus(QLatin1String("updateAirportsInRange"));
        }

        QString CXBusServiceProxy::getAircraftModelPath() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelPath"));
        }
        void CXBusServiceProxy::getAircraftModelPathAsync(QString *o_modelPath)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftModelPath"), setterCallback(o_modelPath));
        }

        QString CXBusServiceProxy::getAircraftModelFilename() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelFilename"));
        }
        void CXBusServiceProxy::getAircraftModelFilenameAsync(QString *o_modelFilename)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftModelFilename"), setterCallback(o_modelFilename));
        }

        QString CXBusServiceProxy::getAircraftLivery() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftLivery"));
        }
        void CXBusServiceProxy::getAircraftLiveryAsync(QString *o_modelLivery)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftLivery"), setterCallback(o_modelLivery));
        }

        QString CXBusServiceProxy::getAircraftIcaoCode() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftIcaoCode"));
        }
        void CXBusServiceProxy::getAircraftIcaoCodeAsync(QString *o_icaoCode)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftIcaoCode"), setterCallback(o_icaoCode));
        }

        int CXBusServiceProxy::getXPlaneVersionMajor() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getXPlaneVersionMajor"));
        }
        void CXBusServiceProxy::getXPlaneVersionMajorAsync(int *o_versionMajor)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getXPlaneVersionMajor"), setterCallback(o_versionMajor));
        }

        int CXBusServiceProxy::getXPlaneVersionMinor() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getXPlaneVersionMinor"));
        }
        void CXBusServiceProxy::getXPlaneVersionMinorAsync(int *o_versionMinor)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getXPlaneVersionMinor"), setterCallback(o_versionMinor));
        }

        QString CXBusServiceProxy::getXPlaneInstallationPath() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getXPlaneInstallationPath"));
        }
        void CXBusServiceProxy::getXPlaneInstallationPathAsync(QString *o_installPath)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getXPlaneInstallationPath"), setterCallback(o_installPath));
        }

        QString CXBusServiceProxy::getXPlanePreferencesPath() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getXPlanePreferencesPath"));
        }
        void CXBusServiceProxy::getXPlanePreferencesPathAsync(QString *o_prefsPath)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getXPlanePreferencesPath"), setterCallback(o_prefsPath));
        }

        bool CXBusServiceProxy::isPaused() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("isPaused"));
        }
        void CXBusServiceProxy::isPausedAsync(bool *o_paused)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("isPaused"), setterCallback(o_paused));
        }

        bool CXBusServiceProxy::isUsingRealTime() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("isUsingRealTime"));
        }
        void CXBusServiceProxy::isUsingRealTimeAsync(bool *o_isRealTime)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("isUsingRealTime"), setterCallback(o_isRealTime));
        }

        double CXBusServiceProxy::getLatitude() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getLatitude"));
        }
        void CXBusServiceProxy::getLatitudeAsync(double *o_latitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLatitude"), setterCallback(o_latitude));
        }

        double CXBusServiceProxy::getLongitude() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getLongitude"));
        }
        void CXBusServiceProxy::getLongitudeAsync(double *o_longitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLongitude"), setterCallback(o_longitude));
        }

        double CXBusServiceProxy::getAltitudeMSL() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getAltitudeMSL"));
        }
        void CXBusServiceProxy::getAltitudeMSLAsync(double *o_altitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAltitudeMSL"), setterCallback(o_altitude));
        }

        double CXBusServiceProxy::getHeightAGL() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getHeightAGL"));
        }
        void CXBusServiceProxy::getHeightAGLAsync(double *o_height)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getHeightAGL"), setterCallback(o_height));
        }

        double CXBusServiceProxy::getGroundSpeed() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getGroundSpeed"));
        }
        void CXBusServiceProxy::getGroundSpeedAsync(double *o_groundspeed)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getGroundSpeed"), setterCallback(o_groundspeed));
        }

        double CXBusServiceProxy::getIndicatedAirspeed() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getIndicatedAirspeed"));
        }
        void CXBusServiceProxy::getIndicatedAirspeedAsync(double *o_ias)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getIndicatedAirspeed"), setterCallback(o_ias));
        }

        double CXBusServiceProxy::getTrueAirspeed() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getTrueAirspeed"));
        }
        void CXBusServiceProxy::getTrueAirspeedAsync(double *o_tas)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTrueAirspeed"), setterCallback(o_tas));
        }

        double CXBusServiceProxy::getPitch() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getPitch"));
        }
        void CXBusServiceProxy::getPitchAsync(double *o_pitch)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getPitch"), setterCallback(o_pitch));
        }

        double CXBusServiceProxy::getRoll() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getRoll"));
        }
        void CXBusServiceProxy::getRollAsync(double *o_roll)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getRoll"), setterCallback(o_roll));
        }

        double CXBusServiceProxy::getTrueHeading() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getTrueHeading"));
        }
        void CXBusServiceProxy::getTrueHeadingAsync(double *o_heading)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTrueHeading"), setterCallback(o_heading));
        }

        bool CXBusServiceProxy::getAnyWheelOnGround() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getAnyWheelOnGround"));
        }
        void CXBusServiceProxy::getAnyWheelOnGroundAsync(bool *o_anyWheel)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAnyWheelOnGround"), setterCallback(o_anyWheel));
        }

        bool CXBusServiceProxy::getAllWheelsOnGround() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getAllWheelsOnGround"));
        }
        void CXBusServiceProxy::getAllWheelsOnGroundAsync(bool *o_allWheels)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAllWheelsOnGround"), setterCallback(o_allWheels));
        }

        int CXBusServiceProxy::getCom1Active() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom1Active"));
        }
        void CXBusServiceProxy::getCom1ActiveAsync(int *o_com1Active)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom1Active"), setterCallback(o_com1Active));
        }

        int CXBusServiceProxy::getCom1Standby() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom1Standby"));
        }
        void CXBusServiceProxy::getCom1StandbyAsync(int *o_com1Standby)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom1Standby"), setterCallback(o_com1Standby));
        }

        int CXBusServiceProxy::getCom2Active() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom2Active"));
        }
        void CXBusServiceProxy::getCom2ActiveAsync(int *o_com2Active)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom2Active"), setterCallback(o_com2Active));
        }

        int CXBusServiceProxy::getCom2Standby() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom2Standby"));
        }
        void CXBusServiceProxy::getCom2StandbyAsync(int *o_com2Standby)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom2Standby"), setterCallback(o_com2Standby));
        }

        int CXBusServiceProxy::getTransponderCode() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getTransponderCode"));
        }
        void CXBusServiceProxy::getTransponderCodeAsync(int *o_xpdrCode)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTransponderCode"), setterCallback(o_xpdrCode));
        }

        int CXBusServiceProxy::getTransponderMode() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getTransponderMode"));
        }
        void CXBusServiceProxy::getTransponderModeAsync(int *o_xpdrMode)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTransponderMode"), setterCallback(o_xpdrMode));
        }

        bool CXBusServiceProxy::getTransponderIdent() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getTransponderIdent"));
        }
        void CXBusServiceProxy::getTransponderIdentAsync(bool *o_ident)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTransponderIdent"), setterCallback(o_ident));
        }

        bool CXBusServiceProxy::getBeaconLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getBeaconLightsOn"));
        }

        void CXBusServiceProxy::getBeaconLightsOnAsync(bool *o_beaconLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getBeaconLightsOn"), setterCallback(o_beaconLightsOn));
        }

        bool CXBusServiceProxy::getLandingLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getLandingLightsOn"));
        }

        void CXBusServiceProxy::getLandingLightsOnAsync(bool *o_landingLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLandingLightsOn"), setterCallback(o_landingLightsOn));
        }

        bool CXBusServiceProxy::getNavLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getNavLightsOn"));
        }

        void CXBusServiceProxy::getNavLightsOnAsync(bool *o_navLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getNavLightsOn"), setterCallback(o_navLightsOn));
        }

        bool CXBusServiceProxy::getStrobeLightsOn() const

        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getStrobeLightsOn"));
        }

        void CXBusServiceProxy::getStrobeLightsOnAsync(bool *o_strobeLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getStrobeLightsOn"), setterCallback(o_strobeLightsOn));
        }

        bool CXBusServiceProxy::getTaxiLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getTaxiLightsOn"));
        }

        void CXBusServiceProxy::getTaxiLightsOnAsync(bool *o_taxiLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTaxiLightsOn"), setterCallback(o_taxiLightsOn));
        }


        void CXBusServiceProxy::setCom1Active(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom1Active"), freq);
        }

        void CXBusServiceProxy::setCom1Standby(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom1Standby"), freq);
        }

        void CXBusServiceProxy::setCom2Active(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom2Active"), freq);
        }

        void CXBusServiceProxy::setCom2Standby(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom2Standby"), freq);
        }

        void CXBusServiceProxy::setTransponderCode(int code)
        {
            m_dbusInterface->callDBus(QLatin1String("setTransponderCode"), code);
        }

        void CXBusServiceProxy::setTransponderMode(int mode)
        {
            m_dbusInterface->callDBus(QLatin1String("setTransponderMode"), mode);
        }
        double CXBusServiceProxy::getFlapsDeployRatio() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getFlapsDeployRatio"));
        }

        void CXBusServiceProxy::getFlapsDeployRatioAsync(double *o_flapsDeployRatio)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getFlapsDeployRatio"), setterCallback(o_flapsDeployRatio));
        }

        double CXBusServiceProxy::getGearDeployRatio() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getGearDeployRatio"));
        }

        void CXBusServiceProxy::getGearDeployRatioAsync(double *o_gearDeployRatio)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getGearDeployRatio"), setterCallback(o_gearDeployRatio));
        }

        int CXBusServiceProxy::getNumberOfEngines() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getNumberOfEngines"));
        }

        void CXBusServiceProxy::getNumberOfEnginesAsync(double *o_numberOfEngines)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getNumberOfEngines"), setterCallback(o_numberOfEngines));
        }

        QList<double> CXBusServiceProxy::getEngineN1Percentage() const
        {
            return m_dbusInterface->callDBusRet<QList<double>>(QLatin1String("getEngineN1Percentage"));
        }

        void CXBusServiceProxy::getEngineN1PercentageAsync(QList<double> *o_engineN1Percentage)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getEngineN1Percentage"), setterCallback(o_engineN1Percentage));
        }

        double CXBusServiceProxy::getSpeedBrakeRatio() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getSpeedBrakeRatio"));
        }

        void CXBusServiceProxy::getSpeedBrakeRatioAsync(double *o_speedBrakeRatio)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getSpeedBrakeRatio"), setterCallback(o_speedBrakeRatio));
        }
    }
}
