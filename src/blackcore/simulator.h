/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SIMULATOR_H
#define BLACKCORE_SIMULATOR_H

#include "blacksim/simulatorinfo.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simdirectaccessownaircraft.h"
#include "blackmisc/simulation/simdirectaccessrenderedaircraft.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/nwtextmessage.h"
#include "blackmisc/nwclient.h"
#include "blackmisc/pixmap.h"
#include <QObject>

namespace BlackCore
{
    //! Interface to a simulator.
    class ISimulator : public QObject
    {
        Q_OBJECT
        Q_ENUMS(ConnectionStatus)

    public:
        //! ISimulator connection
        enum ConnectionStatus
        {
            Disconnected,
            Connected,
            ConnectionFailed
        };

        //! Destructor
        virtual ~ISimulator() {}

        //! Are we connected to the simulator?
        virtual bool isConnected() const = 0;

        //! Can we connect?
        virtual bool canConnect() const = 0;

        //! Is time synchronization on?
        virtual bool isTimeSynchronized() const = 0;

        //! Simulator paused?
        virtual bool isPaused() const = 0;

        //! Simulator running?
        virtual bool isSimulating() const = 0;

        //! Originator
        const QString &simulatorOriginator()
        {
            // string is generated once, the timestamp allows to use multiple
            // components (as long as they are not generated at the same ms)
            static const QString o = QString("SIMULATOR:").append(QString::number(QDateTime::currentMSecsSinceEpoch()));
            return o;
        }

    public slots:

        //! Connect to simulator
        virtual bool connectTo() = 0;

        //! Connect asynchronously to simulator
        virtual void asyncConnectTo() = 0;

        //! Disconnect from simulator
        virtual bool disconnectFrom() = 0;

        //! Add new remote aircraft to the simulator
        virtual bool addRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) = 0;

        //! Remove remote aircraft from simulator
        virtual bool removeRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Change remote aircraft per property
        virtual bool changeRenderedAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator) = 0;

        //! Aircraft got enabled / disabled
        virtual bool changeAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator) = 0;

        //! Update own aircraft cockpit (usually from context)
        virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator) = 0;

        //! ICAO data for model string
        virtual BlackMisc::Aviation::CAircraftIcao getIcaoForModelString(const QString &modelString) const = 0;

        //! Simulator info
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const = 0;

        //! Display a status message in the simulator
        virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const = 0;

        //! Display a text message
        virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const = 0;

        //! Aircraft models for available remote aircrafts
        virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const = 0;

        //! Airports in range
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const = 0;

        //! Set time synchronization between simulator and user's computer time
        //! \remarks not all drivers implement this, e.g. if it is an intrinsic simulator feature
        virtual bool setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) = 0;

        //! Time synchronization offset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const = 0;

        //! Representing icon for model string
        virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const = 0;

        //! Max. rendered aircraft
        virtual int getMaxRenderedAircraft() const = 0;

        //! Max. rendered aircraft
        virtual void setMaxRenderedAircraft(int maxRenderedAircraft) = 0;

    signals:
        //! Emitted when the connection status has changed
        void connectionStatusChanged(ISimulator::ConnectionStatus status);

        //! Emitted when own aircraft model has changed
        void ownAircraftModelChanged(BlackMisc::Simulation::CSimulatedAircraft aircraft);

        //! Simulator combined status
        void simulatorStatusChanged(bool connected, bool running, bool paused);

        //! Simulator started
        void simulatorStarted();

        //! Simulator stopped;
        void simulatorStopped();

        //! A single model has been matched
        void modelMatchingCompleted(BlackMisc::Simulation::CSimulatedAircraft aircraft);

        //! Installed aircraft models ready or changed
        void installedAircraftModelsChanged();

    protected:
        //! Default constructor
        ISimulator(QObject *parent = nullptr) : QObject(parent) {}

        //! Emit the combined status
        //! \sa simulatorStatusChanged;
        void emitSimulatorCombinedStatus();

    };

    //! Factory pattern class to create instances of ISimulator
    class ISimulatorFactory
    {
    public:

        //! ISimulatorVirtual destructor
        virtual ~ISimulatorFactory() {}

        //!
        //! Create a new instance of a driver
        //! \param ownAircraftProvider       in memory access to own aircraft data
        //! \param renderedAircraftProvider  in memory access to rendered aircraft data such as situation history and aircraft itself
        //! \param parent QObject
        //! \return driver instance
        //!
        virtual ISimulator *create(
            BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
            BlackMisc::Simulation::IRenderedAircraftProvider *renderedAircraftProvider,
            QObject *parent = nullptr) = 0;

        //! Simulator info
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const = 0;
    };

    //! Common base class with providers, interface and some base functionality
    class CSimulatorCommon :
        public BlackCore::ISimulator,
        public BlackMisc::Simulation::COwnAircraftProviderSupport,     // gain access to in memor own aircraft data
        public BlackMisc::Simulation::CRenderedAircraftProviderSupport // gain access to in memory rendered aircraft data
    {

    public:
        //! \copydoc ISimulator::getMaxRenderedAircraft
        int getMaxRenderedAircraft() const override;

        //! \copydoc ISimulator::setMaxRenderedAircraft
        void setMaxRenderedAircraft(int maxRenderedAircraft) override;

        //! \copydoc ISimulator::getSimulatorInfo
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;

    protected:
        //! Constructor
        CSimulatorCommon(
            const BlackSim::CSimulatorInfo &simInfo,
            BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
            BlackMisc::Simulation::IRenderedAircraftProvider *renderedAircraftProvider,
            QObject *parent = nullptr);

        BlackSim::CSimulatorInfo m_simulatorInfo; //!< about the simulator
        int m_maxRenderedAircraft = 99;           //!< max. rendered aircraft
    };

} // namespace

Q_DECLARE_INTERFACE(BlackCore::ISimulatorFactory, "org.swift.PilotClient.BlackCore.SimulatorInterface")
Q_DECLARE_METATYPE(BlackCore::ISimulator::ConnectionStatus)

#endif // guard
