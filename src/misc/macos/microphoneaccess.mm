// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "microphoneaccess.h"
#ifdef Q_OS_MAC
#import <AVFoundation/AVFoundation.h>
#endif

namespace swift::misc
{
    swift::misc::CMacOSMicrophoneAccess::CMacOSMicrophoneAccess(QObject *parent) :
        QObject(parent)
    { }

    void CMacOSMicrophoneAccess::requestAccess()
    {
#ifdef Q_OS_MAC
        if (@available(macOS 10.14, *))
        {
            NSString *mediaType = AVMediaTypeAudio;
            [AVCaptureDevice requestAccessForMediaType:mediaType completionHandler: ^ (BOOL granted)
                            {
                                emit permissionRequestAnswered(granted);
                            }];
        }
        else
        {
            emit permissionRequestAnswered(true);
        }
#endif
    }

    CMacOSMicrophoneAccess::AuthorizationStatus CMacOSMicrophoneAccess::getAuthorizationStatus()
    {
#ifdef Q_OS_MAC
        if (@available(macOS 10.14, *))
        {
            NSString *mediaType = AVMediaTypeAudio;
            AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:mediaType];
            if (authStatus == AVAuthorizationStatusAuthorized)
            {
                return AuthorizationStatus::Authorized;
            }
            else if (authStatus == AVAuthorizationStatusNotDetermined)
            {
                return AuthorizationStatus::NotDetermined;
            }
            return AuthorizationStatus::Denied;
        }
        else
        {
            return AuthorizationStatus::Authorized;
        }
#else
        return AuthorizationStatus::NotDetermined;
#endif
    }
} // ns
