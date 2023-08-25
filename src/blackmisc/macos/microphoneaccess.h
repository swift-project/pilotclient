// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AUDIOACCESSREQUEST_H
#define BLACKMISC_AUDIOACCESSREQUEST_H

#include "blackmisc/blackmiscexport.h"
#include <QtGlobal>
#include <QObject>

namespace BlackMisc
{
    //! Wrapper around MacOS 10.14 AVCaptureDevice AVCaptureDevice authorization
    class BLACKMISC_EXPORT CMacOSMicrophoneAccess : public QObject
    {
        Q_OBJECT

    public:
        //! Authorization status
        enum AuthorizationStatus
        {
            Authorized,
            Denied,
            NotDetermined
        };

        //! Constructor
        CMacOSMicrophoneAccess(QObject *parent = nullptr);

        //! Request access
        void requestAccess();

        //! Get current authorization status
        AuthorizationStatus getAuthorizationStatus();

    signals:
        //! User has answered the permission request popup
        void permissionRequestAnswered(bool granted);
    };
}

#endif
