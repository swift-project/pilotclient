// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "swiftguistdapplication.h"

#include <QtGlobal>

#include "core/application.h"
#include "core/corefacadeconfig.h"
#include "core/coremodeenums.h"
#include "misc/dbusserver.h"
#include "misc/icons.h"

using namespace swift::misc;
using namespace swift::core;

CSwiftGuiStdApplication::CSwiftGuiStdApplication() : CGuiApplication(CApplicationInfo::swiftPilotClientGui(), CApplicationInfo::PilotClientGui, CIcons::swift1024())
{
    this->addParserOption(m_cmdFacadeMode);
    this->addDBusAddressOption();
    this->addNetworkOptions();
    this->addAudioOptions();
}

CStatusMessageList CSwiftGuiStdApplication::startHookIn()
{
    Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Not yet parsed cmd line arguments");

    QString dBusAddress(this->getCmdDBusAddressValue());
    const QString coreModeStr = this->isParserOptionSet(m_cmdFacadeMode) ? this->getParserValue(m_cmdFacadeMode) : QString();
    CoreModes::CoreMode coreMode = CoreModes::stringToCoreMode(coreModeStr);

    // Valid combination?
    if (!coreModeStr.isEmpty())
    {
        if (coreMode == CoreModes::Standalone && !dBusAddress.isEmpty())
        {
            const CStatusMessage m = CStatusMessage(this, CLogCategories::validation()).error(u"Inconsistent pair DBus: '%1' and core: '%2'")
                                     << dBusAddress << coreModeStr;
            return CStatusMessageList(m);
        }
    }

    // Implicit configuration
    CStatusMessageList msgs;
    if (!dBusAddress.isEmpty() && coreModeStr.isEmpty())
    {
        coreMode = CoreModes::Distributed; // default
        const CStatusMessage m = CStatusMessage(this, CLogCategories::validation()).info(u"No DBus address, setting core mode: '%1'")
                                 << CoreModes::coreModeToString(coreMode);
        msgs.push_back(m);
    }
    else if (dBusAddress.isEmpty() && coreMode == CoreModes::Distributed)
    {
        dBusAddress = CDBusServer::sessionBusAddress(); // a possible default
        const CStatusMessage m = CStatusMessage(this, CLogCategories::validation()).info(u"Setting DBus address to '%1'")
                                 << dBusAddress;
        msgs.push_back(m);
    }

    CCoreFacadeConfig runtimeConfig;
    switch (coreMode)
    {
    case CoreModes::Distributed:
        runtimeConfig = CCoreFacadeConfig::remote(dBusAddress);
        break;
    default:
    case CoreModes::Standalone:
        runtimeConfig = CCoreFacadeConfig::local(dBusAddress);
        break;
    }

    const CStatusMessageList contextMsgs = this->initContextsAndStartCoreFacade(runtimeConfig);
    msgs.push_back(contextMsgs);
    return contextMsgs;
}

bool CSwiftGuiStdApplication::parsingHookIn()
{
    // Parse core relevant arguments
    const QString dBusAddress(this->getCmdDBusAddressValue());
    if (!dBusAddress.isEmpty())
    {
        // check if reachable
        if (!CDBusServer::isDBusAvailable(dBusAddress))
        {
            this->cmdLineErrorMessage("DBus error", "DBus server at '" + dBusAddress + "' can not be reached");
            return false;
        }
    }
    return CGuiApplication::parsingHookIn();
}

CSwiftGuiStdApplication *CSwiftGuiStdApplication::instance()
{
    return qobject_cast<CSwiftGuiStdApplication *>(CApplication::instance());
}
