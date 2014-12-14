/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorinfo.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;

namespace BlackSim
{
    CSimulatorInfo::CSimulatorInfo(const QString &shortname, const QString &fullname) : m_fullName(fullname), m_shortName(shortname)
    { }

    CSimulatorInfo::CSimulatorInfo() :  m_fullName("Unknown"), m_shortName("Unknown")
    {}

    CVariant CSimulatorInfo::getSimulatorSetupValue(int index) const
    {
        return this->m_simsetup.value(index);
    }

    QString CSimulatorInfo::getSimulatorSetupValueAsString(int index) const
    {
        CVariant qv = getSimulatorSetupValue(index);
        Q_ASSERT(qv.canConvert<QString>());
        return qv.toQString();
    }

    void CSimulatorInfo::setSimulatorSetup(const BlackMisc::CPropertyIndexVariantMap &setup)
    {
        this->m_simsetup = setup;
    }

    bool CSimulatorInfo::isSameSimulator(const CSimulatorInfo &otherSimulator) const
    {
        return this->getFullName() == otherSimulator.getFullName() &&
               this->getShortName() == otherSimulator.getShortName();
    }

    QString CSimulatorInfo::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QString(this->m_shortName).append(" (").append(this->m_fullName).append(")");
    }
}
