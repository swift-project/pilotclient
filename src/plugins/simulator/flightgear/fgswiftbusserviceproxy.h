/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FLIGHTGEAR_SERVICE_PROXY_H
#define BLACKSIMPLUGIN_FLIGHTGEAR_SERVICE_PROXY_H

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
            //! Get Flightgear version number
            int getVersionNumber();

            //! Get own aircraft situation data
            void getOwnAircraftSituationData(FlightgearData *o_flightgearData);

            //! Get own aircraft velocity data
            void getOwnAircraftVelocityData(FlightgearData *o_flightgearData);

            //! Add a text message to the on-screen display
            void addTextMessage(const QString &text);

            //! Get full path to current aircraft model
            //! @{
            QString getAircraftModelPath() const;
            void getAircraftModelPathAsync(QString *o_modelPath);
            //! @}

            //! Get base filename of current aircraft model
            //! @{
            QString getAircraftModelFilename() const;
            void getAircraftModelFilenameAsync(QString *o_modelFilename);
            //! @}

            //! Get current aircraft livery
            //! @{
            QString getAircraftLivery() const;
            void getAircraftLiveryAsync(QString *o_modelLivery);
            //! @}

            //! Get the ICAO code of the current aircraft model
            //! @{
            QString getAircraftIcaoCode() const;
            void getAircraftIcaoCodeAsync(QString *o_icaoCode);
            //! @}

            //! Get the description of the current aircraft model
            //! @{
            QString getAircraftDescription() const;
            void getAircraftDescriptionAsync(QString *o_description);
            //! @}

            //! Get canonical swift model string of current aircraft model
            //! @{
            QString getAircraftModelString() const;
            void getAircraftModelStringAsync(QString *o_modelString);
            //! @}

            //! Get name of current aircraft model
            //! @{
            QString getAircraftName() const;
            void getAircraftNameAsync(QString *o_name);
            //! @}

            //! True if sim is paused
            //! @{
            bool isPaused() const;
            void isPausedAsync(bool *o_paused);
            //! @}

            //! Get aircraft latitude in degrees
            //! @{
            double getLatitudeDeg() const;
            void getLatitudeDegAsync(double *o_latitude);
            //! @}

            //! Get aircraft longitude in degrees
            //! @{
            double getLongitudeDeg() const;
            void getLongitudeDegAsync(double *o_longitude);
            //! @}

            //! Get aircraft altitude in feet
            //! @{
            double getAltitudeMslFt() const;
            void getAltitudeMslFtAsync(double *o_altitude);
            //! @}

            //! Get aircraft height in feet
            //! @{
            double getHeightAglFt() const;
            void getHeightAglFtAsync(double *o_height);
            //! @}

            //! Get aircraft groundspeed in knots
            //! @{
            double getGroundSpeedKts() const;
            void getGroundSpeedKtsAsync(double *o_groundspeed);
            //! @}

            //! Get aircraft pitch in degrees above horizon
            //! @{
            double getPitchDeg() const;
            void getPitchDegAsync(double *o_pitch);
            //! @}

            //! Get aircraft roll in degrees
            //! @{
            double getRollDeg() const;
            void getRollDegAsync(double *o_roll);
            //! @}

            //! Get whether all wheels are on the ground
            //! @{
            bool getAllWheelsOnGround() const;
            void getAllWheelsOnGroundAsync(bool *o_allWheels);
            //! @}

            //! Get the current COM1 active frequency in kHz
            //! @{
            int getCom1ActiveKhz() const;
            void getCom1ActiveKhzAsync(int *o_com1Active);
            //! @}

            //! Get the current COM1 standby frequency in kHz
            //! @{
            int getCom1StandbyKhz() const;
            void getCom1StandbyKhzAsync(int *o_com1Standby);
            //! @}

            //! Get the current COM2 active frequency in kHz
            //! @{
            int getCom2ActiveKhz() const;
            void getCom2ActiveKhzAsync(int *o_com2Active);
            //! @}

            //! Get the current COM2 standby frequency in kHz
            //! @{
            int getCom2StandbyKhz() const;
            void getCom2StandbyKhzAsync(int *o_com2Standby);
            //! @}

            //! Get the current transponder code in decimal
            //! @{
            int getTransponderCode() const;
            void getTransponderCodeAsync(int *o_xpdrCode);
            //! @}

            //! Get the current transponder mode (depends on the aircraft, 0 and 1 usually mean standby, >1 active)
            //! @{
            int getTransponderMode() const;
            void getTransponderModeAsync(int *o_xpdrMode);
            //! @}

            //! Get whether we are currently squawking ident
            //! @{
            bool getTransponderIdent() const;
            void getTransponderIdentAsync(bool *o_ident);
            //! @}

            //! Get whether beacon lights are on
            //! @{
            bool getBeaconLightsOn() const;
            void getBeaconLightsOnAsync(bool *o_beaconLightsOn);
            //! @}

            //! Get whether landing lights are on
            //! @{
            bool getLandingLightsOn() const;
            void getLandingLightsOnAsync(bool *o_landingLightsOn);
            //! @}

            //! Get whether nav lights are on
            //! @{
            bool getNavLightsOn() const;
            void getNavLightsOnAsync(bool *o_navLightsOn);
            //! @}

            //! Get whether strobe lights are on
            //! @{
            bool getStrobeLightsOn() const;
            void getStrobeLightsOnAsync(bool *o_strobeLightsOn);
            //! @}

            //! Get whether taxi lights are on
            //! @{
            bool getTaxiLightsOn() const;
            void getTaxiLightsOnAsync(bool *o_taxiLightsOn);
            //! @}

            //! Get pressure altitude in feet.
            //! @{
            double getPressureAltitudeFt() const;
            void getPressureAltitudeFtAsync(double *o_qnh);
            //! @}

            //! Set the current COM1 active frequency in kHz
            void setCom1ActiveKhz(int freq);

            //! Set the current COM1 standby frequency in kHz
            void setCom1StandbyKhz(int freq);

            //! Set the current COM2 active frequency in kHz
            void setCom2ActiveKhz(int freq);

            //! Set the current COM2 standby frequency in kHz
            void setCom2StandbyKhz(int freq);

            //! Set the current transponder code in decimal
            void setTransponderCode(int code);

            //! Set the current transponder mode (depends on the aircraft, 0 and 1 usually mean standby, >1 active)
            void setTransponderMode(int mode);

            //! Get flaps deploy ratio, where 0.0 is flaps fully retracted, and 1.0 is flaps fully extended.
            //! @{
            double getFlapsDeployRatio() const;
            void getFlapsDeployRatioAsync(double *o_flapsDeployRatio);
            //! @}

            //! Get gear deploy ratio, where 0 is up and 1 is down
            //! @{
            double getGearDeployRatio() const;
            void getGearDeployRatioAsync(double *o_gearDeployRatio);
            //! @}

            //! Get the N1 speed as percent of max (per engine)
            //! @{
            QList<double> getEngineN1Percentage() const;
            void getEngineN1PercentageAsync(QList<double> *o_engineN1Percentage);
            //! @}

            //! Get the ratio how much the speedbrakes surfaces are extended (0.0 is fully retracted, and 1.0 is fully extended)
            //! @{
            double getSpeedBrakeRatio() const;
            void getSpeedBrakeRatioAsync(double *o_speedBrakeRatio);
            //! @}

            //! Get ground elevation [m] for current airplane position
            //! @{
            double getGroundElevation() const;
            void getGroundElevationAsync(double *o_groundElevation);
            //! @}
        };
    }
}

#endif // guard
