// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "swiftguiminiapplication.h"

#include "core/application.h"
#include "core/corefacadeconfig.h"
#include "misc/icons.h"

using namespace swift::misc;
using namespace swift::core;

CSwiftGuiMiniApplication::CSwiftGuiMiniApplication()
    : CGuiApplication(CApplicationInfo::swiftPilotClientGui(), CApplicationInfo::PilotClientGui, CIcons::swift1024())
{
    // deliberately no DBus/core mode command line options:
    // this GUI only supports local (standalone) mode
    this->addNetworkOptions();
    this->addAudioOptions();
}

CStatusMessageList CSwiftGuiMiniApplication::startHookIn()
{
    Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Not yet parsed cmd line arguments");

    // always local, no DBus - strongly simplified compared to CSwiftGuiStdApplication
    const CCoreFacadeConfig runtimeConfig(CCoreFacadeConfig::Local);
    return this->initContextsAndStartCoreFacade(runtimeConfig);
}

CSwiftGuiMiniApplication *CSwiftGuiMiniApplication::instance()
{
    return qobject_cast<CSwiftGuiMiniApplication *>(CApplication::instance());
}