/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FGSWIFTBUS_SERVICE_PROXY_H
#define BLACKSIMPLUGIN_FGSWIFTBUS_SERVICE_PROXY_H

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
#define FGSWIFTBUS_SERVICE_INTERFACENAME "org.swift_project.fgswiftbus.service"
#define FGSWIFTBUS_SERVICE_OBJECTPATH "/fgswiftbus/service"
//! \endcond

namespace BlackSimPlugin
{
    namespace Flightgear
    {
        struct FlightgearData;

        /*!
         * Proxy object connected to a real FGSwiftBus::CService object via DBus
         */
        class CFGSwiftBusServiceProxy : public QObject
        {
            Q_OBJECT

        public:
            //! Service name
            static const QString &InterfaceName()
            {
                static const QString s(FGSWIFTBUS_SERVICE_INTERFACENAME);
                return s;
            }

            //! Service path
            static const QString &ObjectPath()
            {
                static const QString s(FGSWIFTBUS_SERVICE_OBJECTPATH);
                return s;
            }

            //! Constructor
            CFGSwiftBusServiceProxy(QDBusConnection &connection, QObject *parent = nullptr, bool dummy = false);

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
            //! Get FGSwiftBus version number
            QString getVersionNumber();

            //! Get own aircraft situation data
            void getOwnAircraftSituationData(BlackSimPlugin::Flightgear::FlightgearData *o_flightgearData);

            //! \copydoc FGSwiftBus::CService::addTextMessage
            void addTextMessage(const QString &text);

            //! \copydoc FGSwiftBus::CService::getAircraftModelPath
            //! @{
            QString getAircraftModelPath() const;
            void getAircraftModelPathAsync(QString *o_modelPath);
            //! @}

            //! \copydoc FGSwiftBus::CService::getAircraftModelFilename
            //! @{
            QString getAircraftModelFilename() const;
            void getAircraftModelFilenameAsync(QString *o_modelFilename);
            //! @}

            //! \copydoc FGSwiftBus::CService::getAircraftLivery
            //! @{
            QString getAircraftLivery() const;
            void getAircraftLiveryAsync(QString *o_modelLivery);
            //! @}

            //! \copydoc FGSwiftBus::CService::getAircraftIcaoCode
            //! @{
            QString getAircraftIcaoCode() const;
            void getAircraftIcaoCodeAsync(QString *o_icaoCode);
            //! @}

            //! \copydoc FGSwiftBus::CService::getAircraftDescription
            //! @{
            QString getAircraftDescription() const;
            void getAircraftDescriptionAsync(QString *o_description);
            //! @}

            //! \copydoc FGSwiftBus::CService::getAircraftModelString
            //! @{
            QString getAircraftModelString() const;
            void getAircraftModelStringAsync(QString *o_modelString);
            //! @}

            //! \copydoc FGSwiftBus::CService::getAircraftName
            //! @{
            QString getAircraftName() const;
            void getAircraftNameAsync(QString *o_name);
            //! @}

            //! \copydoc FGSwiftBus::CService::isPaused
            //! @{
            bool isPaused() const;
            void isPausedAsync(bool *o_paused);
            //! @}

            //! \copydoc FGSwiftBus::CService::getLatitudeDeg
            //! @{
            double getLatitudeDeg() const;
            void getLatitudeDegAsync(double *o_latitude);
            //! @}

            //! \copydoc FGSwiftBus::CService::getLongitudeDeg
            //! @{
            double getLongitudeDeg() const;
            void getLongitudeDegAsync(double *o_longitude);
            //! @}

            //! \copydoc FGSwiftBus::CService::getAltitudeMslFt
            //! @{
            double getAltitudeMslFt() const;
            void getAltitudeMslFtAsync(double *o_altitude);
            //! @}

            //! \copydoc FGSwiftBus::CService::getHeightAglFt
            //! @{
            double getHeightAglFt() const;
            void getHeightAglFtAsync(double *o_height);
            //! @}

            //! \copydoc FGSwiftBus::CService::getGroundSpeedKts
            //! @{
            double getGroundSpeedKts() const;
            void getGroundSpeedKtsAsync(double *o_groundspeed);
            //! @}

            //! \copydoc FGSwiftBus::CService::getPitchDeg
            //! @{
            double getPitchDeg() const;
            void getPitchDegAsync(double *o_pitch);
            //! @}

            //! \copydoc FGSwiftBus::CService::getRollDeg
            //! @{
            double getRollDeg() const;
            void getRollDegAsync(double *o_roll);
            //! @}

            //! Get whether all wheels are on the ground
            //! @{
            bool getAllWheelsOnGround() const;
            void getAllWheelsOnGroundAsync(bool *o_allWheels);
            //! @}

            //! \copydoc FGSwiftBus::CService::getCom1ActiveKhz
            //! @{
            int getCom1ActiveKhz() const;
            void getCom1ActiveKhzAsync(int *o_com1Active);
            //! @}

            //! \copydoc FGSwiftBus::CService::getCom1StandbyKhz
            //! @{
            int getCom1StandbyKhz() const;
            void getCom1StandbyKhzAsync(int *o_com1Standby);
            //! @}

            //! \copydoc FGSwiftBus::CService::getCom2ActiveKhz
            //! @{
            int getCom2ActiveKhz() const;
            void getCom2ActiveKhzAsync(int *o_com2Active);
            //! @}

            //! \copydoc FGSwiftBus::CService::getCom2StandbyKhz
            //! @{
            int getCom2StandbyKhz() const;
            void getCom2StandbyKhzAsync(int *o_com2Standby);
            //! @}

            //! \copydoc FGSwiftBus::CService::getTransponderCode
            //! @{
            int getTransponderCode() const;
            void getTransponderCodeAsync(int *o_xpdrCode);
            //! @}

            //! \copydoc FGSwiftBus::CService::getTransponderMode
            //! @{
            int getTransponderMode() const;
            void getTransponderModeAsync(int *o_xpdrMode);
            //! @}

            //! \copydoc FGSwiftBus::CService::getTransponderIdent
            //! @{
            bool getTransponderIdent() const;
            void getTransponderIdentAsync(bool *o_ident);
            //! @}

            //! \copydoc FGSwiftBus::CService::getLandingLightsOn
            //! @{
            bool getBeaconLightsOn() const;
            void getBeaconLightsOnAsync(bool *o_beaconLightsOn);
            //! @}

            //! \copydoc FGSwiftBus::CService::getLandingLightsOn
            //! @{
            bool getLandingLightsOn() const;
            void getLandingLightsOnAsync(bool *o_landingLightsOn);
            //! @}

            //! \copydoc FGSwiftBus::CService::getNavLightsOn
            //! @{
            bool getNavLightsOn() const;
            void getNavLightsOnAsync(bool *o_navLightsOn);
            //! @}

            //! \copydoc FGSwiftBus::CService::getStrobeLightsOn
            //! @{
            bool getStrobeLightsOn() const;
            void getStrobeLightsOnAsync(bool *o_strobeLightsOn);
            //! @}

            //! \copydoc FGSwiftBus::CService::getTaxiLightsOn
            //! @{
            bool getTaxiLightsOn() const;
            void getTaxiLightsOnAsync(bool *o_taxiLightsOn);
            //! @}

            //! \copydoc FGSwiftBus::CService::getQNHInHg
            //! @{
            double getQNHInHg() const;
            void getQNHInHgAsync(double *o_qnh);
            //! @}

            //! \copydoc FGSwiftBus::CService::setCom1ActiveKhz
            void setCom1ActiveKhz(int freq);

            //! \copydoc FGSwiftBus::CService::setCom1StandbyKhz
            void setCom1StandbyKhz(int freq);

            //! \copydoc FGSwiftBus::CService::setCom2ActiveKhz
            void setCom2ActiveKhz(int freq);

            //! \copydoc FGSwiftBus::CService::setCom2StandbyKhz
            void setCom2StandbyKhz(int freq);

            //! \copydoc FGSwiftBus::CService::setTransponderCode
            void setTransponderCode(int code);

            //! \copydoc FGSwiftBus::CService::setTransponderMode
            void setTransponderMode(int mode);

            //! \copydoc FGSwiftBus::CService::getFlapsDeployRatio

            //! @{
            double getFlapsDeployRatio() const;
            void getFlapsDeployRatioAsync(double *o_flapsDeployRatio);
            //! @}

            //! \copydoc FGSwiftBus::CService::getGearDeployRatio
            //! @{
            double getGearDeployRatio() const;
            void getGearDeployRatioAsync(double *o_gearDeployRatio);
            //! @}

            //! \copydoc FGSwiftBus::CService::getEngineN1Percentage
            //! @{
            QList<double> getEngineN1Percentage() const;
            void getEngineN1PercentageAsync(QList<double> *o_engineN1Percentage);
            //! @}

            //! \copydoc FGSwiftBus::CService::getSpeedBrakeRatio
            //! @{
            double getSpeedBrakeRatio() const;
            void getSpeedBrakeRatioAsync(double *o_speedBrakeRatio);
            //! @}
        };
    }
}

#endif // guard
