/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAMPLE_PLUGIN_H
#define SAMPLE_PLUGIN_H

#include "blackmisc/plugins.h"

class CPlugin : public BlackMisc::IPlugin
{
public:
    CPlugin(BlackMisc::IPluginFactory &factory, BlackMisc::IContext &context);

    virtual ~CPlugin();

    virtual bool isValid() const override { return m_ctorOK; }

    virtual BlackMisc::IPluginFactory &getFactory() override { return m_factory; }

private:
    bool m_ctorOK;
    BlackMisc::IPluginFactory &m_factory;
    BlackMisc::IContext &m_context;
};

// This needs to be in the header so that Qt's "moc" processor can see it.
class CPluginFactory : public QObject, public BlackMisc::IPluginFactory
{
    Q_OBJECT
    Q_INTERFACES(BlackMisc::IPluginFactory)
    Q_PLUGIN_METADATA(IID BLACKMISC_IPLUGINFACTORY_IID)
    BLACKMISC_IMPLEMENT_IPLUGINFACTORY(CPlugin, "sample_plugin", "An example minimal plugin")
};

#endif //SAMPLE_PLUGIN_H