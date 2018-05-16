/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "xswiftbusserviceproxy.h"
#include "simulatorxplane.h"

#include <QLatin1String>
#include <QPointer>

class QDBusConnection;

#define XSWIFTBUS_SERVICE_SERVICENAME "org.swift-project.xswiftbus"

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CXSwiftBusServiceProxy::CXSwiftBusServiceProxy(QDBusConnection &connection, QObject *parent, bool dummy) : QObject(parent)
        {
            m_dbusInterface = new BlackMisc::CGenericDBusInterface(XSWIFTBUS_SERVICE_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
            if (!dummy) { m_dbusInterface->relayParentSignals(); }
        }

        void CXSwiftBusServiceProxy::getOwnAircraftSituationData(XPlaneData *o_xplaneData)
        {
            QPointer<CXSwiftBusServiceProxy> myself(this);
            std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
            {
                if (!myself) { return; }
                QDBusPendingReply<double, double, double, double, double, double, double, double> reply = *watcher;
                if (!reply.isError())
                {
                    o_xplaneData->latitude = reply.argumentAt<0>();
                    o_xplaneData->longitude = reply.argumentAt<1>();
                    o_xplaneData->altitude = reply.argumentAt<2>();
                    o_xplaneData->groundspeed = reply.argumentAt<3>();
                    o_xplaneData->pitch = reply.argumentAt<4>();
                    o_xplaneData->roll = reply.argumentAt<5>();
                    o_xplaneData->trueHeading = reply.argumentAt<6>();
                    o_xplaneData->seaLeveLPressure = reply.argumentAt<7>();
                }
                watcher->deleteLater();
            };
            m_dbusInterface->callDBusAsync(QLatin1String("getOwnAircraftSituationData"), callback);
        }

        void CXSwiftBusServiceProxy::addTextMessage(const QString &text, double red, double green, double blue)
        {
            m_dbusInterface->callDBus(QLatin1String("addTextMessage"), text, red, green, blue);
        }

        void CXSwiftBusServiceProxy::updateAirportsInRange()
        {
            m_dbusInterface->callDBus(QLatin1String("updateAirportsInRange"));
        }

        QString CXSwiftBusServiceProxy::getAircraftModelPath() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelPath"));
        }
        void CXSwiftBusServiceProxy::getAircraftModelPathAsync(QString *o_modelPath)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftModelPath"), setterCallback(o_modelPath));
        }

        QString CXSwiftBusServiceProxy::getAircraftModelFilename() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelFilename"));
        }
        void CXSwiftBusServiceProxy::getAircraftModelFilenameAsync(QString *o_modelFilename)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftModelFilename"), setterCallback(o_modelFilename));
        }

        QString CXSwiftBusServiceProxy::getAircraftLivery() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftLivery"));
        }
        void CXSwiftBusServiceProxy::getAircraftLiveryAsync(QString *o_modelLivery)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftLivery"), setterCallback(o_modelLivery));
        }

        QString CXSwiftBusServiceProxy::getAircraftIcaoCode() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftIcaoCode"));
        }
        void CXSwiftBusServiceProxy::getAircraftIcaoCodeAsync(QString *o_icaoCode)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftIcaoCode"), setterCallback(o_icaoCode));
        }

        QString CXSwiftBusServiceProxy::getAircraftDescription() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftDescription"));
        }
        void CXSwiftBusServiceProxy::getAircraftDescriptionAsync(QString *o_description)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftDescription"), setterCallback(o_description));
        }

        QString CXSwiftBusServiceProxy::getAircraftModelString() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelString"));
        }
        void CXSwiftBusServiceProxy::getAircraftModelStringAsync(QString *o_modelString)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftModelString"), setterCallback(o_modelString));
        }

        QString CXSwiftBusServiceProxy::getAircraftName() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftName"));
        }
        void CXSwiftBusServiceProxy::getAircraftNameAsync(QString *o_name)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAircraftName"), setterCallback(o_name));
        }

        int CXSwiftBusServiceProxy::getXPlaneVersionMajor() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getXPlaneVersionMajor"));
        }
        void CXSwiftBusServiceProxy::getXPlaneVersionMajorAsync(int *o_versionMajor)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getXPlaneVersionMajor"), setterCallback(o_versionMajor));
        }

        int CXSwiftBusServiceProxy::getXPlaneVersionMinor() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getXPlaneVersionMinor"));
        }
        void CXSwiftBusServiceProxy::getXPlaneVersionMinorAsync(int *o_versionMinor)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getXPlaneVersionMinor"), setterCallback(o_versionMinor));
        }

        QString CXSwiftBusServiceProxy::getXPlaneInstallationPath() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getXPlaneInstallationPath"));
        }
        void CXSwiftBusServiceProxy::getXPlaneInstallationPathAsync(QString *o_installPath)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getXPlaneInstallationPath"), setterCallback(o_installPath));
        }

        QString CXSwiftBusServiceProxy::getXPlanePreferencesPath() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getXPlanePreferencesPath"));
        }
        void CXSwiftBusServiceProxy::getXPlanePreferencesPathAsync(QString *o_prefsPath)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getXPlanePreferencesPath"), setterCallback(o_prefsPath));
        }

        bool CXSwiftBusServiceProxy::isPaused() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("isPaused"));
        }
        void CXSwiftBusServiceProxy::isPausedAsync(bool *o_paused)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("isPaused"), setterCallback(o_paused));
        }

        bool CXSwiftBusServiceProxy::isUsingRealTime() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("isUsingRealTime"));
        }
        void CXSwiftBusServiceProxy::isUsingRealTimeAsync(bool *o_isRealTime)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("isUsingRealTime"), setterCallback(o_isRealTime));
        }

        double CXSwiftBusServiceProxy::getLatitude() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getLatitude"));
        }
        void CXSwiftBusServiceProxy::getLatitudeAsync(double *o_latitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLatitude"), setterCallback(o_latitude));
        }

        double CXSwiftBusServiceProxy::getLongitude() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getLongitude"));
        }
        void CXSwiftBusServiceProxy::getLongitudeAsync(double *o_longitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLongitude"), setterCallback(o_longitude));
        }

        double CXSwiftBusServiceProxy::getAltitudeMSL() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getAltitudeMSL"));
        }
        void CXSwiftBusServiceProxy::getAltitudeMSLAsync(double *o_altitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAltitudeMSL"), setterCallback(o_altitude));
        }

        double CXSwiftBusServiceProxy::getHeightAGL() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getHeightAGL"));
        }
        void CXSwiftBusServiceProxy::getHeightAGLAsync(double *o_height)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getHeightAGL"), setterCallback(o_height));
        }

        double CXSwiftBusServiceProxy::getGroundSpeed() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getGroundSpeed"));
        }
        void CXSwiftBusServiceProxy::getGroundSpeedAsync(double *o_groundspeed)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getGroundSpeed"), setterCallback(o_groundspeed));
        }

        double CXSwiftBusServiceProxy::getIndicatedAirspeed() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getIndicatedAirspeed"));
        }
        void CXSwiftBusServiceProxy::getIndicatedAirspeedAsync(double *o_ias)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getIndicatedAirspeed"), setterCallback(o_ias));
        }

        double CXSwiftBusServiceProxy::getTrueAirspeed() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getTrueAirspeed"));
        }
        void CXSwiftBusServiceProxy::getTrueAirspeedAsync(double *o_tas)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTrueAirspeed"), setterCallback(o_tas));
        }

        double CXSwiftBusServiceProxy::getPitch() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getPitch"));
        }
        void CXSwiftBusServiceProxy::getPitchAsync(double *o_pitch)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getPitch"), setterCallback(o_pitch));
        }

        double CXSwiftBusServiceProxy::getRoll() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getRoll"));
        }
        void CXSwiftBusServiceProxy::getRollAsync(double *o_roll)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getRoll"), setterCallback(o_roll));
        }

        double CXSwiftBusServiceProxy::getTrueHeading() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getTrueHeading"));
        }
        void CXSwiftBusServiceProxy::getTrueHeadingAsync(double *o_heading)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTrueHeading"), setterCallback(o_heading));
        }

        bool CXSwiftBusServiceProxy::getAnyWheelOnGround() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getAnyWheelOnGround"));
        }
        void CXSwiftBusServiceProxy::getAnyWheelOnGroundAsync(bool *o_anyWheel)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAnyWheelOnGround"), setterCallback(o_anyWheel));
        }

        bool CXSwiftBusServiceProxy::getAllWheelsOnGround() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getAllWheelsOnGround"));
        }
        void CXSwiftBusServiceProxy::getAllWheelsOnGroundAsync(bool *o_allWheels)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAllWheelsOnGround"), setterCallback(o_allWheels));
        }

        int CXSwiftBusServiceProxy::getCom1Active() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom1Active"));
        }
        void CXSwiftBusServiceProxy::getCom1ActiveAsync(int *o_com1Active)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom1Active"), setterCallback(o_com1Active));
        }

        int CXSwiftBusServiceProxy::getCom1Standby() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom1Standby"));
        }
        void CXSwiftBusServiceProxy::getCom1StandbyAsync(int *o_com1Standby)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom1Standby"), setterCallback(o_com1Standby));
        }

        int CXSwiftBusServiceProxy::getCom2Active() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom2Active"));
        }
        void CXSwiftBusServiceProxy::getCom2ActiveAsync(int *o_com2Active)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom2Active"), setterCallback(o_com2Active));
        }

        int CXSwiftBusServiceProxy::getCom2Standby() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom2Standby"));
        }
        void CXSwiftBusServiceProxy::getCom2StandbyAsync(int *o_com2Standby)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom2Standby"), setterCallback(o_com2Standby));
        }

        int CXSwiftBusServiceProxy::getTransponderCode() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getTransponderCode"));
        }
        void CXSwiftBusServiceProxy::getTransponderCodeAsync(int *o_xpdrCode)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTransponderCode"), setterCallback(o_xpdrCode));
        }

        int CXSwiftBusServiceProxy::getTransponderMode() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getTransponderMode"));
        }
        void CXSwiftBusServiceProxy::getTransponderModeAsync(int *o_xpdrMode)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTransponderMode"), setterCallback(o_xpdrMode));
        }

        bool CXSwiftBusServiceProxy::getTransponderIdent() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getTransponderIdent"));
        }
        void CXSwiftBusServiceProxy::getTransponderIdentAsync(bool *o_ident)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTransponderIdent"), setterCallback(o_ident));
        }

        bool CXSwiftBusServiceProxy::getBeaconLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getBeaconLightsOn"));
        }

        void CXSwiftBusServiceProxy::getBeaconLightsOnAsync(bool *o_beaconLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getBeaconLightsOn"), setterCallback(o_beaconLightsOn));
        }

        bool CXSwiftBusServiceProxy::getLandingLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getLandingLightsOn"));
        }

        void CXSwiftBusServiceProxy::getLandingLightsOnAsync(bool *o_landingLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLandingLightsOn"), setterCallback(o_landingLightsOn));
        }

        bool CXSwiftBusServiceProxy::getNavLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getNavLightsOn"));
        }

        void CXSwiftBusServiceProxy::getNavLightsOnAsync(bool *o_navLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getNavLightsOn"), setterCallback(o_navLightsOn));
        }

        bool CXSwiftBusServiceProxy::getStrobeLightsOn() const

        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getStrobeLightsOn"));
        }

        void CXSwiftBusServiceProxy::getStrobeLightsOnAsync(bool *o_strobeLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getStrobeLightsOn"), setterCallback(o_strobeLightsOn));
        }

        bool CXSwiftBusServiceProxy::getTaxiLightsOn() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getTaxiLightsOn"));
        }

        void CXSwiftBusServiceProxy::getTaxiLightsOnAsync(bool *o_taxiLightsOn)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getTaxiLightsOn"), setterCallback(o_taxiLightsOn));
        }


        double CXSwiftBusServiceProxy::getQNH() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getQNH"));
        }

        void CXSwiftBusServiceProxy::getQNHAsync(double *o_qnh)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getQNH"), setterCallback(o_qnh));
        }


        void CXSwiftBusServiceProxy::setCom1Active(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom1Active"), freq);
        }

        void CXSwiftBusServiceProxy::setCom1Standby(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom1Standby"), freq);
        }

        void CXSwiftBusServiceProxy::setCom2Active(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom2Active"), freq);
        }

        void CXSwiftBusServiceProxy::setCom2Standby(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom2Standby"), freq);
        }

        void CXSwiftBusServiceProxy::setTransponderCode(int code)
        {
            m_dbusInterface->callDBus(QLatin1String("setTransponderCode"), code);
        }

        void CXSwiftBusServiceProxy::setTransponderMode(int mode)
        {
            m_dbusInterface->callDBus(QLatin1String("setTransponderMode"), mode);
        }
        double CXSwiftBusServiceProxy::getFlapsDeployRatio() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getFlapsDeployRatio"));
        }

        void CXSwiftBusServiceProxy::getFlapsDeployRatioAsync(double *o_flapsDeployRatio)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getFlapsDeployRatio"), setterCallback(o_flapsDeployRatio));
        }

        double CXSwiftBusServiceProxy::getGearDeployRatio() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getGearDeployRatio"));
        }

        void CXSwiftBusServiceProxy::getGearDeployRatioAsync(double *o_gearDeployRatio)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getGearDeployRatio"), setterCallback(o_gearDeployRatio));
        }

        int CXSwiftBusServiceProxy::getNumberOfEngines() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getNumberOfEngines"));
        }

        void CXSwiftBusServiceProxy::getNumberOfEnginesAsync(double *o_numberOfEngines)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getNumberOfEngines"), setterCallback(o_numberOfEngines));
        }

        QList<double> CXSwiftBusServiceProxy::getEngineN1Percentage() const
        {
            return m_dbusInterface->callDBusRet<QList<double>>(QLatin1String("getEngineN1Percentage"));
        }

        void CXSwiftBusServiceProxy::getEngineN1PercentageAsync(QList<double> *o_engineN1Percentage)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getEngineN1Percentage"), setterCallback(o_engineN1Percentage));
        }

        double CXSwiftBusServiceProxy::getSpeedBrakeRatio() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getSpeedBrakeRatio"));
        }

        void CXSwiftBusServiceProxy::getSpeedBrakeRatioAsync(double *o_speedBrakeRatio)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getSpeedBrakeRatio"), setterCallback(o_speedBrakeRatio));
        }
    } // ns
} // ns
