/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include <QJsonValue>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Simulation
    {
        CSimulatorPluginInfo::CSimulatorPluginInfo(const QString &identifier, const QString &name, const QString &simulator, const QString &description, bool valid) :
            m_identifier(identifier), m_name(name), m_simulator(simulator), m_description(description), m_valid(valid)
        { }

        void CSimulatorPluginInfo::convertFromJson(const QJsonObject &json)
        {
            if (json.contains("IID"))   // comes from the plugin
            {
                if (! json.contains("MetaData")) { throw CJsonException("Missing 'MetaData'"); }

                // json data is already validated by CPluginManagerSimulator
                CJsonScope scope("MetaData");
                CValueObject::convertFromJson(json["MetaData"].toObject());
                m_valid = true;
            }
            else
            {
                CValueObject::convertFromJson(json);
            }
        }

        bool CSimulatorPluginInfo::isUnspecified() const
        {
            return m_identifier.isEmpty();
        }

        QString CSimulatorPluginInfo::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return QString("%1 (%2)").arg(m_name, m_identifier);
        }

        const QString &CSimulatorPluginInfo::fsxPluginIndentifier()
        {
            static const QString s("org.swift-project.plugins.simulator.fsx");
            return s;
        }

        const QString &CSimulatorPluginInfo::p3dPluginIndentifier()
        {
            static const QString s("org.swift-project.plugins.simulator.p3d");
            return s;
        }

        const QString &CSimulatorPluginInfo::fs9PluginIndentifier()
        {
            static const QString s("org.swift-project.plugins.simulator.fs9");
            return s;
        }

        const QString &CSimulatorPluginInfo::xplanePluginIndentifier()
        {
            static const QString s("org.swift-project.plugins.simulator.xplane");
            return s;
        }

        QStringList CSimulatorPluginInfo::guessDefaultPlugins()
        {
            if (BlackConfig::CBuildConfig::isRunningOnUnixPlatform())
            {
                // On UNIX we likely run XP
                return QStringList { xplanePluginIndentifier() };
            }

            return QStringList
            {
                fsxPluginIndentifier(),
                p3dPluginIndentifier(),
                xplanePluginIndentifier()
            };
        }
    } // ns
} // ns
