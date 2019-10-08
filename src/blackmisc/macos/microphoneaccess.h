/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
