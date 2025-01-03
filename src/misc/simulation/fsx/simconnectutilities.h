// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_FSX_SIMCONNECTUTILITIES_H
#define SWIFT_MISC_SIMULATION_FSX_SIMCONNECTUTILITIES_H

#include <QMetaType>
#include <QObject>
#include <QSettings>
#include <QString>

#include "misc/aviation/aircraftlights.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/swiftmiscexport.h"
#include "misc/windllutils.h"

// Apart from the below definitions, the following code is OS independent,
// though it does not make sense to be used on non WIN machines.
// But it allows such parts to compile on all platforms.
#ifdef Q_OS_WIN
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <Windows.h>
#else
using DWORD = unsigned long; //!< Fake Windows DWORD
#endif

namespace swift::misc::simulation::fsx
{
    //! Utilities for SimConnect
    //! \remark not using the simconnect.h headers as Misc classes are not driver aware
    class SWIFT_MISC_EXPORT CSimConnectUtilities : public QObject
    {
        Q_OBJECT

    public:
        //! Resolve SimConnect exception (based on Qt metadata).
        //! \param id enum element
        //! \return enum element's name
        static QString simConnectExceptionToString(const DWORD id);

        //! Resolve SimConnect surface (based on Qt metadata).
        //! \param type enum element
        //! \param beautify remove "_"
        static QString simConnectSurfaceTypeToString(const DWORD type, bool beautify = true);

        //! SimConnect surfaces.
        //! \sa http://msdn.microsoft.com/en-us/library/cc526981.aspx#AircraftFlightInstrumentationData
        enum SIMCONNECT_SURFACE
        {
            Concrete,
            Grass,
            Water,
            Grass_bumpy,
            Asphalt,
            Short_grass,
            Long_grass,
            Hard_turf,
            Snow,
            Ice,
            Urban,
            Forest,
            Dirt,
            Coral,
            Gravel,
            Oil_treated,
            Steel_mats,
            Bituminus,
            Brick,
            Macadam,
            Planks,
            Sand,
            Shale,
            Tarmac,
            Wright_flyer_track
        };
        Q_ENUM(SIMCONNECT_SURFACE)

        //! SimConnect exceptions.
        enum SIMCONNECT_EXCEPTION
        {
            SIMCONNECT_EXCEPTION_NONE,
            SIMCONNECT_EXCEPTION_ERROR,
            SIMCONNECT_EXCEPTION_SIZE_MISMATCH,
            SIMCONNECT_EXCEPTION_UNRECOGNIZED_ID,
            SIMCONNECT_EXCEPTION_UNOPENED,
            SIMCONNECT_EXCEPTION_VERSION_MISMATCH,
            SIMCONNECT_EXCEPTION_TOO_MANY_GROUPS,
            SIMCONNECT_EXCEPTION_NAME_UNRECOGNIZED,
            SIMCONNECT_EXCEPTION_TOO_MANY_EVENT_NAMES,
            SIMCONNECT_EXCEPTION_EVENT_ID_DUPLICATE,
            SIMCONNECT_EXCEPTION_TOO_MANY_MAPS,
            SIMCONNECT_EXCEPTION_TOO_MANY_OBJECTS,
            SIMCONNECT_EXCEPTION_TOO_MANY_REQUESTS,
            SIMCONNECT_EXCEPTION_WEATHER_INVALID_PORT,
            SIMCONNECT_EXCEPTION_WEATHER_INVALID_METAR,
            SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_GET_OBSERVATION,
            SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_CREATE_STATION,
            SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_REMOVE_STATION,
            SIMCONNECT_EXCEPTION_INVALID_DATA_TYPE,
            SIMCONNECT_EXCEPTION_INVALID_DATA_SIZE,
            SIMCONNECT_EXCEPTION_DATA_ERROR,
            SIMCONNECT_EXCEPTION_INVALID_ARRAY,
            SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED,
            SIMCONNECT_EXCEPTION_LOAD_FLIGHTPLAN_FAILED,
            SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE,
            SIMCONNECT_EXCEPTION_ILLEGAL_OPERATION,
            SIMCONNECT_EXCEPTION_ALREADY_SUBSCRIBED,
            SIMCONNECT_EXCEPTION_INVALID_ENUM,
            SIMCONNECT_EXCEPTION_DEFINITION_ERROR,
            SIMCONNECT_EXCEPTION_DUPLICATE_ID,
            SIMCONNECT_EXCEPTION_DATUM_ID,
            SIMCONNECT_EXCEPTION_OUT_OF_BOUNDS,
            SIMCONNECT_EXCEPTION_ALREADY_CREATED,
            SIMCONNECT_EXCEPTION_OBJECT_OUTSIDE_REALITY_BUBBLE,
            SIMCONNECT_EXCEPTION_OBJECT_CONTAINER,
            SIMCONNECT_EXCEPTION_OBJECT_AI,
            SIMCONNECT_EXCEPTION_OBJECT_ATC,
            SIMCONNECT_EXCEPTION_OBJECT_SCHEDULE
        };
        Q_ENUM(SIMCONNECT_EXCEPTION)

        //! Lights for FSX/P3D "LIGHT ON STATES"
        //! \sa http://www.prepar3d.com/SDKv2/LearningCenter/utilities/variables/simulation_variables.html
        enum LIGHT_STATES
        {
            Nav = 0x0001,
            Beacon = 0x0002,
            Landing = 0x0004,
            Taxi = 0x0008,
            Strobe = 0x0010,
            Panel = 0x0020,
            Recognition = 0x0040,
            Wing = 0x0080,
            Logo = 0x0100,
            Cabin = 0x0200
        };

        //! Receive IDs for SimConnect
        enum SIMCONNECT_RECV_ID
        {
            SIMCONNECT_RECV_ID_NULL,
            SIMCONNECT_RECV_ID_EXCEPTION,
            SIMCONNECT_RECV_ID_OPEN,
            SIMCONNECT_RECV_ID_QUIT,
            SIMCONNECT_RECV_ID_EVENT,
            SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE,
            SIMCONNECT_RECV_ID_EVENT_FILENAME,
            SIMCONNECT_RECV_ID_EVENT_FRAME,
            SIMCONNECT_RECV_ID_SIMOBJECT_DATA,
            SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE,
            SIMCONNECT_RECV_ID_WEATHER_OBSERVATION,
            SIMCONNECT_RECV_ID_CLOUD_STATE,
            SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID,
            SIMCONNECT_RECV_ID_RESERVED_KEY,
            SIMCONNECT_RECV_ID_CUSTOM_ACTION,
            SIMCONNECT_RECV_ID_SYSTEM_STATE,
            SIMCONNECT_RECV_ID_CLIENT_DATA,
            SIMCONNECT_RECV_ID_EVENT_WEATHER_MODE,
            SIMCONNECT_RECV_ID_AIRPORT_LIST,
            SIMCONNECT_RECV_ID_VOR_LIST,
            SIMCONNECT_RECV_ID_NDB_LIST,
            SIMCONNECT_RECV_ID_WAYPOINT_LIST,
            SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_SERVER_STARTED,
            SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_CLIENT_STARTED,
            SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_SESSION_ENDED,
            SIMCONNECT_RECV_ID_EVENT_RACE_END,
            SIMCONNECT_RECV_ID_EVENT_RACE_LAP,
        };
        Q_ENUM(SIMCONNECT_RECV_ID)

        //! Receive id to string
        static QString simConnectReceiveIdToString(DWORD type);

        //! Lights to states
        static int lightsToLightStates(const aviation::CAircraftLights &lights);

        //! Get info about SimConnect DLL
        static swift::misc::CWinDllUtils::DLLInfo simConnectDllInfo();

        //! Register metadata
        static void registerMetadata();

    private:
        //!
        //! Resolve enum value to its cleartext (based on Qt metadata).
        //! \param id enum element
        //! \param enumName name of the resolved enum
        //! \return enum element's name
        static QString resolveEnumToString(const DWORD id, const char *enumName);

        //! Hidden constructor
        CSimConnectUtilities();
    };
} // namespace swift::misc::simulation::fsx

Q_DECLARE_METATYPE(swift::misc::simulation::fsx::CSimConnectUtilities::SIMCONNECT_EXCEPTION)
Q_DECLARE_METATYPE(swift::misc::simulation::fsx::CSimConnectUtilities::SIMCONNECT_SURFACE)
Q_DECLARE_METATYPE(swift::misc::simulation::fsx::CSimConnectUtilities::SIMCONNECT_RECV_ID)

#endif // SWIFT_MISC_SIMULATION_FSX_SIMCONNECTUTILITIES_H
