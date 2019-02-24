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

#define XSWIFTBUS_SERVICE_SERVICENAME "org.swift-project.xswiftbus"

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CXSwiftBusServiceProxy::CXSwiftBusServiceProxy(QDBusConnection &connection, QObject *parent, bool dummy) : QObject(parent)
        {
            m_dbusInterface = new BlackMisc::CGenericDBusInterface(XSWIFTBUS_SERVICE_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
            if (!dummy)
            {
                bool s;
                s = connection.connect(QString(), "/xswiftbus/service", "org.swift_project.xswiftbus.service",
                                       "aircraftModelChanged", this,
                                       SIGNAL(aircraftModelChanged(QString, QString, QString, QString, QString, QString, QString)));
                Q_ASSERT(s);

                s = connection.connect(QString(), "/xswiftbus/service", "org.swift_project.xswiftbus.service",
                                       "airportsInRangeUpdated", this,
                                       SIGNAL(airportsInRangeUpdated(QStringList, QStringList, QList<double>, QList<double>, QList<double>)));
                Q_ASSERT(s);
            }
        }

        QString CXSwiftBusServiceProxy::getVersionNumber()
        {
             return m_dbusInterface->callDBusRet<QString>(QLatin1String("getVersionNumber"));
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
                    o_xplaneData->latitudeDeg = reply.argumentAt<0>();
                    o_xplaneData->longitudeDeg = reply.argumentAt<1>();
                    o_xplaneData->altitudeM = reply.argumentAt<2>();
                    o_xplaneData->groundspeedMs = reply.argumentAt<3>();
                    o_xplaneData->pitchDeg = reply.argumentAt<4>();
                    o_xplaneData->rollDeg = reply.argumentAt<5>();
                    o_xplaneData->trueHeadingDeg = reply.argumentAt<6>();
                    o_xplaneData->seaLevelPressureInHg = reply.argumentAt<7>();
                }
                watcher->deleteLater();
            };
            m_dbusInterface->callDBusAsync(QLatin1String("getOwnAircraftSituationData"), callback);
        }

        void CXSwiftBusServiceProxy::addTextMessage(const QString &text)
        {
            m_dbusInterface->callDBus(QLatin1String("addTextMessage"), text);
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

        double CXSwiftBusServiceProxy::getLatitudeDeg() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getLatitudeDeg"));
        }
        void CXSwiftBusServiceProxy::getLatitudeDegAsync(double *o_latitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLatitudeDeg"), setterCallback(o_latitude));
        }

        double CXSwiftBusServiceProxy::getLongitudeDeg() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getLongitudeDeg"));
        }
        void CXSwiftBusServiceProxy::getLongitudeDegAsync(double *o_longitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getLongitudeDeg"), setterCallback(o_longitude));
        }

        double CXSwiftBusServiceProxy::getAltitudeMslFt() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getAltitudeMslFt"));
        }
        void CXSwiftBusServiceProxy::getAltitudeMslFtAsync(double *o_altitude)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAltitudeMslFt"), setterCallback(o_altitude));
        }

        double CXSwiftBusServiceProxy::getHeightAglFt() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getHeightAglFt"));
        }
        void CXSwiftBusServiceProxy::getHeightAglFtAsync(double *o_height)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getHeightAglFt"), setterCallback(o_height));
        }

        double CXSwiftBusServiceProxy::getGroundSpeedKts() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getGroundSpeedKts"));
        }
        void CXSwiftBusServiceProxy::getGroundSpeedKtsAsync(double *o_groundspeed)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getGroundSpeedKts"), setterCallback(o_groundspeed));
        }

        double CXSwiftBusServiceProxy::getPitchDeg() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getPitchDeg"));
        }
        void CXSwiftBusServiceProxy::getPitchDegAsync(double *o_pitch)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getPitchDeg"), setterCallback(o_pitch));
        }

        double CXSwiftBusServiceProxy::getRollDeg() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getRollDeg"));
        }
        void CXSwiftBusServiceProxy::getRollDegAsync(double *o_roll)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getRollDeg"), setterCallback(o_roll));
        }

        bool CXSwiftBusServiceProxy::getAllWheelsOnGround() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("getAllWheelsOnGround"));
        }
        void CXSwiftBusServiceProxy::getAllWheelsOnGroundAsync(bool *o_allWheels)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getAllWheelsOnGround"), setterCallback(o_allWheels));
        }

        int CXSwiftBusServiceProxy::getCom1ActiveKhz() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom1ActiveKhz"));
        }
        void CXSwiftBusServiceProxy::getCom1ActiveKhzAsync(int *o_com1Active)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom1ActiveKhz"), setterCallback(o_com1Active));
        }

        int CXSwiftBusServiceProxy::getCom1StandbyKhz() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom1StandbyKhz"));
        }
        void CXSwiftBusServiceProxy::getCom1StandbyKhzAsync(int *o_com1Standby)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom1StandbyKhz"), setterCallback(o_com1Standby));
        }

        int CXSwiftBusServiceProxy::getCom2ActiveKhz() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom2ActiveKhz"));
        }
        void CXSwiftBusServiceProxy::getCom2ActiveKhzAsync(int *o_com2Active)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom2ActiveKhz"), setterCallback(o_com2Active));
        }

        int CXSwiftBusServiceProxy::getCom2StandbyKhz() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getCom2StandbyKhz"));
        }
        void CXSwiftBusServiceProxy::getCom2StandbyKhzAsync(int *o_com2Standby)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getCom2StandbyKhz"), setterCallback(o_com2Standby));
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


        double CXSwiftBusServiceProxy::getQNHInHg() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getQNHInHg"));
        }

        void CXSwiftBusServiceProxy::getQNHInHgAsync(double *o_qnh)
        {
            m_dbusInterface->callDBusAsync(QLatin1String("getQNHInHg"), setterCallback(o_qnh));
        }


        void CXSwiftBusServiceProxy::setCom1ActiveKhz(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom1ActiveKhz"), freq);
        }

        void CXSwiftBusServiceProxy::setCom1StandbyKhz(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom1StandbyKhz"), freq);
        }

        void CXSwiftBusServiceProxy::setCom2ActiveKhz(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom2ActiveKhz"), freq);
        }

        void CXSwiftBusServiceProxy::setCom2StandbyKhz(int freq)
        {
            m_dbusInterface->callDBus(QLatin1String("setCom2StandbyKhz"), freq);
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
