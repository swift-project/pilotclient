/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "directplayerror.h"
#include "blackmisc/logmessage.h"

namespace BlackSimPlugin::Fs9
{
    HRESULT logDirectPlayError(HRESULT error)
    {
        QString errorMessage;
        switch (error)
        {
        case DPNERR_BUFFERTOOSMALL:
            errorMessage = "The supplied buffer is not large enough to contain the requested data.";
            break;
        case DPNERR_DOESNOTEXIST:
            errorMessage = "Requested element is not part of the address.";
            break;
        case DPNERR_INVALIDFLAGS:
            errorMessage = "The flags passed to this method are invalid.";
            break;
        case DPNERR_INVALIDPARAM:
            errorMessage = "One or more of the parameters passed to the method are invalid.";
            break;
        case DPNERR_INVALIDPOINTER:
            errorMessage = "Pointer specified as a parameter is invalid.";
            break;
        case DPNERR_INVALIDURL:
            errorMessage = "Specified string is not a valid DirectPlayURL.";
            break;
        case DPNERR_NOTALLOWED:
            errorMessage = "This function is not allowed on this object.";
            break;
        case DPNERR_INVALIDOBJECT:
            errorMessage = "The Microsoft DirectPlay object pointer is invalid.";
            break;
        case DPNERR_UNINITIALIZED:
            errorMessage = "This function is not allowed on this object.";
            break;
        case DPNERR_UNSUPPORTED:
            errorMessage = "The function or feature is not available in this implementation or on this service provider.";
            break;
        case DPNERR_NOTHOST:
            errorMessage = "The client attempted to connect to a nonhost computer. Additionally, this error value may be returned by a nonhost that tried to set the application description.";
            break;
        default:
            errorMessage = QString("Unknown error code %1").arg(error);
            break;
        }

        errorMessage = "DirectPlay: " + errorMessage;
        BlackMisc::CLogMessage("swift.fs9.freefunctions").error(errorMessage);
        return error;
    }
}
