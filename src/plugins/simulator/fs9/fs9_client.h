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
#include "blackcore/interpolator_linear.h"
#include "blackmisc/avaircraftsituation.h"
#include "blackmisc/pqtime.h"
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
            CFs9Client(const QString &callsign, const BlackMisc::PhysicalQuantities::CTime &updateInterval,
                       QObject *parent = nullptr);

            //! Destructor
            virtual ~CFs9Client();

            //! Set DirectPlay host address
            void setHostAddress(const QString &hostAddress);

            //! Add new aircraft situation
            void addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        public slots:

            //! \copydoc CDirectPlayPeer::init
            virtual void init() override;

            //! Send new text message
            void sendTextMessage(const QString &textMessage);

            //! Disconnect client from session
            void disconnectFrom();

        signals:

            //! Client timed out
            void clientTimedOut(const QString &callsign);

            //! Client status changed
            void statusChanged(const QString &callsign, BlackSimPlugin::Fs9::CFs9Client::ClientStatus);

        protected slots:

            //! Timer event slot
            virtual void timerEvent(QTimerEvent *event) override;

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

            BlackMisc::Aviation::CAircraftSituation m_lastAircraftSituation;
            BlackMisc::PhysicalQuantities::CTime m_updateInterval;
            BlackCore::CInterpolatorLinear m_interpolator;
            int m_timerId = 0;

            QMutex m_mutexInterpolator;
            IDirectPlay8Address *m_hostAddress = nullptr;
            ClientStatus m_clientStatus = Disconnected;

            typedef CallbackWrapper<CFs9Client, HRESULT, DWORD, void *> TCallbackWrapper;
            TCallbackWrapper m_callbackWrapper;

        };
    }
}

Q_DECLARE_METATYPE(BlackSimPlugin::Fs9::CFs9Client::ClientStatus)

#endif // DIRECTPLAY_CLIENT_H
