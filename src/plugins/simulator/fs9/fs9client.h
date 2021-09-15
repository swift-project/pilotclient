/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef DIRECTPLAY_CLIENT_H
#define DIRECTPLAY_CLIENT_H

#include "directplaypeer.h"
#include "blackmisc/simulation/interpolatormulti.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/pq/time.h"
#include <QScopedPointer>
#include <QReadWriteLock>

//! \file

namespace BlackCore { class ISimulator; }
namespace BlackSimPlugin::Fs9
{
    //! Class faking a FS9 multiplayer client connection
    class CFs9Client : public CDirectPlayPeer
    {
        Q_OBJECT

    public:
        //! Connection status
        enum ClientStatus
        {
            Connected,
            Disconnected
        };

        //! Constructor
        CFs9Client(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft,
                    const BlackMisc::PhysicalQuantities::CTime      &updateInterval,
                    BlackMisc::Simulation::CInterpolationLogger    *logger,
                    BlackCore::ISimulator *simulator);

        //! Destructor
        virtual ~CFs9Client() override;

        //! Set DirectPlay host address
        void setHostAddress(const QString &hostAddress);

        //! Starts the FS9 client messaging
        void start();

        //! Get interpolator
        //! @{
        BlackMisc::Simulation::CInterpolatorMulti       *getInterpolator() { return &m_interpolator; }
        const BlackMisc::Simulation::CInterpolatorMulti *getInterpolator() const { return &m_interpolator; }
        //! @}

        //! \copydoc BlackMisc::Simulation::CInterpolator::getInterpolationMessages
        BlackMisc::CStatusMessageList getInterpolationMessages(BlackMisc::Simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! Send a situation (position)
        void sendMultiplayerPosition(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Send parts (lights, gear ...)
        void sendMultiplayerParts(const BlackMisc::Aviation::CAircraftParts &parts);

        //! Send new text message
        void sendTextMessage(const QString &textMessage);

    signals:
        //! Client status changed
        void statusChanged(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, BlackSimPlugin::Fs9::CFs9Client::ClientStatus);

    protected:
        //! \copydoc QObject::timerEvent
        virtual void timerEvent(QTimerEvent *event) override;

    private:
        //! Enumerate all FS9 session hosts
        HRESULT enumDirectPlayHosts();

        //! Create host address
        HRESULT createHostAddress();

        //! Start hosting session
        HRESULT connectToSession(const BlackMisc::Aviation::CCallsign &callsign);

        //! Close the connection
        HRESULT closeConnection();

        //! Send functions
        //! @{
        void sendMultiplayerPositionAndPartsFromInterpolation();
        void sendMultiplayerChangePlayerPlane();
        //! @}

        void handleConnectionCompleted();

        //! Simulator interface
        const BlackCore::ISimulator *simulator() const;

        BlackMisc::Simulation::CSimulatedAircraft m_remoteAircraft;
        BlackMisc::PhysicalQuantities::CTime      m_updateInterval;
        BlackMisc::Simulation::CInterpolatorMulti m_interpolator;
        QString m_modelName;
        int m_timerId = 0;

        IDirectPlay8Address *m_hostAddress  = nullptr;
        ClientStatus         m_clientStatus = Disconnected;

        PLAYER_INFO_STRUCT m_playerInfo;
        DPN_PLAYER_INFO m_player;
    };
} // ns

Q_DECLARE_METATYPE(BlackSimPlugin::Fs9::CFs9Client::ClientStatus)

#endif // guard
