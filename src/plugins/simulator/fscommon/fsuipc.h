/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSUIPC_H
#define BLACKSIMPLUGIN_FSUIPC_H

#include "plugins/simulator/fscommon/fscommonexport.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/weather/weathergrid.h"
#include <QStringList>

namespace BlackSimPlugin::FsCommon
{
    //! Class representing a FSUIPC "interface"
    class FSCOMMON_EXPORT CFsuipc : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CFsuipc(QObject *parent = nullptr);

        //! Destructor
        virtual ~CFsuipc() override;

        //! Open connection with FSUIPC
        bool open(bool force = false);

        //! Disconnect
        void close();

        //! Is opened?
        bool isOpened() const;

        //! Really open, means connected and data can be sent
        bool isOpen() const;

        //! Write aircraft
        bool write(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Write variables
        bool write(const BlackMisc::Aviation::CTransponder &xpdr);

        //! Write weather grid to simulator
        bool write(const BlackMisc::Weather::CWeatherGrid &weatherGrid);

        //! Set simulator time
        bool setSimulatorTime(int hour, int minute);

        //! Get the version
        QString getVersion() const;

        //! Read data from FSUIPC
        //! \param aircraft       object to be updated
        //! \param cockpit        update cockpit data
        //! \param situation      update situation data
        //! \param aircraftParts  update parts
        //! \return read
        bool read(BlackMisc::Simulation::CSimulatedAircraft &aircraft, bool cockpit, bool situation, bool aircraftParts);

        //! Error messages
        static const QStringList &errorMessages()
        {
            static const QStringList errors(
                {
                    "Okay",
                    "Attempt to Open when already Open",
                    "Cannot link to FSUIPC or WideClient",
                    "Failed to Register common message with Windows",
                    "Failed to create Atom for mapping filename",
                    "Failed to create a file mapping object",
                    "Failed to open a view to the file map",
                    "Incorrect version of FSUIPC, or not FSUIPC",
                    "Sim is not version requested",
                    "Call cannot execute, link not Open",
                    "Call cannot execute: no requests accumulated",
                    "IPC timed out all retries",
                    "IPC sendmessage failed all retries",
                    "IPC request contains bad data",
                    "Maybe running on WideClient, but FS not running on Server, or wrong FSUIPC",
                    "Read or Write request cannot be added, memory for Process is full",
                });
            return errors;
        }

        //! Message for index
        static const QString &errorMessage(int index)
        {
            if (index >= 0 && index < errorMessages().size()) { return errorMessages().at(index); }
            static const QString unknown("Unknown error message index");
            return unknown;
        }

        //! Simulators
        static const QStringList &simulators()
        {
            static const QStringList sims(
                { "any", "FS98", "FS2000", "CFS2", "CFS1", "Fly!", "FS2002", "FS2004", "FSX", "ESP", "P3D" });
            return sims;
        }

        //! Simulator for index
        static const QString &simulator(int index)
        {
            if (index >= 0 && index < simulators().size()) { return simulators().at(index); }
            static const QString unknown("Unknown simulator index");
            return unknown;
        }

    protected:
        //! \copydoc QObject::timerEvent
        void timerEvent(QTimerEvent *event) override;

    private:
        //! Clear weather
        void clearAllWeather();

        //! Process weather
        void processWeatherMessages();

        bool m_opened = false;
        int m_openCount = 0; //!< statistics
        int m_closeCount = 0; //!< statistics
        int m_lastErrorIndex = 0;
        QString m_lastErrorMessage;
        QString m_fsuipcVersion;

        struct FsuipcWeatherMessage;
        QVector<FsuipcWeatherMessage> m_weatherMessageQueue;
        unsigned int m_lastTimestamp = 0;

        //! Integer representing fractional
        static double intToFractional(double fractional);
    };
} // ns

#endif // guard
