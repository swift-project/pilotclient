/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_XSWIFTBUS_SERVICE_PROXY_H
#define BLACKSIMPLUGIN_XSWIFTBUS_SERVICE_PROXY_H

#include "blackmisc/genericdbusinterface.h"

#include <QDBusError>
#include <QDBusPendingReply>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <functional>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;
class QDBusPendingCallWatcher;

//! \cond PRIVATE
#define XSWIFTBUS_SERVICE_INTERFACENAME "org.swift_project.xswiftbus.service"
#define XSWIFTBUS_SERVICE_OBJECTPATH "/xswiftbus/service"
//! \endcond

namespace BlackSimPlugin
{
    namespace XPlane
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
            BlackMisc::CGenericDBusInterface *m_dbusInterface = nullptr;

            // Returns a function object which can be passed to CGenericDBusInterface::callDBusAsync.
            template <typename T>
            std::function<void(QDBusPendingCallWatcher *)> setterCallback(T *obj)
            {
                return [this, obj](QDBusPendingCallWatcher * watcher)
                {
                    QDBusPendingReply<T> reply = *watcher;
                    if (reply.isError()) { emit this->asyncMethodError(reply.error()); }
                    else { *obj = reply; }
                    watcher->deleteLater();
                };
            }

        signals:
            //! Emitted if an asynchronous method call caused a DBus error
            BLACK_NO_RELAY void asyncMethodError(QDBusError error);

            //! Own aircraft model changed
            void aircraftModelChanged(
                const QString &path, const QString &filename, const QString &livery,
                const QString &icao, const QString &modelString, const QString &name, const QString &description);

            //! Airports in range are updated
            void airportsInRangeUpdated(const QStringList &icaoCodes, const QStringList &names, const QList<double> &lats, const QList<double> &lons, const QList<double> &alts);

        public slots:
            //! Get XSwiftBus version number
            QString getVersionNumber();

            //! Get own aircraft situation data
            void getOwnAircraftSituationData(BlackSimPlugin::XPlane::XPlaneData *o_xplaneData);

            //! \copydoc XSwiftBus::CService::addTextMessage
            void addTextMessage(const QString &text);

            //! \copydoc XSwiftBus::CService::getAircraftModelPath
            //! @{
            QString getAircraftModelPath() const;
            void getAircraftModelPathAsync(QString *o_modelPath);
            //! @}

            //! \copydoc XSwiftBus::CService::getAircraftModelFilename
            //! @{
            QString getAircraftModelFilename() const;
            void getAircraftModelFilenameAsync(QString *o_modelFilename);
            //! @}

            //! \copydoc XSwiftBus::CService::getAircraftLivery
            //! @{
            QString getAircraftLivery() const;
            void getAircraftLiveryAsync(QString *o_modelLivery);
            //! @}

            //! \copydoc XSwiftBus::CService::getAircraftIcaoCode
            //! @{
            QString getAircraftIcaoCode() const;
            void getAircraftIcaoCodeAsync(QString *o_icaoCode);
            //! @}

            //! \copydoc XSwiftBus::CService::getAircraftDescription
            //! @{
            QString getAircraftDescription() const;
            void getAircraftDescriptionAsync(QString *o_description);
            //! @}

            //! \copydoc XSwiftBus::CService::getAircraftModelString
            //! @{
            QString getAircraftModelString() const;
            void getAircraftModelStringAsync(QString *o_modelString);
            //! @}

            //! \copydoc XSwiftBus::CService::getAircraftName
            //! @{
            QString getAircraftName() const;
            void getAircraftNameAsync(QString *o_name);
            //! @}

            //! \copydoc XSwiftBus::CService::getXPlaneInstallationPath
            //! @{
            QString getXPlaneInstallationPath() const;
            void getXPlaneInstallationPathAsync(QString *o_installPath);
            //! @}

            //! \copydoc XSwiftBus::CService::getXPlanePreferencesPath
            //! @{
            QString getXPlanePreferencesPath() const;
            void getXPlanePreferencesPathAsync(QString *o_prefsPath);
            //! @}

            //! \copydoc XSwiftBus::CService::isPaused
            //! @{
            bool isPaused() const;
            void isPausedAsync(bool *o_paused);
            //! @}

            //! \copydoc XSwiftBus::CService::getLatitudeDeg
            //! @{
            double getLatitudeDeg() const;
            void getLatitudeDegAsync(double *o_latitude);
            //! @}

            //! \copydoc XSwiftBus::CService::getLongitudeDeg
            //! @{
            double getLongitudeDeg() const;
            void getLongitudeDegAsync(double *o_longitude);
            //! @}

            //! \copydoc XSwiftBus::CService::getAltitudeMslFt
            //! @{
            double getAltitudeMslFt() const;
            void getAltitudeMslFtAsync(double *o_altitude);
            //! @}

            //! \copydoc XSwiftBus::CService::getHeightAglFt
            //! @{
            double getHeightAglFt() const;
            void getHeightAglFtAsync(double *o_height);
            //! @}

            //! \copydoc XSwiftBus::CService::getGroundSpeedKts
            //! @{
            double getGroundSpeedKts() const;
            void getGroundSpeedKtsAsync(double *o_groundspeed);
            //! @}

            //! \copydoc XSwiftBus::CService::getPitchDeg
            //! @{
            double getPitchDeg() const;
            void getPitchDegAsync(double *o_pitch);
            //! @}

            //! \copydoc XSwiftBus::CService::getRollDeg
            //! @{
            double getRollDeg() const;
            void getRollDegAsync(double *o_roll);
            //! @}

            //! Get whether all wheels are on the ground
            //! @{
            bool getAllWheelsOnGround() const;
            void getAllWheelsOnGroundAsync(bool *o_allWheels);
            //! @}

            //! \copydoc XSwiftBus::CService::getCom1ActiveKhz
            //! @{
            int getCom1ActiveKhz() const;
            void getCom1ActiveKhzAsync(int *o_com1Active);
            //! @}

            //! \copydoc XSwiftBus::CService::getCom1StandbyKhz
            //! @{
            int getCom1StandbyKhz() const;
            void getCom1StandbyKhzAsync(int *o_com1Standby);
            //! @}

            //! \copydoc XSwiftBus::CService::getCom2ActiveKhz
            //! @{
            int getCom2ActiveKhz() const;
            void getCom2ActiveKhzAsync(int *o_com2Active);
            //! @}

            //! \copydoc XSwiftBus::CService::getCom2StandbyKhz
            //! @{
            int getCom2StandbyKhz() const;
            void getCom2StandbyKhzAsync(int *o_com2Standby);
            //! @}

            //! \copydoc XSwiftBus::CService::getTransponderCode
            //! @{
            int getTransponderCode() const;
            void getTransponderCodeAsync(int *o_xpdrCode);
            //! @}

            //! \copydoc XSwiftBus::CService::getTransponderMode
            //! @{
            int getTransponderMode() const;
            void getTransponderModeAsync(int *o_xpdrMode);
            //! @}

            //! \copydoc XSwiftBus::CService::getTransponderIdent
            //! @{
            bool getTransponderIdent() const;
            void getTransponderIdentAsync(bool *o_ident);
            //! @}

            //! \copydoc XSwiftBus::CService::getLandingLightsOn
            //! @{
            bool getBeaconLightsOn() const;
            void getBeaconLightsOnAsync(bool *o_beaconLightsOn);
            //! @}

            //! \copydoc XSwiftBus::CService::getLandingLightsOn
            //! @{
            bool getLandingLightsOn() const;
            void getLandingLightsOnAsync(bool *o_landingLightsOn);
            //! @}

            //! \copydoc XSwiftBus::CService::getNavLightsOn
            //! @{
            bool getNavLightsOn() const;
            void getNavLightsOnAsync(bool *o_navLightsOn);
            //! @}

            //! \copydoc XSwiftBus::CService::getStrobeLightsOn
            //! @{
            bool getStrobeLightsOn() const;
            void getStrobeLightsOnAsync(bool *o_strobeLightsOn);
            //! @}

            //! \copydoc XSwiftBus::CService::getTaxiLightsOn
            //! @{
            bool getTaxiLightsOn() const;
            void getTaxiLightsOnAsync(bool *o_taxiLightsOn);
            //! @}

            //! \copydoc XSwiftBus::CService::getQNHInHg
            //! @{
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

            //! \copydoc XSwiftBus::CService::getFlapsDeployRatio

            //! @{
            double getFlapsDeployRatio() const;
            void getFlapsDeployRatioAsync(double *o_flapsDeployRatio);
            //! @}

            //! \copydoc XSwiftBus::CService::getGearDeployRatio
            //! @{
            double getGearDeployRatio() const;
            void getGearDeployRatioAsync(double *o_gearDeployRatio);
            //! @}

            //! \copydoc XSwiftBus::CService::getEngineN1Percentage
            //! @{
            QList<double> getEngineN1Percentage() const;
            void getEngineN1PercentageAsync(QList<double> *o_engineN1Percentage);
            //! @}

            //! \copydoc XSwiftBus::CService::getSpeedBrakeRatio
            //! @{
            double getSpeedBrakeRatio() const;
            void getSpeedBrakeRatioAsync(double *o_speedBrakeRatio);
            //! @}
        };
    }
}

#endif // guard
