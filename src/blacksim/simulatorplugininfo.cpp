/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorplugininfo.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;

namespace BlackSim
{
    
    CSimulatorPluginInfo::CSimulatorPluginInfo(const QJsonObject &json)
    {
        if (json["IID"].toString() != QStringLiteral("org.swift.pilotclient.BlackCore.SimulatorInterface"))
        {
            return;
        }

        if (!json["MetaData"].isObject())
        {
            return;
        }

        QJsonObject data = json["MetaData"].toObject();
        if (data["name"].isUndefined() || data["simulator"].isUndefined())
        {
            return;
        }

        m_name = data["name"].toString();
        m_simulator = data["simulator"].toString();

        if (data["description"].isString())
        {
            m_description = data["description"].toString();
        }

        m_valid = true;
    }
    
    bool CSimulatorPluginInfo::isUnspecified() const
    {
        return m_name.isEmpty();
    }

    CVariant CSimulatorPluginInfo::getSimulatorSetupValue(int index) const
    {
        return this->m_simsetup.value(index);
    }

    QString CSimulatorPluginInfo::getSimulatorSetupValueAsString(int index) const
    {
        CVariant qv = getSimulatorSetupValue(index);
        Q_ASSERT(qv.canConvert<QString>());
        return qv.toQString();
    }

    void CSimulatorPluginInfo::setSimulatorSetup(const BlackMisc::CPropertyIndexVariantMap &setup)
    {
        this->m_simsetup = setup;
    }

    QString CSimulatorPluginInfo::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QString("%1 (%2)").arg(m_name, m_simulator);
    }
}
