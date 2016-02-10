/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIMPLUGIN_XBUS_SERVICE_PROXY_H
#define BLACKSIMPLUGIN_XBUS_SERVICE_PROXY_H

//! \file

#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/sequence.h"
#include <functional>

//! \cond PRIVATE
#define XBUS_SERVICE_INTERFACENAME "org.swift_project.xbus.service"
#define XBUS_SERVICE_OBJECTPATH "/xbus/service"
//! \endcond

namespace BlackSimPlugin
{
    namespace XPlane
    {

        /*!
         * Proxy object connected to a real XBus::CService object via DBus
         */
        class CXBusServiceProxy : public QObject
        {
            Q_OBJECT

        public:
            //! Service name
            static const QString &InterfaceName()
            {
                static QString s(XBUS_SERVICE_INTERFACENAME);
                return s;
            }

            //! Service path
            static const QString &ObjectPath()
            {
                static QString s(XBUS_SERVICE_OBJECTPATH);
                return s;
            }

            //! Constructor
            CXBusServiceProxy(QDBusConnection &connection, QObject *parent = nullptr, bool dummy = false);

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
                return [this, obj] (QDBusPendingCallWatcher *watcher)
                {
                    QDBusPendingReply<T> reply = *watcher;
                    if (reply.isError()) { emit asyncMethodError(reply.error()); }
                    else { *obj = reply; }
                };
            }

        signals:
            //! Emitted if an asynchronous method call caused a DBus error
            void asyncMethodError(QDBusError error);

            //! \copydoc XBus::CService::aircraftModelChanged
            void aircraftModelChanged(const QString &path, const QString &filename, const QString &livery, const QString &icao);

            //! \copydoc XBus::CService::airportsInRangeUpdated
            void airportsInRangeUpdated(const QStringList &icaoCodes, const QStringList &names, const QList<double> &lats, const QList<double> &lons, const QList<double> &alts);

        public slots:
            //! \copydoc XBus::CService::addTextMessage
            void addTextMessage(const QString &text, double red, double green, double blue);

            //! \copydoc XBus::CService::updateAirportsInRange
            void updateAirportsInRange();

            //! \copydoc XBus::CService::getAircraftModelPath
            //! @{
            QString getAircraftModelPath() const;
            void getAircraftModelPathAsync(QString *o_modelPath);
            //! @}

            //! \copydoc XBus::CService::getAircraftModelFilename
            //! @{
            QString getAircraftModelFilename() const;
            void getAircraftModelFilenameAsync(QString *o_modelFilename);
            //! @}

            //! \copydoc XBus::CService::getAircraftLivery
            //! @{
            QString getAircraftLivery() const;
            void getAircraftLiveryAsync(QString *o_modelLivery);
            //! @}

            //! \copydoc XBus::CService::getAircraftIcaoCode
            //! @{
            QString getAircraftIcaoCode() const;
            void getAircraftIcaoCodeAsync(QString *o_icaoCode);
            //! @}

            //! \copydoc XBus::CService::getXPlaneVersionMajor
            //! @{
            int getXPlaneVersionMajor() const;
            void getXPlaneVersionMajorAsync(int *o_versionMajor);
            //! @}

            //! \copydoc XBus::CService::getXPlaneVersionMinor
            //! @{
            int getXPlaneVersionMinor() const;
            void getXPlaneVersionMinorAsync(int *o_versionMinor);
            //! @}

            //! \copydoc XBus::CService::getXPlaneInstallationPath
            //! @{
            QString getXPlaneInstallationPath() const;
            void getXPlaneInstallationPathAsync(QString *o_installPath);
            //! @}

            //! \copydoc XBus::CService::getXPlanePreferencesPath
            //! @{
            QString getXPlanePreferencesPath() const;
            void getXPlanePreferencesPathAsync(QString *o_prefsPath);
            //! @}

            //! \copydoc XBus::CService::isPaused
            //! @{
            bool isPaused() const;
            void isPausedAsync(bool *o_paused);
            //! @}

            //! \copydoc XBus::CService::isUsingRealTime
            //! @{
            bool isUsingRealTime() const;
            void isUsingRealTimeAsync(bool *o_isRealTime);
            //! @}

            //! \copydoc XBus::CService::getLatitude
            //! @{
            double getLatitude() const;
            void getLatitudeAsync(double *o_latitude);
            //! @}

            //! \copydoc XBus::CService::getLongitude
            //! @{
            double getLongitude() const;
            void getLongitudeAsync(double *o_longitude);
            //! @}

            //! \copydoc XBus::CService::getAltitudeMSL
            //! @{
            double getAltitudeMSL() const;
            void getAltitudeMSLAsync(double *o_altitude);
            //! @}

            //! \copydoc XBus::CService::getHeightAGL
            //! @{
            double getHeightAGL() const;
            void getHeightAGLAsync(double *o_height);
            //! @}

            //! \copydoc XBus::CService::getGroundSpeed
            //! @{
            double getGroundSpeed() const;
            void getGroundSpeedAsync(double *o_groundspeed);
            //! @}

            //! \copydoc XBus::CService::getIndicatedAirspeed
            //! @{
            double getIndicatedAirspeed() const;
            void getIndicatedAirspeedAsync(double *o_ias);
            //! @}

            //! \copydoc XBus::CService::getTrueAirspeed
            //! @{
            double getTrueAirspeed() const;
            void getTrueAirspeedAsync(double *o_tas);
            //! @}

            //! \copydoc XBus::CService::getPitch
            //! @{
            double getPitch() const;
            void getPitchAsync(double *o_pitch);
            //! @}

            //! \copydoc XBus::CService::getRoll
            //! @{
            double getRoll() const;
            void getRollAsync(double *o_roll);
            //! @}

            //! \copydoc XBus::CService::getTrueHeading
            //! @{
            double getTrueHeading() const;
            void getTrueHeadingAsync(double *o_heading);
            //! @}

            //! Get whether any wheel is on the ground
            //! @{
            bool getAnyWheelOnGround() const;
            void getAnyWheelOnGroundAsync(bool *o_anyWheel);
            //! @}

            //! Get whether all wheels are on the ground
            //! @{
            bool getAllWheelsOnGround() const;
            void getAllWheelsOnGroundAsync(bool *o_allWheels);
            //! @}

            //! \copydoc XBus::CService::getCom1Active
            //! @{
            int getCom1Active() const;
            void getCom1ActiveAsync(int *o_com1Active);
            //! @}

            //! \copydoc XBus::CService::getCom1Standby
            //! @{
            int getCom1Standby() const;
            void getCom1StandbyAsync(int *o_com1Standby);
            //! @}

            //! \copydoc XBus::CService::getCom2Active
            //! @{
            int getCom2Active() const;
            void getCom2ActiveAsync(int *o_com2Active);
            //! @}

            //! \copydoc XBus::CService::getCom2Standby
            //! @{
            int getCom2Standby() const;
            void getCom2StandbyAsync(int *o_com2Standby);
            //! @}

            //! \copydoc XBus::CService::getTransponderCode
            //! @{
            int getTransponderCode() const;
            void getTransponderCodeAsync(int *o_xpdrCode);
            //! @}

            //! \copydoc XBus::CService::getTransponderMode
            //! @{
            int getTransponderMode() const;
            void getTransponderModeAsync(int *o_xpdrMode);
            //! @}

            //! \copydoc XBus::CService::getTransponderIdent
            //! @{
            bool getTransponderIdent() const;
            void getTransponderIdentAsync(bool *o_ident);
            //! @}

            //! \copydoc XBus::CService::getLandingLightsOn
            //! @{
            bool getBeaconLightsOn() const;
            void getBeaconLightsOnAsync(bool *o_beaconLightsOn);
            //! @}

            //! \copydoc XBus::CService::getLandingLightsOn
            //! @{
            bool getLandingLightsOn() const;
            void getLandingLightsOnAsync(bool *o_landingLightsOn);
            //! @}

            //! \copydoc XBus::CService::getNavLightsOn
            //! @{
            bool getNavLightsOn() const;
            void getNavLightsOnAsync(bool *o_navLightsOn);
            //! @}

            //! \copydoc XBus::CService::getStrobeLightsOn
            //! @{
            bool getStrobeLightsOn() const;
            void getStrobeLightsOnAsync(bool *o_strobeLightsOn);
            //! @}

            //! \copydoc XBus::CService::getTaxiLightsOn

            //! @{
            bool getTaxiLightsOn() const;
            void getTaxiLightsOnAsync(bool *o_taxiLightsOn);
            //! @}

            //! \copydoc XBus::CService::setCom1Active
            void setCom1Active(int freq);

            //! \copydoc XBus::CService::setCom1Standby
            void setCom1Standby(int freq);

            //! \copydoc XBus::CService::setCom2Active
            void setCom2Active(int freq);

            //! \copydoc XBus::CService::setCom2Standby
            void setCom2Standby(int freq);

            //! \copydoc XBus::CService::setTransponderCode
            void setTransponderCode(int code);

            //! \copydoc XBus::CService::setTransponderMode
            void setTransponderMode(int mode);

            //! \copydoc XBus::CService::getFlapsDeployRatio

            //! @{
            double getFlapsDeployRatio() const;
            void getFlapsDeployRatioAsync(double *o_flapsDeployRatio);
            //! @}

            //! \copydoc XBus::CService::getGearDeployRatio
            //! @{
            double getGearDeployRatio() const;
            void getGearDeployRatioAsync(double *o_gearDeployRatio);
            //! @}

            //! \copydoc XBus::CService::getNumberOfEngines

            //! @{
            int getNumberOfEngines() const;
            void getNumberOfEnginesAsync(double *o_numberOfEngines);
            //! @}

            //! \copydoc XBus::CService::getEngineN1Percentage
            //! @{
            QList<double> getEngineN1Percentage() const;
            void getEngineN1PercentageAsync(QList<double> *o_engineN1Percentage);
            //! @}

            //! \copydoc XBus::CService::getSpeedBrakeRatio
            //! @{
            double getSpeedBrakeRatio() const;
            void getSpeedBrakeRatioAsync(double *o_speedBrakeRatio);
            //! @}

        };

    }
}

#endif // guard
