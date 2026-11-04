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
    // bewusst keine DBus-/Core-Mode Kommandozeilenoptionen:
    // diese GUI unterstuetzt nur den lokalen (Standalone) Modus
    this->addNetworkOptions();
    this->addAudioOptions();
}

CStatusMessageList CSwiftGuiMiniApplication::startHookIn()
{
    Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Not yet parsed cmd line arguments");

    // immer lokal, kein DBus - stark vereinfacht gegenueber CSwiftGuiStdApplication
    const CCoreFacadeConfig runtimeConfig(CCoreFacadeConfig::Local);
    return this->initContextsAndStartCoreFacade(runtimeConfig);
}

CSwiftGuiMiniApplication *CSwiftGuiMiniApplication::instance()
{
    return qobject_cast<CSwiftGuiMiniApplication *>(CApplication::instance());
}