/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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
                static QString s(XSWIFTBUS_SERVICE_INTERFACENAME);
                return s;
            }

            //! Service path
            static const QString &ObjectPath()
            {
                static QString s(XSWIFTBUS_SERVICE_OBJECTPATH);
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
                return [this, obj] (QDBusPendingCallWatcher *watcher)
                {
                    QDBusPendingReply<T> reply = *watcher;
                    if (reply.isError()) { emit asyncMethodError(reply.error()); }
                    else { *obj = reply; }
                    watcher->deleteLater();
                };
            }

        signals:
            //! Emitted if an asynchronous method call caused a DBus error
            void asyncMethodError(QDBusError error);

            //! \copydoc XSwiftBus::CService::aircraftModelChanged
            void aircraftModelChanged(const QString &path, const QString &filename, const QString &livery,
                const QString &icao, const QString &modelString, const QString &name, const QString &description);

            //! \copydoc XSwiftBus::CService::airportsInRangeUpdated
            void airportsInRangeUpdated(const QStringList &icaoCodes, const QStringList &names, const QList<double> &lats, const QList<double> &lons, const QList<double> &alts);

        public slots:
            //! \copydoc XSwiftBus::CService::addTextMessage
            void addTextMessage(const QString &text, double red, double green, double blue);

            //! \copydoc XSwiftBus::CService::updateAirportsInRange
            void updateAirportsInRange();

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

            //! \copydoc XSwiftBus::CService::getXPlaneVersionMajor
            //! @{
            int getXPlaneVersionMajor() const;
            void getXPlaneVersionMajorAsync(int *o_versionMajor);
            //! @}

            //! \copydoc XSwiftBus::CService::getXPlaneVersionMinor
            //! @{
            int getXPlaneVersionMinor() const;
            void getXPlaneVersionMinorAsync(int *o_versionMinor);
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

            //! \copydoc XSwiftBus::CService::isUsingRealTime
            //! @{
            bool isUsingRealTime() const;
            void isUsingRealTimeAsync(bool *o_isRealTime);
            //! @}

            //! \copydoc XSwiftBus::CService::getLatitude
            //! @{
            double getLatitude() const;
            void getLatitudeAsync(double *o_latitude);
            //! @}

            //! \copydoc XSwiftBus::CService::getLongitude
            //! @{
            double getLongitude() const;
            void getLongitudeAsync(double *o_longitude);
            //! @}

            //! \copydoc XSwiftBus::CService::getAltitudeMSL
            //! @{
            double getAltitudeMSL() const;
            void getAltitudeMSLAsync(double *o_altitude);
            //! @}

            //! \copydoc XSwiftBus::CService::getHeightAGL
            //! @{
            double getHeightAGL() const;
            void getHeightAGLAsync(double *o_height);
            //! @}

            //! \copydoc XSwiftBus::CService::getGroundSpeed
            //! @{
            double getGroundSpeed() const;
            void getGroundSpeedAsync(double *o_groundspeed);
            //! @}

            //! \copydoc XSwiftBus::CService::getIndicatedAirspeed
            //! @{
            double getIndicatedAirspeed() const;
            void getIndicatedAirspeedAsync(double *o_ias);
            //! @}

            //! \copydoc XSwiftBus::CService::getTrueAirspeed
            //! @{
            double getTrueAirspeed() const;
            void getTrueAirspeedAsync(double *o_tas);
            //! @}

            //! \copydoc XSwiftBus::CService::getPitch
            //! @{
            double getPitch() const;
            void getPitchAsync(double *o_pitch);
            //! @}

            //! \copydoc XSwiftBus::CService::getRoll
            //! @{
            double getRoll() const;
            void getRollAsync(double *o_roll);
            //! @}

            //! \copydoc XSwiftBus::CService::getTrueHeading
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

            //! \copydoc XSwiftBus::CService::getCom1Active
            //! @{
            int getCom1Active() const;
            void getCom1ActiveAsync(int *o_com1Active);
            //! @}

            //! \copydoc XSwiftBus::CService::getCom1Standby
            //! @{
            int getCom1Standby() const;
            void getCom1StandbyAsync(int *o_com1Standby);
            //! @}

            //! \copydoc XSwiftBus::CService::getCom2Active
            //! @{
            int getCom2Active() const;
            void getCom2ActiveAsync(int *o_com2Active);
            //! @}

            //! \copydoc XSwiftBus::CService::getCom2Standby
            //! @{
            int getCom2Standby() const;
            void getCom2StandbyAsync(int *o_com2Standby);
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

            //! \copydoc XSwiftBus::CService::getQNH
            //! @{
            double getQNH() const;
            void getQNHAsync(double *o_qnh);
            //! @}

            //! \copydoc XSwiftBus::CService::setCom1Active
            void setCom1Active(int freq);

            //! \copydoc XSwiftBus::CService::setCom1Standby
            void setCom1Standby(int freq);

            //! \copydoc XSwiftBus::CService::setCom2Active
            void setCom2Active(int freq);

            //! \copydoc XSwiftBus::CService::setCom2Standby
            void setCom2Standby(int freq);

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

            //! \copydoc XSwiftBus::CService::getNumberOfEngines

            //! @{
            int getNumberOfEngines() const;
            void getNumberOfEnginesAsync(double *o_numberOfEngines);
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
