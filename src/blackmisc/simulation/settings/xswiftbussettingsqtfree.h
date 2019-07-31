/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKMISC_SIMULATION_SETTINGS_CXSWIFTBUSSETTINGSQTFREE_H
#define BLACKMISC_SIMULATION_SETTINGS_CXSWIFTBUSSETTINGSQTFREE_H

#include "blackmisc/simulation/xplane/qtfreeutils.h"
#include <string>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            /*!
             * XSwiftBus/swift side settings class, JSON capable, shared among all services
             * \details Used on swift and XSwiftBus side, MUST BE QT free
             */
            class CXSwiftBusSettingsQtFree
            {
            public:
                //! Constructor.
                CXSwiftBusSettingsQtFree();

                //! JSON Constructor.
                CXSwiftBusSettingsQtFree(const std::string &json);

                //! Dtor
                virtual ~CXSwiftBusSettingsQtFree() {}

                //! DBus server
                const std::string &getDBusServerAddress() const { return m_dBusServerAddress; }

                //! Set DBus server
                void setDBusServerAddress(const std::string &dBusServer) { m_dBusServerAddress = dBusServer; }

                //! Set whether the plugin draws type and callsign labels above aircraft
                void setDrawingLabels(bool drawing) { m_drawingLabels = drawing; }

                //! Get whether the plugin draws type and callsign labels above aircraft
                bool isDrawingLabels() const { return m_drawingLabels; }

                //! Set the maximum number of aircraft.
                bool setMaxPlanes(int planes)
                {
                    if (planes == m_maxPlanes) { return false; }
                    m_maxPlanes = planes;
                    return true;
                }

                //! Get the maximum number of aircraft.
                int getMaxPlanes() const { return m_maxPlanes; }

                //! Set follow aircraft distance
                bool setFollowAircraftDistanceM(int meters)
                {
                    if (meters == m_followAircraftDistanceM) { return false; }
                    m_followAircraftDistanceM = meters;
                    return true;
                }

                //! Get follow aircraft distance
                int getFollowAircraftDistanceM() const { return m_followAircraftDistanceM; }

                //! Set the maximum distance at which to draw aircraft (nautical miles).
                double getMaxDrawDistanceNM() const { return m_maxDrawDistanceNM; }

                //! Set the maximum distance at which to draw aircraft (nautical miles).
                bool setMaxDrawDistanceNM(double nauticalMiles)
                {
                    if (BlackMisc::Simulation::XPlane::QtFreeUtils::isFuzzyEqual(nauticalMiles, m_maxDrawDistanceNM)) { return false; }
                    m_maxDrawDistanceNM = nauticalMiles;
                    return true;
                }

                //! Load and parse config file
                bool parseXSwiftBusString(const std::string &json);

                //! As JSON string
                std::string toXSwiftBusJsonString() const;

                //! Convert to string
                std::string convertToString() const;

                //! Update only changed values
                int update(const CXSwiftBusSettingsQtFree &newValues);

                //! Sets timestamp to now
                virtual void setCurrentUtcTime();

            protected:
                //! The JSON members @{
                static constexpr char JsonDBusServerAddress[] = "dbusserveradress";
                static constexpr char JsonDrawingLabels[]     = "drawinglabels";
                static constexpr char JsonMaxPlanes[]         = "maxplanes";
                static constexpr char JsonMaxDrawDistance[]   = "maxDrawDistance";
                static constexpr char JsonTimestamp[]         = "timestamp";
                static constexpr char JsonFollowAircraftDistanceM[] = "followAircraftDistance";
                //! @}

                //! Object has been updated
                virtual void objectUpdated();

                std::string m_dBusServerAddress { "tcp:host=127.0.0.1,port=45001" }; //!< DBus server
                int    m_maxPlanes = 100;               //!< max. planes in XPlane
                int    m_followAircraftDistanceM = 200; //!< follow aircraft in distance
                bool   m_drawingLabels       = true;    //!< labels in XPlane
                double m_maxDrawDistanceNM   = 50.0;    //!< distance in XPlane
                int64_t m_msSinceEpochQtFree = 0;       //!< timestamp
            };
        } // ns
    } // ns
} // ns

#endif
