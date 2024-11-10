// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef DIRECTPLAY_HOST_H
#define DIRECTPLAY_HOST_H

#include "directplaypeer.h"

//! \file

namespace swift::simplugin::fs9
{
    //! Class encapsulating a FS9 host
    class CFs9Host : public CDirectPlayPeer
    {
        Q_OBJECT

    public:
        //! Connection status
        enum HostStatus
        {
            Hosting,
            Terminated
        };

        //! Constructor
        CFs9Host(QObject *parent = nullptr);

        //! Destructor
        virtual ~CFs9Host() override;

        //! Returns true if the users simulator is connected
        bool isConnected() const { return m_playerUser != 0; }

        //! Get DirectPlay host url
        QString getHostAddress();

    public slots:
        //! Send new text message
        void sendTextMessage(const QString &textMessage);

    signals:
        //! Hosting status changed
        void statusChanged(swift::simplugin::fs9::CFs9Host::HostStatus);

    private:
        //! Start host session
        HRESULT startHosting(const QString &session, const QString &callsign);

        //! Terminate a current active hosting session
        HRESULT stopHosting();

        HostStatus m_hostStatus = Terminated;
    };
} // ns

Q_DECLARE_METATYPE(swift::simplugin::fs9::CFs9Host::HostStatus)

#endif // guard
