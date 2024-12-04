// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/fsx/simconnectutilities.h"

#include <QFlags>
#include <QLatin1String>
#include <QMetaEnum>
#include <QMetaObject>

#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::misc::simulation::fsx
{
    CSimConnectUtilities::CSimConnectUtilities() {}

    QString CSimConnectUtilities::resolveEnumToString(const DWORD id, const char *enumName)
    {
        const int i = CSimConnectUtilities::staticMetaObject.indexOfEnumerator(enumName);
        if (i < 0) { return QStringLiteral("No enumerator for %1").arg(enumName); }
        const QMetaEnum m = CSimConnectUtilities::staticMetaObject.enumerator(i);
        const char *k = m.valueToKey(static_cast<int>(id));
        return (k) ? QLatin1String(k) : QStringLiteral("Id '%1' not found for %2").arg(id).arg(enumName);
    }

    QString CSimConnectUtilities::simConnectExceptionToString(const DWORD id)
    {
        return CSimConnectUtilities::resolveEnumToString(id, "SIMCONNECT_EXCEPTION");
    }

    QString CSimConnectUtilities::simConnectSurfaceTypeToString(const DWORD type, bool beautify)
    {
        QString sf = CSimConnectUtilities::resolveEnumToString(type, "SIMCONNECT_SURFACE");
        return beautify ? sf.replace('_', ' ') : sf;
    }

    QString CSimConnectUtilities::simConnectReceiveIdToString(DWORD type)
    {
        const QString ri = CSimConnectUtilities::resolveEnumToString(type, "SIMCONNECT_RECV_ID");
        return ri;
    }

    int CSimConnectUtilities::lightsToLightStates(const CAircraftLights &lights)
    {
        int lightMask = 0;
        if (lights.isBeaconOn()) { lightMask |= Beacon; }
        if (lights.isLandingOn()) { lightMask |= Landing; }
        if (lights.isLogoOn()) { lightMask |= Logo; }
        if (lights.isNavOn()) { lightMask |= Nav; }
        if (lights.isStrobeOn()) { lightMask |= Strobe; }
        if (lights.isTaxiOn()) { lightMask |= Taxi; }
        return lightMask;
    }

    void CSimConnectUtilities::registerMetadata()
    {
        qRegisterMetaType<CSimConnectUtilities::SIMCONNECT_EXCEPTION>();
        qRegisterMetaType<CSimConnectUtilities::SIMCONNECT_SURFACE>();
    }

    CWinDllUtils::DLLInfo CSimConnectUtilities::simConnectDllInfo()
    {
        const QList<CWinDllUtils::ProcessModule> modules = CWinDllUtils::getModules(-1, "simconnect");
        if (modules.isEmpty())
        {
            CWinDllUtils::DLLInfo info;
            info.errorMsg = "No SimConnect.dll loaded";
            return info;
        }
        return CWinDllUtils::getDllInfo(modules.first().executable);
    }
} // namespace swift::misc::simulation::fsx
