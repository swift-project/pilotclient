// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/simulatorplugininfo.h"

#include <QJsonValue>
#include <QtGlobal>

#include "config/buildconfig.h"

using namespace swift::config;
using namespace swift::misc;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation, CSimulatorPluginInfo)

namespace swift::misc::simulation
{
    CSimulatorPluginInfo::CSimulatorPluginInfo(const QString &identifier, const QString &name, const QString &simulator,
                                               const QString &description, bool valid)
        : m_identifier(identifier), m_name(name), m_simulator(simulator), m_description(description), m_info(simulator),
          m_valid(valid)
    {
        Q_ASSERT_X(m_info.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");
    }

    void CSimulatorPluginInfo::convertFromJson(const QJsonObject &json)
    {
        if (json.contains("IID")) // comes from the plugin
        {
            if (!json.contains("MetaData")) { throw CJsonException("Missing 'MetaData'"); }

            // json data is already validated by CPluginManagerSimulator
            CJsonScope scope("MetaData");
            Q_UNUSED(scope)
            CValueObject::convertFromJson(json["MetaData"].toObject());
            m_valid = true;
        }
        else { CValueObject::convertFromJson(json); }

        // set info if it wasn't set already
        if (m_info.isNoSimulator() && !m_simulator.isEmpty())
        {
            if (!this->isEmulatedPlugin()) { m_info = CSimulatorInfo(m_simulator); }
        }
    }

    bool CSimulatorPluginInfo::isUnspecified() const { return m_identifier.isEmpty(); }

    bool CSimulatorPluginInfo::isEmulatedPlugin() const { return this->getIdentifier() == emulatedPluginIdentifier(); }

    QString CSimulatorPluginInfo::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        return QStringLiteral("%1 (%2)").arg(m_name, m_identifier);
    }

    const QString &CSimulatorPluginInfo::identifierFromSimulatorInfo(const CSimulatorInfo &simInfo)
    {
        static const QString e;
        if (!simInfo.isSingleSimulator()) { return e; }
        const CSimulatorInfo::Simulator s = simInfo.getSimulator();
        if (s.testFlag(CSimulatorInfo::FSX)) { return CSimulatorPluginInfo::fsxPluginIdentifier(); }
        if (s.testFlag(CSimulatorInfo::FS9)) { return CSimulatorPluginInfo::fs9PluginIdentifier(); }
        if (s.testFlag(CSimulatorInfo::P3D)) { return CSimulatorPluginInfo::p3dPluginIdentifier(); }
        if (s.testFlag(CSimulatorInfo::XPLANE)) { return CSimulatorPluginInfo::xplanePluginIdentifier(); }
        if (s.testFlag(CSimulatorInfo::FG)) { return CSimulatorPluginInfo::fgPluginIdentifier(); }
        if (s.testFlag(CSimulatorInfo::MSFS)) { return CSimulatorPluginInfo::msfsPluginIdentifier(); }
        return e;
    }

    const QString &CSimulatorPluginInfo::fsxPluginIdentifier()
    {
        static const QString s("org.swift-project.plugins.simulator.fsx");
        return s;
    }

    const QString &CSimulatorPluginInfo::p3dPluginIdentifier()
    {
        static const QString s("org.swift-project.plugins.simulator.p3d");
        return s;
    }

    const QString &CSimulatorPluginInfo::fs9PluginIdentifier()
    {
        static const QString s("org.swift-project.plugins.simulator.fs9");
        return s;
    }

    const QString &CSimulatorPluginInfo::xplanePluginIdentifier()
    {
        static const QString s("org.swift-project.plugins.simulator.xplane");
        return s;
    }

    const QString &CSimulatorPluginInfo::fgPluginIdentifier()
    {
        static const QString s("org.swift-project.plugins.simulator.flightgear");
        return s;
    }

    const QString &CSimulatorPluginInfo::msfsPluginIdentifier()
    {
        static const QString s("org.swift-project.plugins.simulator.msfs");
        return s;
    }

    const QString &CSimulatorPluginInfo::msfs2024PluginIdentifier()
    {
        static const QString s("org.swift-project.plugins.simulator.msfs2024");
        return s;
    }

    const QString &CSimulatorPluginInfo::emulatedPluginIdentifier()
    {
        static const QString s("org.swift-project.plugins.simulator.emulated");
        return s;
    }

    const QStringList &CSimulatorPluginInfo::allIdentifiers()
    {
        static const QStringList identifiers({ fsxPluginIdentifier(), p3dPluginIdentifier(), xplanePluginIdentifier(),
                                               fs9PluginIdentifier(), emulatedPluginIdentifier(), fgPluginIdentifier(),
                                               msfsPluginIdentifier() });
        return identifiers;
    }

    QStringList CSimulatorPluginInfo::guessDefaultPlugins()
    {
        if (CBuildConfig::isRunningOnUnixPlatform())
        {
            // On UNIX we likely run XP
            return QStringList { xplanePluginIdentifier(), fgPluginIdentifier() };
        }

        return QStringList { fsxPluginIdentifier(), msfsPluginIdentifier(), p3dPluginIdentifier(),
                             xplanePluginIdentifier(), fgPluginIdentifier() };
    }
} // namespace swift::misc::simulation
