// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AUDIOACCESSREQUEST_H
#define SWIFT_MISC_AUDIOACCESSREQUEST_H

#include <QObject>
#include <QtGlobal>

#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    //! Wrapper around MacOS 10.14 AVCaptureDevice AVCaptureDevice authorization
    class SWIFT_MISC_EXPORT CMacOSMicrophoneAccess : public QObject
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
} // namespace swift::misc

#endif // SWIFT_MISC_AUDIOACCESSREQUEST_H
