// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef DIRECTPLAY_CLIENT_H
#define DIRECTPLAY_CLIENT_H

#include "directplaypeer.h"
#include "misc/simulation/interpolation/interpolatormulti.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/callsign.h"
#include "misc/pq/time.h"
#include <QScopedPointer>
#include <QReadWriteLock>

//! \file

namespace swift::core
{
    class ISimulator;
}
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
        CFs9Client(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft,
                   const swift::misc::physical_quantities::CTime &updateInterval,
                   swift::misc::simulation::CInterpolationLogger *logger,
                   swift::core::ISimulator *simulator);

        //! Destructor
        virtual ~CFs9Client() override;

        //! Set DirectPlay host address
        void setHostAddress(const QString &hostAddress);

        //! Starts the FS9 client messaging
        void start();

        //! Get interpolator
        //! @{
        swift::misc::simulation::CInterpolatorMulti *getInterpolator() { return &m_interpolator; }
        const swift::misc::simulation::CInterpolatorMulti *getInterpolator() const { return &m_interpolator; }
        //! @}

        //! \copydoc swift::misc::simulation::CInterpolator::getInterpolationMessages
        swift::misc::CStatusMessageList getInterpolationMessages(swift::misc::simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! Send a situation (position)
        void sendMultiplayerPosition(const swift::misc::aviation::CAircraftSituation &situation);

        //! Send parts (lights, gear ...)
        void sendMultiplayerParts(const swift::misc::aviation::CAircraftParts &parts);

        //! Send new text message
        void sendTextMessage(const QString &textMessage);

    signals:
        //! Client status changed
        void statusChanged(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, BlackSimPlugin::Fs9::CFs9Client::ClientStatus);

    protected:
        //! \copydoc QObject::timerEvent
        virtual void timerEvent(QTimerEvent *event) override;

    private:
        //! Enumerate all FS9 session hosts
        HRESULT enumDirectPlayHosts();

        //! Create host address
        HRESULT createHostAddress();

        //! Start hosting session
        HRESULT connectToSession(const swift::misc::aviation::CCallsign &callsign);

        //! Close the connection
        HRESULT closeConnection();

        //! Send functions
        //! @{
        void sendMultiplayerPositionAndPartsFromInterpolation();
        void sendMultiplayerChangePlayerPlane();
        //! @}

        void handleConnectionCompleted();

        //! Simulator interface
        const swift::core::ISimulator *simulator() const;

        swift::misc::simulation::CSimulatedAircraft m_remoteAircraft;
        swift::misc::physical_quantities::CTime m_updateInterval;
        swift::misc::simulation::CInterpolatorMulti m_interpolator;
        QString m_modelName;
        int m_timerId = 0;

        IDirectPlay8Address *m_hostAddress = nullptr;
        ClientStatus m_clientStatus = Disconnected;

        PLAYER_INFO_STRUCT m_playerInfo;
        DPN_PLAYER_INFO m_player;
    };
} // ns

Q_DECLARE_METATYPE(BlackSimPlugin::Fs9::CFs9Client::ClientStatus)

#endif // guard
