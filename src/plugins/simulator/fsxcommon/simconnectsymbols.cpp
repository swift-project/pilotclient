/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simconnectsymbols.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/logcategories.h"
#include <QLibrary>
#include <Windows.h>
#include <SimConnect.h>
#include <array>

// clazy:excludeall=function-args-by-ref

using namespace BlackMisc;

bool loadAndResolveSimConnect(bool manifestProbing)
{
    Q_UNUSED(manifestProbing);
    return true;
}

// old FSX API: https://docs.microsoft.com/en-us/previous-versions/microsoft-esp/cc526983(v=msdn.10)
//! \todo MS 2018-11 as of slack chat, change to "using PfnSimConnect_Open = HRESULT(__stdcall *)(HANDLE *, LPCSTR, HWND, DWORD, HANDLE, DWORD);"
typedef HRESULT(__stdcall *PfnSimConnect_Open)(HANDLE *, LPCSTR, HWND, DWORD, HANDLE, DWORD);
typedef HRESULT(__stdcall *PfnSimConnect_Close)(HANDLE);
typedef HRESULT(__stdcall *PfnSimConnect_AddToDataDefinition)(HANDLE, SIMCONNECT_DATA_DEFINITION_ID, const char *, const char *, SIMCONNECT_DATATYPE, float, DWORD);
typedef HRESULT(__stdcall *PfnSimConnect_Text)(HANDLE, SIMCONNECT_TEXT_TYPE, float, SIMCONNECT_CLIENT_EVENT_ID, DWORD, void *);
typedef HRESULT(__stdcall *PfnSimConnect_CallDispatch)(HANDLE, DispatchProc, void *);
typedef HRESULT(__stdcall *PfnSimConnect_WeatherSetModeCustom)(HANDLE);
typedef HRESULT(__stdcall *PfnSimConnect_WeatherSetModeGlobal)(HANDLE);
typedef HRESULT(__stdcall *PfnSimConnect_WeatherSetObservation)(HANDLE, DWORD, const char *);
typedef HRESULT(__stdcall *PfnSimConnect_TransmitClientEvent)(HANDLE, SIMCONNECT_OBJECT_ID, SIMCONNECT_CLIENT_EVENT_ID, DWORD, SIMCONNECT_NOTIFICATION_GROUP_ID, SIMCONNECT_EVENT_FLAG);
typedef HRESULT(__stdcall *PfnSimConnect_SetClientData)(HANDLE, SIMCONNECT_CLIENT_DATA_ID, SIMCONNECT_CLIENT_DATA_DEFINITION_ID, SIMCONNECT_CLIENT_DATA_SET_FLAG, DWORD, DWORD, void *);
typedef HRESULT(__stdcall *PfnSimConnect_RequestDataOnSimObject)(HANDLE, SIMCONNECT_DATA_REQUEST_ID, SIMCONNECT_DATA_DEFINITION_ID, SIMCONNECT_OBJECT_ID, SIMCONNECT_PERIOD, SIMCONNECT_DATA_REQUEST_FLAG, DWORD, DWORD, DWORD);
typedef HRESULT(__stdcall *PfnSimConnect_RequestClientData)(HANDLE, SIMCONNECT_CLIENT_DATA_ID, SIMCONNECT_DATA_REQUEST_ID, SIMCONNECT_CLIENT_DATA_DEFINITION_ID, SIMCONNECT_CLIENT_DATA_PERIOD, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG, DWORD, DWORD, DWORD);
typedef HRESULT(__stdcall *PfnSimConnect_SubscribeToSystemEvent)(HANDLE, SIMCONNECT_CLIENT_EVENT_ID, const char *);
typedef HRESULT(__stdcall *PfnSimConnect_MapClientEventToSimEvent)(HANDLE, SIMCONNECT_CLIENT_EVENT_ID, const char *);
typedef HRESULT(__stdcall *PfnSimConnect_SubscribeToFacilities)(HANDLE, SIMCONNECT_FACILITY_LIST_TYPE, SIMCONNECT_DATA_REQUEST_ID);
typedef HRESULT(__stdcall *PfnSimConnect_GetLastSentPacketID)(HANDLE, DWORD *);
typedef HRESULT(__stdcall *PfnSimConnect_AIRemoveObject)(HANDLE, SIMCONNECT_OBJECT_ID, SIMCONNECT_DATA_REQUEST_ID);
typedef HRESULT(__stdcall *PfnSimConnect_SetDataOnSimObject)(HANDLE, SIMCONNECT_DATA_DEFINITION_ID, SIMCONNECT_OBJECT_ID, SIMCONNECT_DATA_SET_FLAG, DWORD, DWORD, void *);
typedef HRESULT(__stdcall *PfnSimConnect_AIReleaseControl)(HANDLE, SIMCONNECT_OBJECT_ID, SIMCONNECT_DATA_REQUEST_ID);
typedef HRESULT(__stdcall *PfnSimConnect_AICreateNonATCAircraft)(HANDLE, const char *, const char *, SIMCONNECT_DATA_INITPOSITION, SIMCONNECT_DATA_REQUEST_ID);
typedef HRESULT(__stdcall *PfnSimConnect_AICreateEnrouteATCAircraft)(HANDLE, const char *, const char *, int, const char *, double, BOOL, SIMCONNECT_DATA_REQUEST_ID);
typedef HRESULT(__stdcall *PfnSimConnect_AICreateParkedATCAircraft)(HANDLE, const char *, const char *, const char *, SIMCONNECT_DATA_REQUEST_ID);
typedef HRESULT(__stdcall *PfnSimConnect_AICreateSimulatedObject)(HANDLE, const char *, SIMCONNECT_DATA_INITPOSITION, SIMCONNECT_DATA_REQUEST_ID);
typedef HRESULT(__stdcall *PfnSimConnect_MapClientDataNameToID)(HANDLE, const char *, SIMCONNECT_CLIENT_DATA_ID);
typedef HRESULT(__stdcall *PfnSimConnect_CreateClientData)(HANDLE, SIMCONNECT_CLIENT_DATA_ID, DWORD, SIMCONNECT_CREATE_CLIENT_DATA_FLAG);
typedef HRESULT(__stdcall *PfnSimConnect_AddToClientDataDefinition)(HANDLE, SIMCONNECT_CLIENT_DATA_DEFINITION_ID, DWORD, DWORD, float, DWORD);

#ifdef Q_OS_WIN64
typedef HRESULT(__stdcall *PfnSimConnect_RequestGroundInfo)(HANDLE, SIMCONNECT_DATA_REQUEST_ID, double, double, double, double, double, double, DWORD, DWORD, SIMCONNECT_GROUND_INFO_LATLON_FORMAT, SIMCONNECT_GROUND_INFO_ALT_FORMAT, SIMCONNECT_GROUND_INFO_SOURCE_FLAG);
typedef HRESULT(__stdcall *PfnSimConnect_ChangeView)(HANDLE, const char *);
typedef HRESULT(__stdcall *PfnSimConnect_AIReleaseControlEx)(HANDLE, SIMCONNECT_OBJECT_ID, SIMCONNECT_DATA_REQUEST_ID, BOOL);
typedef HRESULT(__stdcall *PfnSimConnect_CloseView)(HANDLE, const char *);
typedef HRESULT(__stdcall *PfnSimConnect_OpenView)(HANDLE, const char *, const char *);
typedef HRESULT(__stdcall *PfnSimConnect_ChangeView)(HANDLE, const char *);
typedef HRESULT(__stdcall *PfnSimConnect_CreateCameraInstance)(HANDLE, const GUID, const char *, SIMCONNECT_OBJECT_ID, SIMCONNECT_DATA_REQUEST_ID);
typedef HRESULT(__stdcall *PfnSimConnect_DeleteCameraInstance)(HANDLE, const GUID, UINT32);
typedef HRESULT(__stdcall *PfnSimConnect_CreateCameraDefinition)(HANDLE, const GUID, SIMCONNECT_CAMERA_TYPE, const char *, SIMCONNECT_DATA_XYZ, SIMCONNECT_DATA_PBH);
typedef HRESULT(__stdcall *PfnSimConnect_ObserverAttachToEntityOn)(HANDLE, const char *, DWORD, SIMCONNECT_DATA_XYZ);
typedef HRESULT(__stdcall *PfnSimConnect_CreateObserver)(HANDLE, const char *, SIMCONNECT_DATA_OBSERVER);
typedef HRESULT(__stdcall *PfnSimConnect_SetObserverLookAt)(HANDLE, const char *, SIMCONNECT_DATA_LATLONALT);
#endif

//! The SimConnect Symbols
//! \private
//! @{
struct SimConnectSymbols
{
    PfnSimConnect_Open SimConnect_Open = nullptr;
    PfnSimConnect_Close SimConnect_Close = nullptr;
    PfnSimConnect_AddToDataDefinition SimConnect_AddToDataDefinition = nullptr;
    PfnSimConnect_Text SimConnect_Text = nullptr;
    PfnSimConnect_CallDispatch SimConnect_CallDispatch = nullptr;
    PfnSimConnect_WeatherSetModeCustom SimConnect_WeatherSetModeCustom = nullptr;
    PfnSimConnect_WeatherSetModeGlobal SimConnect_WeatherSetModeGlobal = nullptr;
    PfnSimConnect_WeatherSetObservation SimConnect_WeatherSetObservation = nullptr;
    PfnSimConnect_TransmitClientEvent SimConnect_TransmitClientEvent = nullptr;
    PfnSimConnect_SetClientData SimConnect_SetClientData = nullptr;
    PfnSimConnect_RequestDataOnSimObject SimConnect_RequestDataOnSimObject = nullptr;
    PfnSimConnect_RequestClientData SimConnect_RequestClientData = nullptr;
    PfnSimConnect_SubscribeToSystemEvent SimConnect_SubscribeToSystemEvent = nullptr;
    PfnSimConnect_MapClientEventToSimEvent SimConnect_MapClientEventToSimEvent = nullptr;
    PfnSimConnect_SubscribeToFacilities SimConnect_SubscribeToFacilities = nullptr;
    PfnSimConnect_GetLastSentPacketID SimConnect_GetLastSentPacketID = nullptr;
    PfnSimConnect_AIRemoveObject SimConnect_AIRemoveObject = nullptr;
    PfnSimConnect_SetDataOnSimObject SimConnect_SetDataOnSimObject = nullptr;
    PfnSimConnect_AIReleaseControl SimConnect_AIReleaseControl = nullptr;
    PfnSimConnect_AICreateNonATCAircraft SimConnect_AICreateNonATCAircraft = nullptr;
    PfnSimConnect_AICreateParkedATCAircraft SimConnect_AICreateParkedATCAircraft = nullptr;
    PfnSimConnect_AICreateEnrouteATCAircraft SimConnect_AICreateEnrouteATCAircraft = nullptr;
    PfnSimConnect_AICreateSimulatedObject SimConnect_AICreateSimulatedObject = nullptr;
    PfnSimConnect_MapClientDataNameToID SimConnect_MapClientDataNameToID = nullptr;
    PfnSimConnect_CreateClientData SimConnect_CreateClientData = nullptr;
    PfnSimConnect_AddToClientDataDefinition SimConnect_AddToClientDataDefinition = nullptr;
#ifdef Q_OS_WIN64
    PfnSimConnect_RequestGroundInfo SimConnect_RequestGroundInfo = nullptr;
    PfnSimConnect_ChangeView SimConnect_ChangeView = nullptr;
    PfnSimConnect_AIReleaseControlEx SimConnect_AIReleaseControlEx = nullptr;
    PfnSimConnect_CreateCameraDefinition SimConnect_CreateCameraDefinition = nullptr;
    PfnSimConnect_ObserverAttachToEntityOn SimConnect_ObserverAttachToEntityOn = nullptr;
    PfnSimConnect_CreateObserver SimConnect_CreateObserver = nullptr;
    PfnSimConnect_CreateCameraInstance SimConnect_CreateCameraInstance = nullptr;
    PfnSimConnect_DeleteCameraInstance SimConnect_DeleteCameraInstance = nullptr;
    PfnSimConnect_SetObserverLookAt SimConnect_SetObserverLookAt = nullptr;
    PfnSimConnect_OpenView SimConnect_OpenView = nullptr;
    PfnSimConnect_CloseView SimConnect_CloseView = nullptr;
#endif
};
//! @}

static SimConnectSymbols gSymbols;

template<typename FuncPtr>
bool resolveSimConnectSymbol(QLibrary &library, FuncPtr &funcPtr, const char *funcName)
{
    funcPtr = reinterpret_cast<FuncPtr>(library.resolve(funcName));
    if (! funcPtr)
    {
        CLogMessage(CLogCategories::driver()).error(u"Failed to resolve %1: %2") << funcName << library.errorString();
        return false;
    }
    return true;
}

bool resolveCommonSimConnectSymbols(QLibrary &simConnectDll)
{
    bool resolveSuccess = true;
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_Open, "SimConnect_Open");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_Close, "SimConnect_Close");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_AddToDataDefinition, "SimConnect_AddToDataDefinition");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_Text, "SimConnect_Text");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_CallDispatch, "SimConnect_CallDispatch");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_WeatherSetModeCustom, "SimConnect_WeatherSetModeCustom");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_WeatherSetModeGlobal, "SimConnect_WeatherSetModeGlobal");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_WeatherSetObservation, "SimConnect_WeatherSetObservation");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_TransmitClientEvent, "SimConnect_TransmitClientEvent");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_SetClientData, "SimConnect_SetClientData");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_RequestDataOnSimObject, "SimConnect_RequestDataOnSimObject");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_RequestClientData, "SimConnect_RequestClientData");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_SubscribeToSystemEvent, "SimConnect_SubscribeToSystemEvent");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_MapClientEventToSimEvent, "SimConnect_MapClientEventToSimEvent");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_SubscribeToFacilities, "SimConnect_SubscribeToFacilities");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_GetLastSentPacketID, "SimConnect_GetLastSentPacketID");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_AIRemoveObject, "SimConnect_AIRemoveObject");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_SetDataOnSimObject, "SimConnect_SetDataOnSimObject");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_AIReleaseControl, "SimConnect_AIReleaseControl");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_AICreateNonATCAircraft, "SimConnect_AICreateNonATCAircraft");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_AICreateEnrouteATCAircraft, "SimConnect_AICreateEnrouteATCAircraft");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_AICreateParkedATCAircraft, "SimConnect_AICreateParkedATCAircraft");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_AICreateSimulatedObject, "SimConnect_AICreateSimulatedObject");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_MapClientDataNameToID, "SimConnect_MapClientDataNameToID");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_CreateClientData, "SimConnect_CreateClientData");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_AddToClientDataDefinition, "SimConnect_AddToClientDataDefinition");
    return resolveSuccess;
}

#ifdef Q_OS_WIN64
bool resolveP3DSimConnectSymbols(QLibrary &simConnectDll)
{
    bool resolveSuccess = true;
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_RequestGroundInfo, "SimConnect_RequestGroundInfo");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_ChangeView, "SimConnect_ChangeView");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_AIReleaseControlEx, "SimConnect_AIReleaseControlEx");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_CreateCameraDefinition, "SimConnect_CreateCameraDefinition");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_ObserverAttachToEntityOn, "SimConnect_ObserverAttachToEntityOn");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_CreateObserver, "SimConnect_CreateObserver");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_CreateCameraInstance, "SimConnect_CreateCameraInstance");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_DeleteCameraInstance, "SimConnect_DeleteCameraInstance");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_SetObserverLookAt, "SimConnect_SetObserverLookAt");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_OpenView, "SimConnect_OpenView");
    resolveSuccess = resolveSuccess & resolveSimConnectSymbol(simConnectDll, gSymbols.SimConnect_CloseView, "SimConnect_CloseView");
    return resolveSuccess;
}


P3DSimConnectVersion stringToP3DVersion(const QString &p3d)
{
    if (p3d.length() >= 2)
    {
        const QString p = digitOnlyString(p3d);
        if (p.contains("40")) { return P3DSimConnectv40; }
        if (p.contains("41")) { return P3DSimConnectv41; }
        if (p.contains("42")) { return P3DSimConnectv42; }
        if (p.contains("43")) { return P3DSimConnectv43; }
        if (p.contains("44")) { return P3DSimConnectv44; }
        if (p.contains("45")) { return P3DSimConnectv45; }

        if (p.contains("50")) { return P3DSimConnectv45; }
        if (p.contains("51")) { return P3DSimConnectv45; }
        if (p.contains("52")) { return P3DSimConnectv45; }

    }
    return P3DSimConnectv44; // default
}

bool loadAndResolveP3DSimConnect(P3DSimConnectVersion version)
{
    // Check if already loaded
    if (gSymbols.SimConnect_Open) { return true; }

    QString simConnectFileName(QStringLiteral("SimConnect.P3D-"));

    switch (version)
    {
    case P3DSimConnectv40: simConnectFileName += "v4.0"; break;
    case P3DSimConnectv41: simConnectFileName += "v4.1"; break;
    case P3DSimConnectv42: simConnectFileName += "v4.2"; break;
    case P3DSimConnectv43: simConnectFileName += "v4.3"; break;
    case P3DSimConnectv44: simConnectFileName += "v4.4"; break;
    case P3DSimConnectv45: simConnectFileName += "v4.5"; break;
    }

    QLibrary simConnectDll(simConnectFileName);
    simConnectDll.setLoadHints(QLibrary::PreventUnloadHint);
    if (simConnectDll.load())
    {
        const bool resolvedCommon = resolveCommonSimConnectSymbols(simConnectDll);
        const bool resolvedP3DSimConnectSymbols = resolveP3DSimConnectSymbols(simConnectDll);
        if (!resolvedCommon)
        {
            CLogMessage(CLogCategories::driver()).error(u"Failed to resolve common symbols from SimConnect.dll: '%1'") << simConnectFileName;
            return false;
        }
        if (!resolvedP3DSimConnectSymbols)
        {
            CLogMessage(CLogCategories::driver()).error(u"Failed to resolve P3D symbols from SimConnect.dll: '%1'") << simConnectFileName;
            return false;
        }

        CLogMessage(CLogCategories::driver()).info(u"Loaded and resolved P3D symbols from SimConnect.dll: '%1'") << simConnectFileName;
        return  resolvedCommon && resolvedP3DSimConnectSymbols;
    }
    else
    {
        CLogMessage(CLogCategories::driver()).error(u"Failed to load SimConnect.dll: '%1' '%2'") << simConnectFileName << simConnectDll.errorString();
        return false;
    }
}

bool loadAndResolveMSFSimConnect()
{
    // Check if already loaded
    if (gSymbols.SimConnect_Open) { return true; }

    QString simConnectFileName(QStringLiteral("SimConnect.MSFS"));

    QLibrary simConnectDll(simConnectFileName);
    simConnectDll.setLoadHints(QLibrary::PreventUnloadHint);
    if (simConnectDll.load())
    {
        const bool resolvedCommon = resolveCommonSimConnectSymbols(simConnectDll);
        if (!resolvedCommon)
        {
            CLogMessage(CLogCategories::driver()).error(u"Failed to resolve common symbols from SimConnect.dll: '%1'") << simConnectFileName;
            return false;
        }

        CLogMessage(CLogCategories::driver()).info(u"Loaded and resolved MSFS symbols from SimConnect.dll: '%1'") << simConnectFileName;
        return  resolvedCommon;
    }
    else
    {
        CLogMessage(CLogCategories::driver()).error(u"Failed to load SimConnect.dll: '%1' '%2'") << simConnectFileName << simConnectDll.errorString();
        return false;
    }
}

#else
bool loadAndResolveFsxSimConnect(bool manifestProbing)
{
    // Check if already loaded
    if (gSymbols.SimConnect_Open) { return true; }

    QLibrary simConnectDll(QStringLiteral("SimConnect"));
    simConnectDll.setLoadHints(QLibrary::PreventUnloadHint);
    if (manifestProbing)
    {
        HMODULE hInst = nullptr;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)loadAndResolveSimConnect, &hInst);
        wchar_t szModuleName[MAX_PATH];
        GetModuleFileName(hInst, szModuleName, MAX_PATH);

        // 101 => "SimConnect_RTM.manifest"
        // 102 => "SimConnect_SP1.manifest"
        // 103 => "SimConnect_XPack.manifest"
        // Use only SP1 and XPack, since RTM is missing two important symbols.
        // Try the latest one first.
        std::array<WORD, 2> resourceNumbers = {{ 103U, 102U }};

        for (const auto resourceNumber :  resourceNumbers)
        {
            ACTCTX actCtx;
            memset(&actCtx, 0, sizeof(ACTCTX));
            actCtx.cbSize = sizeof(actCtx);
            actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
            actCtx.lpSource = szModuleName;
            actCtx.lpResourceName = MAKEINTRESOURCE(resourceNumber);

            HANDLE hActCtx;
            hActCtx = CreateActCtx(&actCtx);
            if (hActCtx != INVALID_HANDLE_VALUE)
            {
                ULONG_PTR lpCookie = 0;
                if (ActivateActCtx(hActCtx, &lpCookie))
                {
                    simConnectDll.load();
                    DeactivateActCtx(0, lpCookie);
                    if (simConnectDll.isLoaded()) { break; }
                }
            }
            ReleaseActCtx(hActCtx);
        }
    }

    // If at that stage, no SimConnect library was found in the WinSxS folder, try once without activation context to link to the one we
    // ship ourselves.
    if (!simConnectDll.isLoaded())
    {
        simConnectDll.load();
    }

    if (simConnectDll.isLoaded())
    {
        return resolveCommonSimConnectSymbols(simConnectDll);
    }
    else
    {
        CLogMessage(CLogCategories::driver()).error(u"Failed to load SimConnect.dll: %1") << simConnectDll.errorString();
        return false;
    }
}

#endif

SIMCONNECTAPI SimConnect_Open(HANDLE *phSimConnect, LPCSTR szName, HWND hWnd, DWORD UserEventWin32, HANDLE hEventHandle, DWORD ConfigIndex)
{
    return gSymbols.SimConnect_Open(phSimConnect, szName, hWnd, UserEventWin32, hEventHandle, ConfigIndex);
}

SIMCONNECTAPI SimConnect_Close(HANDLE hSimConnect)
{
    return gSymbols.SimConnect_Close(hSimConnect);
}

SIMCONNECTAPI SimConnect_AddToDataDefinition(HANDLE hSimConnect, SIMCONNECT_DATA_DEFINITION_ID DefineID, const char *DatumName, const char *UnitsName, SIMCONNECT_DATATYPE DatumType, float fEpsilon, DWORD DatumID)
{
    return gSymbols.SimConnect_AddToDataDefinition(hSimConnect, DefineID, DatumName, UnitsName, DatumType, fEpsilon, DatumID);
}

SIMCONNECTAPI SimConnect_Text(HANDLE hSimConnect, SIMCONNECT_TEXT_TYPE type, float fTimeSeconds, SIMCONNECT_CLIENT_EVENT_ID EventID, DWORD cbUnitSize, void *pDataSet)
{
    return gSymbols.SimConnect_Text(hSimConnect, type, fTimeSeconds, EventID, cbUnitSize, pDataSet);
}

SIMCONNECTAPI SimConnect_CallDispatch(HANDLE hSimConnect, DispatchProc pfcnDispatch, void *pContext)
{
    return gSymbols.SimConnect_CallDispatch(hSimConnect, pfcnDispatch, pContext);
}

SIMCONNECTAPI SimConnect_WeatherSetModeCustom(HANDLE hSimConnect)
{
    return gSymbols.SimConnect_WeatherSetModeCustom(hSimConnect);
}

SIMCONNECTAPI SimConnect_WeatherSetModeGlobal(HANDLE hSimConnect)
{
    return gSymbols.SimConnect_WeatherSetModeGlobal(hSimConnect);
}

SIMCONNECTAPI SimConnect_WeatherSetObservation(HANDLE hSimConnect, DWORD Seconds, const char *szMETAR)
{
    return gSymbols.SimConnect_WeatherSetObservation(hSimConnect, Seconds, szMETAR);
}

SIMCONNECTAPI SimConnect_TransmitClientEvent(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_CLIENT_EVENT_ID EventID, DWORD dwData, SIMCONNECT_NOTIFICATION_GROUP_ID GroupID, SIMCONNECT_EVENT_FLAG Flags)
{
    return gSymbols.SimConnect_TransmitClientEvent(hSimConnect, ObjectID, EventID, dwData, GroupID, Flags);
}

SIMCONNECTAPI SimConnect_SetClientData(HANDLE hSimConnect, SIMCONNECT_CLIENT_DATA_ID ClientDataID, SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID, SIMCONNECT_CLIENT_DATA_SET_FLAG Flags, DWORD dwReserved, DWORD cbUnitSize, void *pDataSet)
{
    return gSymbols.SimConnect_SetClientData(hSimConnect, ClientDataID, DefineID, Flags, dwReserved, cbUnitSize, pDataSet);
}

SIMCONNECTAPI SimConnect_RequestDataOnSimObject(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, SIMCONNECT_DATA_DEFINITION_ID DefineID, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_PERIOD Period, SIMCONNECT_DATA_REQUEST_FLAG Flags, DWORD origin, DWORD interval, DWORD limit)
{
    return gSymbols.SimConnect_RequestDataOnSimObject(hSimConnect, RequestID, DefineID, ObjectID, Period, Flags, origin, interval, limit);
}

SIMCONNECTAPI SimConnect_RequestClientData(HANDLE hSimConnect, SIMCONNECT_CLIENT_DATA_ID ClientDataID, SIMCONNECT_DATA_REQUEST_ID RequestID, SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID, SIMCONNECT_CLIENT_DATA_PERIOD Period, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG Flags, DWORD origin, DWORD interval, DWORD limit)
{
    return gSymbols.SimConnect_RequestClientData(hSimConnect, ClientDataID, RequestID, DefineID, Period, Flags, origin, interval, limit);
}

SIMCONNECTAPI SimConnect_SubscribeToSystemEvent(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID EventID, const char *SystemEventName)
{
    return gSymbols.SimConnect_SubscribeToSystemEvent(hSimConnect, EventID, SystemEventName);
}

SIMCONNECTAPI SimConnect_MapClientEventToSimEvent(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID EventID, const char *EventName)
{
    return gSymbols.SimConnect_MapClientEventToSimEvent(hSimConnect, EventID, EventName);
}

SIMCONNECTAPI SimConnect_SubscribeToFacilities(HANDLE hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE type, SIMCONNECT_DATA_REQUEST_ID RequestID)
{
    return gSymbols.SimConnect_SubscribeToFacilities(hSimConnect, type, RequestID);
}

SIMCONNECTAPI SimConnect_GetLastSentPacketID(HANDLE hSimConnect, DWORD *pdwError)
{
    return gSymbols.SimConnect_GetLastSentPacketID(hSimConnect, pdwError);
}

SIMCONNECTAPI SimConnect_AIRemoveObject(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_DATA_REQUEST_ID RequestID)
{
    return gSymbols.SimConnect_AIRemoveObject(hSimConnect, ObjectID, RequestID);
}

SIMCONNECTAPI SimConnect_SetDataOnSimObject(HANDLE hSimConnect, SIMCONNECT_DATA_DEFINITION_ID DefineID, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_DATA_SET_FLAG Flags, DWORD ArrayCount, DWORD cbUnitSize, void *pDataSet)
{
    return gSymbols.SimConnect_SetDataOnSimObject(hSimConnect, DefineID, ObjectID, Flags, ArrayCount, cbUnitSize, pDataSet);
}

SIMCONNECTAPI SimConnect_AIReleaseControl(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_DATA_REQUEST_ID RequestID)
{
    return gSymbols.SimConnect_AIReleaseControl(hSimConnect, ObjectID, RequestID);
}

SIMCONNECTAPI SimConnect_AICreateNonATCAircraft(HANDLE hSimConnect, const char *szContainerTitle, const char *szTailNumber, SIMCONNECT_DATA_INITPOSITION InitPos, SIMCONNECT_DATA_REQUEST_ID RequestID)
{
    return gSymbols.SimConnect_AICreateNonATCAircraft(hSimConnect, szContainerTitle, szTailNumber, InitPos, RequestID);
}

SIMCONNECTAPI SimConnect_AICreateEnrouteATCAircraft(HANDLE hSimConnect, const char *szContainerTitle, const char *szTailNumber, int iFlightNumber, const char *szFlightPlanPath, double dFlightPlanPosition, BOOL bTouchAndGo, SIMCONNECT_DATA_REQUEST_ID RequestID)
{
    return gSymbols.SimConnect_AICreateEnrouteATCAircraft(hSimConnect, szContainerTitle, szTailNumber, iFlightNumber, szFlightPlanPath, dFlightPlanPosition, bTouchAndGo, RequestID);
}

SIMCONNECTAPI SimConnect_AICreateParkedATCAircraft(HANDLE hSimConnect, const char *szContainerTitle, const char *szTailNumber, const char *szAirportID, SIMCONNECT_DATA_REQUEST_ID RequestID)
{
    return gSymbols.SimConnect_AICreateParkedATCAircraft(hSimConnect, szContainerTitle, szTailNumber, szAirportID, RequestID);
}

SIMCONNECTAPI SimConnect_AICreateSimulatedObject(HANDLE hSimConnect, const char *szContainerTitle, SIMCONNECT_DATA_INITPOSITION InitPos, SIMCONNECT_DATA_REQUEST_ID RequestID)
{
    return gSymbols.SimConnect_AICreateSimulatedObject(hSimConnect, szContainerTitle, InitPos, RequestID);
}

SIMCONNECTAPI SimConnect_MapClientDataNameToID(HANDLE hSimConnect, const char *szClientDataName, SIMCONNECT_CLIENT_DATA_ID ClientDataID)
{
    return gSymbols.SimConnect_MapClientDataNameToID(hSimConnect, szClientDataName, ClientDataID);
}

SIMCONNECTAPI SimConnect_CreateClientData(HANDLE hSimConnect, SIMCONNECT_CLIENT_DATA_ID ClientDataID, DWORD dwSize, SIMCONNECT_CREATE_CLIENT_DATA_FLAG Flags)
{
    return gSymbols.SimConnect_CreateClientData(hSimConnect, ClientDataID, dwSize, Flags);
}

SIMCONNECTAPI SimConnect_AddToClientDataDefinition(HANDLE hSimConnect, SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID, DWORD dwOffset, DWORD dwSizeOrType, float fEpsilon, DWORD DatumID)
{
    return gSymbols.SimConnect_AddToClientDataDefinition(hSimConnect, DefineID, dwOffset, dwSizeOrType, fEpsilon, DatumID);
}

#ifdef Q_OS_WIN64
SIMCONNECTAPI SimConnect_RequestGroundInfo(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, double minLat, double minLon, double minAlt, double maxLat, double maxLon, double maxAlt, DWORD dwGridWidth, DWORD dwGridHeight, SIMCONNECT_GROUND_INFO_LATLON_FORMAT eLatLonFormat, SIMCONNECT_GROUND_INFO_ALT_FORMAT eAltFormat, SIMCONNECT_GROUND_INFO_SOURCE_FLAG eSourceFlags)
{
    return gSymbols.SimConnect_RequestGroundInfo(hSimConnect, RequestID, minLat, minLon, minAlt, maxLat, maxLon, maxAlt, dwGridWidth, dwGridHeight, eLatLonFormat, eAltFormat, eSourceFlags);
}

SIMCONNECTAPI SimConnect_ChangeView(HANDLE hSimConnect, const char *szName)
{
    return gSymbols.SimConnect_ChangeView(hSimConnect, szName);
}

SIMCONNECTAPI SimConnect_AIReleaseControlEx(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_DATA_REQUEST_ID RequestID, BOOL destroyAI)
{
    return gSymbols.SimConnect_AIReleaseControlEx(hSimConnect, ObjectID, RequestID, destroyAI);
}

SIMCONNECTAPI SimConnect_ObserverAttachToEntityOn(HANDLE hSimConnect, const char *szName, DWORD dwObjectID, SIMCONNECT_DATA_XYZ Offset)
{
    return gSymbols.SimConnect_ObserverAttachToEntityOn(hSimConnect, szName, dwObjectID, Offset);
}

SIMCONNECTAPI SimConnect_CreateObserver(HANDLE hSimConnect, const char *szName, SIMCONNECT_DATA_OBSERVER ObserverData)
{
    return gSymbols.SimConnect_CreateObserver(hSimConnect, szName, ObserverData);
}

SIMCONNECTAPI SimConnect_OpenView(HANDLE hSimConnect, const char *szName, const char *szTitle)
{
    return gSymbols.SimConnect_OpenView(hSimConnect, szName, szTitle);
}

SIMCONNECTAPI SimConnect_CloseView(HANDLE hSimConnect, const char *szName)
{
    return gSymbols.SimConnect_CloseView(hSimConnect, szName);
}

SIMCONNECTAPI SimConnect_SetObserverLookAt(HANDLE hSimConnect, const char *szName, SIMCONNECT_DATA_LATLONALT TargetPosition)
{
    return gSymbols.SimConnect_SetObserverLookAt(hSimConnect, szName, TargetPosition);
}

#endif
