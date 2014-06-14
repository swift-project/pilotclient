/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "service.h"
#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMUtilities.h>

namespace XBus
{

    CService::CService(QObject *parent) : QObject(parent)
    {
    }

    void CService::onAircraftModelChanged()
    {
        char filename[256];
        char path[512];
        XPLMGetNthAircraftModel(XPLM_USER_AIRCRAFT, filename, path);
        emit aircraftModelChanged(path, filename, m_liveryPath.get().c_str());
    }

    int CService::getXPlaneVersionMajor() const
    {
        int version;
        XPLMGetVersions(&version, nullptr, nullptr);
        if (version > 5000) { version /= 10; }
        return version / 100;
    }

    int CService::getXPlaneVersionMinor() const
    {
        int version;
        XPLMGetVersions(&version, nullptr, nullptr);
        if (version > 5000) { version /= 10; }
        return version % 100;
    }

    QString CService::getXPlaneInstallationPath() const
    {
        char path[512];
        XPLMGetSystemPath(path);
        return path;
    }

    QString CService::getXPlanePreferencesPath() const
    {
        char path[512];
        XPLMGetPrefsPath(path);
        return path;
    }

}
