/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simconnect_exception.h"
#include <QDebug>

namespace BlackCore
{
    namespace FSX
    {
        CSimConnectException::CSimConnectException()
        {
        }

        void CSimConnectException::handleException(SIMCONNECT_EXCEPTION exception)
        {
            switch(exception)
            {
            case SIMCONNECT_EXCEPTION_ERROR:
                break;
            case SIMCONNECT_EXCEPTION_SIZE_MISMATCH:
                break;
            case SIMCONNECT_EXCEPTION_UNRECOGNIZED_ID:
                break;
            case SIMCONNECT_EXCEPTION_UNOPENED:
                break;
            case SIMCONNECT_EXCEPTION_VERSION_MISMATCH:
                break;
            case SIMCONNECT_EXCEPTION_TOO_MANY_GROUPS:
                break;
            case SIMCONNECT_EXCEPTION_NAME_UNRECOGNIZED:
                break;
            case SIMCONNECT_EXCEPTION_TOO_MANY_EVENT_NAMES:
                break;
            case SIMCONNECT_EXCEPTION_EVENT_ID_DUPLICATE:
                break;
            case SIMCONNECT_EXCEPTION_TOO_MANY_MAPS:
                break;
            case SIMCONNECT_EXCEPTION_TOO_MANY_OBJECTS:
                break;
            case SIMCONNECT_EXCEPTION_TOO_MANY_REQUESTS:
                break;
            case SIMCONNECT_EXCEPTION_WEATHER_INVALID_PORT:
                break;
            case SIMCONNECT_EXCEPTION_WEATHER_INVALID_METAR:
                break;
            case SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_GET_OBSERVATION:
                break;
            case SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_CREATE_STATION:
                break;
            case SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_REMOVE_STATION:
                break;
            case SIMCONNECT_EXCEPTION_INVALID_DATA_TYPE:
                break;
            case SIMCONNECT_EXCEPTION_INVALID_DATA_SIZE:
            {
                qDebug() << "Invalid data size!";
                break;
            }
            case SIMCONNECT_EXCEPTION_DATA_ERROR:
                break;
            case SIMCONNECT_EXCEPTION_INVALID_ARRAY:
                break;
            case SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED:
            {
                qDebug() << "Failed to create object!";
                break;
            }
            case SIMCONNECT_EXCEPTION_LOAD_FLIGHTPLAN_FAILED:
                break;
            case SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE:
                break;
            case SIMCONNECT_EXCEPTION_ILLEGAL_OPERATION:
                break;
            case SIMCONNECT_EXCEPTION_ALREADY_SUBSCRIBED:
                break;
            case SIMCONNECT_EXCEPTION_INVALID_ENUM:
                break;
            case SIMCONNECT_EXCEPTION_DEFINITION_ERROR:
                break;
            case SIMCONNECT_EXCEPTION_DUPLICATE_ID:
                break;
            case SIMCONNECT_EXCEPTION_DATUM_ID:
                break;
            case SIMCONNECT_EXCEPTION_OUT_OF_BOUNDS:
                break;
            case SIMCONNECT_EXCEPTION_ALREADY_CREATED:
                break;
            case SIMCONNECT_EXCEPTION_OBJECT_OUTSIDE_REALITY_BUBBLE:
                break;
            case SIMCONNECT_EXCEPTION_OBJECT_CONTAINER:
                break;
            case SIMCONNECT_EXCEPTION_OBJECT_AI:
                break;
            case SIMCONNECT_EXCEPTION_OBJECT_ATC:
                break;
            case SIMCONNECT_EXCEPTION_OBJECT_SCHEDULE:
                break;
            default:
                break;
            }
        }
    }
}
