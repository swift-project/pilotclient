/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef DIRECTPLAY_CLIENT_H
#define DIRECTPLAY_CLIENT_H

#include "directplay_peer.h"
#include "blackcore/interpolator.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/pq/time.h"
#include <QMutex>
#include <QScopedPointer>

//! \file

namespace BlackSimPlugin
{
    namespace Fs9
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
            CFs9Client(BlackCore::IInterpolator *interpolator, QObject *owner, const QString &callsign, const BlackMisc::PhysicalQuantities::CTime &updateInterval);

            //! Destructor
            virtual ~CFs9Client();

            //! Set DirectPlay host address
            void setHostAddress(const QString &hostAddress);

            //! Add new aircraft situation
            void addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        public slots:
            //! Send new text message
            void sendTextMessage(const QString &textMessage);

        signals:
            //! Client status changed
            void statusChanged(const QString &callsign, BlackSimPlugin::Fs9::CFs9Client::ClientStatus);

        protected slots:
            //! Timer event slot
            virtual void timerEvent(QTimerEvent *event) override;

        protected:
            //! \copydoc CContinuousWorker::initialize
            virtual void initialize() override;

            //! \copydoc CContinuousWorker::cleanup
            virtual void cleanup() override;

        private:
            /*!
             * Enumerate all FS9 session hosts
             * \todo Ideally host enumeration is required only once (if ever).
             *       Move this into its own class with a static member host address.
             */
            HRESULT enumDirectPlayHosts();

            HRESULT createHostAddress();

            //! Start hosting session
            HRESULT connectToSession(const QString &playername);

            HRESULT closeConnection();

            void sendMultiplayerPosition(const BlackMisc::Aviation::CAircraftSituation &situation);
            void sendMultiplayerParamaters();

            BlackMisc::PhysicalQuantities::CTime m_updateInterval;
            BlackCore::IInterpolator *m_interpolator = nullptr;
            int m_timerId = 0;

            IDirectPlay8Address *m_hostAddress = nullptr;
            ClientStatus m_clientStatus = Disconnected;

            PLAYER_INFO_STRUCT m_playerInfo;
            DPN_PLAYER_INFO m_player;
        };
    }
}

Q_DECLARE_METATYPE(BlackSimPlugin::Fs9::CFs9Client::ClientStatus)

#endif // DIRECTPLAY_CLIENT_H
