/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "xswiftbusserviceproxy.h"
#include "simulatorxplane.h"

#include <QLatin1String>
#include <QPointer>

class QDBusConnection;

#define XSWIFTBUS_SERVICE_SERVICENAME "org.swift-project.xswiftbus"

namespace BlackSimPlugin::XPlane
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

            s = connection.connect(QString(), "/xswiftbus/service", "org.swift_project.xswiftbus.service",
                                    "sceneryLoaded", this,
                                    SIGNAL(sceneryLoaded()));
            Q_ASSERT(s);
        }
    }

    QString CXSwiftBusServiceProxy::getVersionNumber()
    {
        return m_dbusInterface->callDBusRet<QString>(QLatin1String("getVersionNumber"));
    }

    QString CXSwiftBusServiceProxy::getCommitHash()
    {
        return m_dbusInterface->callDBusRet<QString>(QLatin1String("getCommitHash"));
    }

    void CXSwiftBusServiceProxy::getOwnAircraftSituationDataAsync(XPlaneData *o_xplaneData)
    {
        if (!o_xplaneData) { return; }
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

    void CXSwiftBusServiceProxy::getOwnAircraftCom1DataAsync(XPlaneData *o_xplaneData)
    {
        if (!o_xplaneData) { return; }
        QPointer<CXSwiftBusServiceProxy> myself(this);
        std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
        {
            if (!myself) { return; }
            QDBusPendingReply<int, int, double, bool, bool> reply = *watcher;
            if (!reply.isError())
            {
                o_xplaneData->com1ActiveKhz = reply.argumentAt<0>();
                o_xplaneData->com1StandbyKhz = reply.argumentAt<1>();
                o_xplaneData->com1Volume = reply.argumentAt<2>();
                o_xplaneData->isCom1Receiving = reply.argumentAt<3>();
                o_xplaneData->isCom1Transmitting = reply.argumentAt<4>();
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getOwnAircraftCom1Data"), callback);
    }

    void CXSwiftBusServiceProxy::getOwnAircraftCom2DataAsync(XPlaneData *o_xplaneData)
    {
        if (!o_xplaneData) { return; }
        QPointer<CXSwiftBusServiceProxy> myself(this);
        std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
        {
            if (!myself) { return; }
            QDBusPendingReply<int, int, double, bool, bool> reply = *watcher;
            if (!reply.isError())
            {
                o_xplaneData->com2ActiveKhz = reply.argumentAt<0>();
                o_xplaneData->com2StandbyKhz = reply.argumentAt<1>();
                o_xplaneData->com2Volume = reply.argumentAt<2>();
                o_xplaneData->isCom2Receiving = reply.argumentAt<3>();
                o_xplaneData->isCom2Transmitting = reply.argumentAt<4>();
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getOwnAircraftCom2Data"), callback);
    }

    void CXSwiftBusServiceProxy::getOwnAircraftXpdrAsync(XPlaneData *o_xplaneData)
    {
        if (!o_xplaneData) { return; }
        QPointer<CXSwiftBusServiceProxy> myself(this);
        std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
        {
            if (!myself) { return; }
            QDBusPendingReply<int, int, bool> reply = *watcher;
            if (!reply.isError())
            {
                o_xplaneData->xpdrCode  = reply.argumentAt<0>();
                o_xplaneData->xpdrMode  = reply.argumentAt<1>();
                o_xplaneData->xpdrIdent = reply.argumentAt<2>();
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getOwnAircraftXpdr"), callback);
    }

    void CXSwiftBusServiceProxy::getOwnAircraftLightsAsync(XPlaneData *o_xplaneData)
    {
        if (!o_xplaneData) { return; }
        QPointer<CXSwiftBusServiceProxy> myself(this);
        std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
        {
            if (!myself) { return; }
            QDBusPendingReply<bool, bool, bool, bool, bool> reply = *watcher;
            if (!reply.isError())
            {
                o_xplaneData->beaconLightsOn   = reply.argumentAt<0>();
                o_xplaneData->landingLightsOn  = reply.argumentAt<1>();
                o_xplaneData->navLightsOn      = reply.argumentAt<2>();
                o_xplaneData->strobeLightsOn   = reply.argumentAt<3>();
                o_xplaneData->taxiLightsOn     = reply.argumentAt<4>();
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getOwnAircraftLights"), callback);
    }

    void CXSwiftBusServiceProxy::getOwnAircraftPartsAsync(XPlaneData *o_xplaneData)
    {
        if (!o_xplaneData) { return; }
        QPointer<CXSwiftBusServiceProxy> myself(this);
        std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
        {
            if (!myself) { return; }
            QDBusPendingReply<double, double, double, QList<double> > reply = *watcher;
            if (!reply.isError())
            {
                o_xplaneData->flapsDeployRatio = reply.argumentAt<0>();
                o_xplaneData->gearDeployRatio  = reply.argumentAt<1>();
                o_xplaneData->speedBrakeRatio  = reply.argumentAt<2>();
                o_xplaneData->enginesN1Percentage = reply.argumentAt<3>();
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getOwnAircraftParts"), callback);
    }

    void CXSwiftBusServiceProxy::getOwnAircraftModelDataAsync(XPlaneData *o_xplaneData)
    {
        if (!o_xplaneData) { return; }
        QPointer<CXSwiftBusServiceProxy> myself(this);
        std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
        {
            if (!myself) { return; }
            QDBusPendingReply<QString, QString> reply = *watcher;
            if (!reply.isError())
            {
                o_xplaneData->aircraftModelPath = reply.argumentAt<0>(); // this is NOT the model string
                o_xplaneData->aircraftIcaoCode  = reply.argumentAt<1>();
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getOwnAircraftModelData"), callback);
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

    void CXSwiftBusServiceProxy::getFrameStats(double *o_averageFps, double *o_simTimeRatio, double *o_trackMilesShort, double *o_minutesLate) const
    {
        std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
        {
            QDBusPendingReply<double, double, double, double> reply = *watcher;
            if (!reply.isError())
            {
                *o_averageFps = reply.argumentAt<0>();
                *o_simTimeRatio = reply.argumentAt<1>();
                *o_trackMilesShort = reply.argumentAt<2>();
                *o_minutesLate = reply.argumentAt<3>();
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getFrameStats"), callback)->waitForFinished();
    }

    void CXSwiftBusServiceProxy::getFrameStatsAsync(double *o_averageFps, double *o_simTimeRatio, double *o_trackMilesShort, double *o_minutesLate)
    {
        std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
        {
            QDBusPendingReply<double, double, double, double> reply = *watcher;
            if (!reply.isError())
            {
                *o_averageFps = reply.argumentAt<0>();
                *o_simTimeRatio = reply.argumentAt<1>();
                *o_trackMilesShort = reply.argumentAt<2>();
                *o_minutesLate = reply.argumentAt<3>();
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getFrameStats"), callback);
    }

    void CXSwiftBusServiceProxy::resetFrameTotals()
    {
        m_dbusInterface->callDBus(QLatin1String("resetFrameTotals"));
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

    double CXSwiftBusServiceProxy::getAltitudeMslM() const
    {
        return m_dbusInterface->callDBusRet<double>(QLatin1String("getAltitudeMslM"));
    }
    void CXSwiftBusServiceProxy::getAltitudeMslMAsync(double *o_altitude)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("getAltitudeMslM"), setterCallback(o_altitude));
    }

    double CXSwiftBusServiceProxy::getHeightAglM() const
    {
        return m_dbusInterface->callDBusRet<double>(QLatin1String("getHeightAglM"));
    }
    void CXSwiftBusServiceProxy::getHeightAglMAsync(double *o_height)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("getHeightAglM"), setterCallback(o_height));
    }

    double CXSwiftBusServiceProxy::getGroundSpeedMps() const
    {
        return m_dbusInterface->callDBusRet<double>(QLatin1String("getGroundSpeedMps"));
    }
    void CXSwiftBusServiceProxy::getGroundSpeedMpsAsync(double *o_groundspeed)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("getGroundSpeedMps"), setterCallback(o_groundspeed));
    }

    double CXSwiftBusServiceProxy::getIndicatedAirspeedKias() const
    {
        return m_dbusInterface->callDBusRet<double>(QLatin1String("getIndicatedAirspeedKias"));
    }
    void CXSwiftBusServiceProxy::getIndicatedAirspeedKiasAsync(double *o_ias)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("getIndicatedAirspeedKias"), setterCallback(o_ias));
    }

    double CXSwiftBusServiceProxy::getTrueAirspeedKias() const
    {
        return m_dbusInterface->callDBusRet<double>(QLatin1String("getTrueAirspeedKias"));
    }
    void CXSwiftBusServiceProxy::getTrueAirspeedKiasAsync(double *o_tas)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("getTrueAirspeedKias"), setterCallback(o_tas));
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

    double CXSwiftBusServiceProxy::getTrueHeadingDeg() const
    {
        return m_dbusInterface->callDBusRet<double>(QLatin1String("getTrueHeadingDeg"));
    }
    void CXSwiftBusServiceProxy::getTrueHeadingDegAsync(double *o_heading)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("getTrueHeadingDeg"), setterCallback(o_heading));
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

    bool CXSwiftBusServiceProxy::isCom1Receiving() const
    {
        return m_dbusInterface->callDBusRet<bool>(QLatin1String("isCom1Receiving"));
    }

    void CXSwiftBusServiceProxy::isCom1ReceivingAsync(bool *o_com1Rec)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("isCom1Receiving"), setterCallback(o_com1Rec));
    }

    bool CXSwiftBusServiceProxy::isCom1Transmitting() const
    {
        return m_dbusInterface->callDBusRet<bool>(QLatin1String("isCom1Transmitting"));
    }

    void CXSwiftBusServiceProxy::isCom1TransmittingAsync(bool *o_com1Tx)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("isCom1Transmitting"), setterCallback(o_com1Tx));
    }

    double CXSwiftBusServiceProxy::getCom1Volume() const
    {
        return m_dbusInterface->callDBusRet<double>(QLatin1String("getCom1Volume"));
    }

    void CXSwiftBusServiceProxy::getCom1VolumeAsync(double *o_com1Volume)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("getCom1Volume"), setterCallback(o_com1Volume));
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

    bool CXSwiftBusServiceProxy::isCom2Receiving() const
    {
        return m_dbusInterface->callDBusRet<bool>(QLatin1String("isCom2Receiving"));
    }

    void CXSwiftBusServiceProxy::isCom2ReceivingAsync(bool *o_com2Rec)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("isCom2Receiving"), setterCallback(o_com2Rec));
    }

    bool CXSwiftBusServiceProxy::isCom2Transmitting() const
    {
        return m_dbusInterface->callDBusRet<bool>(QLatin1String("isCom2Transmitting"));
    }

    void CXSwiftBusServiceProxy::isCom2TransmittingAsync(bool *o_com2Tx)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("isCom2Transmitting"), setterCallback(o_com2Tx));
    }

    double CXSwiftBusServiceProxy::getCom2Volume() const
    {
        return m_dbusInterface->callDBusRet<double>(QLatin1String("getCom2Volume"));
    }

    void CXSwiftBusServiceProxy::getCom2VolumeAsync(double *o_com2Volume)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("getCom2Volume"), setterCallback(o_com2Volume));
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

    QString CXSwiftBusServiceProxy::getSettingsJson() const
    {
        return m_dbusInterface->callDBusRet<QString>(QLatin1String("getSettingsJson"));
    }

    void CXSwiftBusServiceProxy::getSettingsJsonAsync(QString *o_jsonSettings)
    {
        m_dbusInterface->callDBusAsync(QLatin1String("getSettingsJson"), setterCallback(o_jsonSettings));
    }

    void CXSwiftBusServiceProxy::setSettingsJson(const QString &json)
    {
        m_dbusInterface->callDBus(QLatin1String("setSettingsJson"), json);
    }
} // ns
