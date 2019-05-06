/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "microphoneaccess.h"
#import <AVFoundation/AVFoundation.h>

namespace BlackMisc
{
//#ifdef Q_OS_MAC

    BlackMisc::CMacOSMicrophoneAccess::CMacOSMicrophoneAccess(QObject *parent) :
        QObject(parent)
    { }

    void CMacOSMicrophoneAccess::requestAccess()
    {
        if (@available(macOS 10.14, *))
        {
            NSString *mediaType = AVMediaTypeAudio;
            [AVCaptureDevice requestAccessForMediaType:mediaType completionHandler:^(BOOL granted)
            {
                emit permissionRequestAnswered(granted);
            }];
        }
        else
        {
            emit permissionRequestAnswered(true);
        }

    }

    CMacOSMicrophoneAccess::AuthorizationStatus CMacOSMicrophoneAccess::getAuthorizationStatus()
    {
        if (@available(macOS 10.14, *))
        {
            NSString *mediaType = AVMediaTypeAudio;
            AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:mediaType];
            if(authStatus == AVAuthorizationStatusAuthorized)
            {
                return AuthorizationStatus::Authorized;
            }
            else if(authStatus == AVAuthorizationStatusNotDetermined)
            {
                return AuthorizationStatus::NotDetermined;
            }
            return AuthorizationStatus::Denied;
        }
        else
        {
            return AuthorizationStatus::Authorized;
        }

    }

// #endif
}

