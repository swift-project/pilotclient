/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/plugins.h"
#include <iostream>

class CPlugin : public BlackMisc::IPlugin
{
public:
    CPlugin(BlackMisc::IPluginFactory &factory, BlackMisc::IContext &context);

    virtual ~CPlugin();

    virtual bool isValid() const { return m_ctorOK; }

    virtual BlackMisc::IPluginFactory &getFactory() { return m_factory; }

private:
    bool m_ctorOK;
    BlackMisc::IPluginFactory &m_factory;
    BlackMisc::IContext &m_context;
};

MAKE_BLACK_PLUGIN(sample_plugin, CPlugin, "An example minimal plugin")

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