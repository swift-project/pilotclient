/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "plugin.h"
#include <iostream>

CPlugin::CPlugin(BlackMisc::IPluginFactory &factory, BlackMisc::IContext &context)
: m_ctorOK(false),
  m_factory(factory),
  m_context(context)
{
    std::cout << "Sample plugin constructor" << std::endl;
    m_ctorOK = true;
}

CPlugin::~CPlugin()
{
    std::cout << "Sample plugin destructor" << std::endl;
}