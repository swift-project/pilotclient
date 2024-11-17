// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_XSWIFTBUS_SERVICE_PROXY_H
#define SWIFT_SIMPLUGIN_XSWIFTBUS_SERVICE_PROXY_H

#include <functional>

#include <QDBusError>
#include <QDBusPendingReply>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#include "misc/genericdbusinterface.h"

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;
class QDBusPendingCallWatcher;

//! \cond PRIVATE
#define XSWIFTBUS_SERVICE_INTERFACENAME "org.swift_project.xswiftbus.service"
#define XSWIFTBUS_SERVICE_OBJECTPATH "/xswiftbus/service"
//! \endcond

namespace swift::simplugin::xplane
{
    struct XPlaneData;

    /*!
     * Proxy object connected to a real XSwiftBus::CService object via DBus
     */
    class CXSwiftBusServiceProxy : public QObject
    {
        Q_OBJECT

    public:
        //! Service name
        static const QString &InterfaceName()
        {
            static const QString s(XSWIFTBUS_SERVICE_INTERFACENAME);
            return s;
        }

        //! Service path
        static const QString &ObjectPath()
        {
            static const QString s(XSWIFTBUS_SERVICE_OBJECTPATH);
            return s;
        }

        //! Constructor
        CXSwiftBusServiceProxy(QDBusConnection &connection, QObject *parent = nullptr, bool dummy = false);

        //! Does the remote object exist?
        bool isValid() const { return m_dbusInterface->isValid(); }

        //! Cancel all current async slot calls
        void cancelAllPendingAsyncCalls() { m_dbusInterface->cancelAllPendingAsyncCalls(); }

    private:
        swift::misc::CGenericDBusInterface *m_dbusInterface = nullptr;

        // Returns a function object which can be passed to CGenericDBusInterface::callDBusAsync.
        template <typename T>
        std::function<void(QDBusPendingCallWatcher *)> setterCallback(T *obj)
        {
            return [this, obj](QDBusPendingCallWatcher *watcher) {
                QDBusPendingReply<T> reply = *watcher;
                if (reply.isError()) { emit this->asyncMethodError(reply.error()); }
                else { *obj = reply; }
                watcher->deleteLater();
            };
        }

    signals:
        //! Emitted if an asynchronous method call caused a DBus error
        SWIFT_NO_RELAY void asyncMethodError(QDBusError error);

        //! Own aircraft model changed
        void aircraftModelChanged(const QString &path, const QString &filename, const QString &livery,
                                  const QString &icao, const QString &modelString, const QString &name,
                                  const QString &description);

        //! Airports in range are updated
        void airportsInRangeUpdated(const QStringList &icaoCodes, const QStringList &names, const QList<double> &lats,
                                    const QList<double> &lons, const QList<double> &alts);

        //! Scenery was loaded
        void sceneryLoaded();

    public slots:
        //! Get xswiftbus version number
        QString getVersionNumber();

        //! Get SHA1 of the last commit that could influence xswiftbus
        QString getCommitHash();

        //! Get own aircraft situation data
        void getOwnAircraftSituationDataAsync(swift::simplugin::xplane::XPlaneData *o_xplaneData);

        //! Get own aircraft velocity data
        void getOwnAircraftVelocityDataAsync(swift::simplugin::xplane::XPlaneData *o_xplaneData);

        //! Get own aircraft COM1 data
        void getOwnAircraftCom1DataAsync(swift::simplugin::xplane::XPlaneData *o_xplaneData);

        //! Get own aircraft COM2 data
        void getOwnAircraftCom2DataAsync(swift::simplugin::xplane::XPlaneData *o_xplaneData);

        //! Get own XPDR data
        void getOwnAircraftXpdrAsync(swift::simplugin::xplane::XPlaneData *o_xplaneData);

        //! Get own lights data
        void getOwnAircraftLightsAsync(swift::simplugin::xplane::XPlaneData *o_xplaneData);

        //! Get own parts such as gear, flaps
        void getOwnAircraftPartsAsync(swift::simplugin::xplane::XPlaneData *o_xplaneData);

        //! Get own model data
        void getOwnAircraftModelDataAsync(swift::simplugin::xplane::XPlaneData *o_xplaneData);

        //! \copydoc XSwiftBus::CService::addTextMessage
        void addTextMessage(const QString &text, double red, double green, double blue);

        //! \copydoc XSwiftBus::CService::updateAirportsInRange
        void updateAirportsInRange();

        //! @{
        //! \copydoc XSwiftBus::CService::getAircraftModelPath
        QString getAircraftModelPath() const;
        void getAircraftModelPathAsync(QString *o_modelPath);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getAircraftModelFilename
        QString getAircraftModelFilename() const;
        void getAircraftModelFilenameAsync(QString *o_modelFilename);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getAircraftLivery
        QString getAircraftLivery() const;
        void getAircraftLiveryAsync(QString *o_modelLivery);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getAircraftIcaoCode
        QString getAircraftIcaoCode() const;
        void getAircraftIcaoCodeAsync(QString *o_icaoCode);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getAircraftDescription
        QString getAircraftDescription() const;
        void getAircraftDescriptionAsync(QString *o_description);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getAircraftModelString
        QString getAircraftModelString() const;
        void getAircraftModelStringAsync(QString *o_modelString);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getAircraftName
        QString getAircraftName() const;
        void getAircraftNameAsync(QString *o_name);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getXPlaneVersionMajor
        int getXPlaneVersionMajor() const;
        void getXPlaneVersionMajorAsync(int *o_versionMajor);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getXPlaneVersionMinor
        int getXPlaneVersionMinor() const;
        void getXPlaneVersionMinorAsync(int *o_versionMinor);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getXPlaneInstallationPath
        QString getXPlaneInstallationPath() const;
        void getXPlaneInstallationPathAsync(QString *o_installPath);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getXPlanePreferencesPath
        QString getXPlanePreferencesPath() const;
        void getXPlanePreferencesPathAsync(QString *o_prefsPath);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::isPaused
        bool isPaused() const;
        void isPausedAsync(bool *o_paused);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::isUsingRealTime
        bool isUsingRealTime() const;
        void isUsingRealTimeAsync(bool *o_isRealTime);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getFrameStats
        void getFrameStats(double *o_averageFps, double *o_simTimeRatio, double *o_trackMilesShort,
                           double *o_minutesLate) const;
        void getFrameStatsAsync(double *o_averageFps, double *o_simTimeRatio, double *o_trackMilesShort,
                                double *o_minutesLate);
        //! @}

        //! \copydoc XSwiftBus::CService::resetFrameTotals
        void resetFrameTotals();

        //! \copydoc XSwiftBus::CService::setFlightNetworkConnected
        void setFlightNetworkConnected(bool connected);

        //! @{
        //! \copydoc XSwiftBus::CService::getLatitudeDeg
        double getLatitudeDeg() const;
        void getLatitudeDegAsync(double *o_latitude);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getLongitudeDeg
        double getLongitudeDeg() const;
        void getLongitudeDegAsync(double *o_longitude);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getAltitudeMslM
        double getAltitudeMslM() const;
        void getAltitudeMslMAsync(double *o_altitude);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getPressureAltitudeFt
        double getPressureAltitudeFt() const;
        void getPressureAltitudeFtAsync(double *o_altitude);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getHeightAglM
        double getHeightAglM() const;
        void getHeightAglMAsync(double *o_height);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getGroundSpeedMps
        double getGroundSpeedMps() const;
        void getGroundSpeedMpsAsync(double *o_groundspeed);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getIndicatedAirspeedKias
        double getIndicatedAirspeedKias() const;
        void getIndicatedAirspeedKiasAsync(double *o_ias);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getTrueAirspeedKias
        double getTrueAirspeedKias() const;
        void getTrueAirspeedKiasAsync(double *o_tas);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getPitchDeg
        double getPitchDeg() const;
        void getPitchDegAsync(double *o_pitch);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getRollDeg
        double getRollDeg() const;
        void getRollDegAsync(double *o_roll);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getTrueHeadingDeg
        double getTrueHeadingDeg() const;
        void getTrueHeadingDegAsync(double *o_heading);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getLocalXVelocityMps
        double getLocalXVelocityMps() const;
        void getLocalXVelocityMpsAsync(double *o_velocity);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getLocalYVelocityMps
        double getLocalYVelocityMps() const;
        void getLocalYVelocityMpsAsync(double *o_velocity);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getLocalZVelocityMps
        double getLocalZVelocityMps() const;
        void getLocalZVelocityMpsAsync(double *o_velocity);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getPitchRadPerSec
        double getPitchRadPerSec() const;
        void getPitchRadPerSecAsync(double *o_radPerSec);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getRollRadPerSec
        double getRollRadPerSec() const;
        void getRollRadPerSecAsync(double *o_radPerSec);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getHeadingRadPerSec
        double getHeadingRadPerSec() const;
        void getHeadingRadPerSecAsync(double *o_radPerSec);
        //! @}

        //! @{
        //! Get whether any wheel is on the ground
        bool getAnyWheelOnGround() const;
        void getAnyWheelOnGroundAsync(bool *o_anyWheel);
        //! @}

        //! @{
        //! Get whether all wheels are on the ground
        bool getAllWheelsOnGround() const;
        void getAllWheelsOnGroundAsync(bool *o_allWheels);
        //! @}

        //! @{
        //! Get elevation of ground under the plane (in meters)
        double getGroundElevation() const;
        void getGroundElevationAsync(double *o_elevationM);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getCom1ActiveKhz
        int getCom1ActiveKhz() const;
        void getCom1ActiveKhzAsync(int *o_com1Active);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getCom1StandbyKhz
        int getCom1StandbyKhz() const;
        void getCom1StandbyKhzAsync(int *o_com1Standby);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::isCom1Receiving
        bool isCom1Receiving() const;
        void isCom1ReceivingAsync(bool *o_com1Rec);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::isCom1Transmitting
        bool isCom1Transmitting() const;
        void isCom1TransmittingAsync(bool *o_com1Tx);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getCom1Volume
        double getCom1Volume() const;
        void getCom1VolumeAsync(double *o_com1Volume);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getCom2ActiveKhz
        int getCom2ActiveKhz() const;
        void getCom2ActiveKhzAsync(int *o_com2Active);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getCom2StandbyKhz
        int getCom2StandbyKhz() const;
        void getCom2StandbyKhzAsync(int *o_com2Standby);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::isCom2Receiving
        bool isCom2Receiving() const;
        void isCom2ReceivingAsync(bool *o_com2Rec);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::isCom2Transmitting
        bool isCom2Transmitting() const;
        void isCom2TransmittingAsync(bool *o_com2Tx);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getCom2Volume
        double getCom2Volume() const;
        void getCom2VolumeAsync(double *o_com2Volume);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getTransponderCode
        int getTransponderCode() const;
        void getTransponderCodeAsync(int *o_xpdrCode);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getTransponderMode
        int getTransponderMode() const;
        void getTransponderModeAsync(int *o_xpdrMode);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getTransponderIdent
        bool getTransponderIdent() const;
        void getTransponderIdentAsync(bool *o_ident);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getLandingLightsOn
        bool getBeaconLightsOn() const;
        void getBeaconLightsOnAsync(bool *o_beaconLightsOn);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getLandingLightsOn
        bool getLandingLightsOn() const;
        void getLandingLightsOnAsync(bool *o_landingLightsOn);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getNavLightsOn
        bool getNavLightsOn() const;
        void getNavLightsOnAsync(bool *o_navLightsOn);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getStrobeLightsOn
        bool getStrobeLightsOn() const;
        void getStrobeLightsOnAsync(bool *o_strobeLightsOn);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getTaxiLightsOn
        bool getTaxiLightsOn() const;
        void getTaxiLightsOnAsync(bool *o_taxiLightsOn);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getQNHInHg
        double getQNHInHg() const;
        void getQNHInHgAsync(double *o_qnh);
        //! @}

        //! \copydoc XSwiftBus::CService::setCom1ActiveKhz
        void setCom1ActiveKhz(int freq);

        //! \copydoc XSwiftBus::CService::setCom1StandbyKhz
        void setCom1StandbyKhz(int freq);

        //! \copydoc XSwiftBus::CService::setCom2ActiveKhz
        void setCom2ActiveKhz(int freq);

        //! \copydoc XSwiftBus::CService::setCom2StandbyKhz
        void setCom2StandbyKhz(int freq);

        //! \copydoc XSwiftBus::CService::setTransponderCode
        void setTransponderCode(int code);

        //! \copydoc XSwiftBus::CService::setTransponderMode
        void setTransponderMode(int mode);

        //! @{
        //! \copydoc XSwiftBus::CService::getFlapsDeployRatio
        double getFlapsDeployRatio() const;
        void getFlapsDeployRatioAsync(double *o_flapsDeployRatio);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getGearDeployRatio
        double getGearDeployRatio() const;
        void getGearDeployRatioAsync(double *o_gearDeployRatio);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getNumberOfEngines
        int getNumberOfEngines() const;
        void getNumberOfEnginesAsync(double *o_numberOfEngines);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getEngineN1Percentage
        QList<double> getEngineN1Percentage() const;
        void getEngineN1PercentageAsync(QList<double> *o_engineN1Percentage);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CService::getSpeedBrakeRatio
        double getSpeedBrakeRatio() const;
        void getSpeedBrakeRatioAsync(double *o_speedBrakeRatio);
        //! @}

        //! @{
        //! \copydoc XSwiftBus::CSettingsProvider::getSettings
        QString getSettingsJson() const;
        void getSettingsJsonAsync(QString *o_jsonSettings);
        //! @}

        //! \copydoc XSwiftBus::CSettingsProvider::setSettings
        void setSettingsJson(const QString &json);
    };
} // namespace swift::simplugin::xplane

#endif // guard
