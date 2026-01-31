// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "swiftdataapplication.h"

#include <QtGlobal>

#include "core/application.h"
#include "core/corefacadeconfig.h"
#include "core/coremodeenums.h"
#include "misc/dbusserver.h"
#include "misc/icons.h"

using namespace swift::misc;
using namespace swift::core;

CSwiftDataApplication::CSwiftDataApplication()
    : CGuiApplication(CApplicationInfo::swiftMappingTool(), CApplicationInfo::MappingTool, CIcons::swiftDatabase48())
{
    // this->addParserOption(m_cmdFacadeMode);
    // this->addDBusAddressOption();
    // this->addNetworkOptions();
    // this->addAudioOptions();
}

CStatusMessageList CSwiftDataApplication::startHookIn()
{
    Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Not yet parsed cmd line arguments");

    QString dBusAddress(this->getCmdDBusAddressValue());
    const QString coreModeStr =
        this->isParserOptionSet(m_cmdFacadeMode) ? this->getParserValue(m_cmdFacadeMode) : QString();
    CoreModes::CoreMode coreMode = CoreModes::stringToCoreMode(coreModeStr);

    CStatusMessageList msgs;

    CCoreFacadeConfig runtimeConfig = coreModeToCoreFacadeConfig(coreMode, dBusAddress);
    const CStatusMessageList contextMsgs = this->initContextsAndStartCoreFacade(runtimeConfig);
    msgs.push_back(contextMsgs);
    return contextMsgs;
}

bool CSwiftDataApplication::parsingHookIn() { return CGuiApplication::parsingHookIn(); }

CSwiftDataApplication *CSwiftDataApplication::instance()
{
    return qobject_cast<CSwiftDataApplication *>(CApplication::instance());
}

CCoreFacadeConfig CSwiftDataApplication::coreModeToCoreFacadeConfig(CoreModes::CoreMode coreMode,
                                                                    const QString &dBusAddress)
{
    switch (coreMode)
    {
    case CoreModes::Distributed: return CCoreFacadeConfig(CCoreFacadeConfig::Remote, dBusAddress);
    case CoreModes::Standalone: return CCoreFacadeConfig(CCoreFacadeConfig::Local, dBusAddress); break;
    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Not handled core mode");
        return CCoreFacadeConfig(CCoreFacadeConfig::NotUsed, dBusAddress);
    }
}
