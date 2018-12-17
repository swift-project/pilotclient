/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftguistdapplication.h"
#include "blackcore/application.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/coremodeenums.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/icons.h"

#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackCore;

CSwiftGuiStdApplication::CSwiftGuiStdApplication() :
    CGuiApplication(CApplicationInfo::swiftPilotClientGui(), CApplicationInfo::PilotClientGui, CIcons::swift1024())
{
    this->addParserOption(m_cmdFacadeMode);
    this->addDBusAddressOption();
    this->addVatlibOptions();
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
        if (coreMode == CoreModes::CoreInGuiProcess && !dBusAddress.isEmpty())
        {
            const CStatusMessage m = CStatusMessage(this, CLogCategory::validation()).
                                     error("Inconsistent pair DBus: '%1' and core: '%2'")
                                     << dBusAddress << coreModeStr;
            return CStatusMessageList(m) ;
        }
    }

    // Implicit configuration
    CStatusMessageList msgs;
    if (!dBusAddress.isEmpty() && coreModeStr.isEmpty())
    {
        coreMode = CoreModes::CoreExternalAudioGui; // default
        const CStatusMessage m = CStatusMessage(this, CLogCategory::validation()).
                                 info("No DBus address, setting core mode: '%1'")
                                 << CoreModes::coreModeToString(coreMode);
        msgs.push_back(m);
    }
    else if (dBusAddress.isEmpty() && (coreMode == CoreModes::CoreExternalCoreAudio || coreMode == CoreModes::CoreExternalAudioGui))
    {
        dBusAddress = CDBusServer::sessionBusAddress(); // a possible default
        const CStatusMessage m = CStatusMessage(this, CLogCategory::validation()).
                                 info("Setting DBus address to '%1'")
                                 << dBusAddress;
        msgs.push_back(m);
    }

    CCoreFacadeConfig runtimeConfig;
    switch (coreMode)
    {
    case CoreModes::CoreExternalCoreAudio:
        runtimeConfig = CCoreFacadeConfig::remote(dBusAddress);
        break;
    case CoreModes::CoreExternalAudioGui:
        runtimeConfig = CCoreFacadeConfig::remoteLocalAudio(dBusAddress);
        break;
    default:
    case CoreModes::CoreInGuiProcess:
        runtimeConfig = CCoreFacadeConfig::local(dBusAddress);
        break;
    }

    const CStatusMessageList contextMsgs = this->useContexts(runtimeConfig);
    msgs.push_back(contextMsgs);
    return contextMsgs;
}

bool CSwiftGuiStdApplication::parsingHookIn()
{
    // Parse core relevant arguments
    const QString dBusAddress(this->getCmdDBusAddressValue());
    if (!dBusAddress.isEmpty())
    {
        // check if rechable
        if (!CDBusServer::isDBusAvailable(dBusAddress))
        {
            this->cmdLineErrorMessage("DBus server at '" + dBusAddress + "' can not be reached");
            return false;
        }
    }
    return CGuiApplication::parsingHookIn();
}

CSwiftGuiStdApplication *CSwiftGuiStdApplication::instance()
{
    return qobject_cast<CSwiftGuiStdApplication *>(CApplication::instance());
}
