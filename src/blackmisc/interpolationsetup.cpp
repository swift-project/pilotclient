/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/interpolationsetup.h"
#include "stringutils.h"

namespace BlackMisc
{
    CInterpolationAndRenderingSetup::CInterpolationAndRenderingSetup()
    { }

    QString CInterpolationAndRenderingSetup::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s("Setup: debug sim: ");
        s += boolToYesNo(this->m_simulatorDebugMessages);
        s += " debug interpolator: ";
        s += boolToYesNo(this->m_interpolatorDebugMessage);
        s += " force full interpolation: ";
        s += boolToYesNo(this->m_forceFullInterpolation);
        return s;
    }

    CVariant CInterpolationAndRenderingSetup::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexInterpolatorDebugMessages:
            return CVariant::fromValue(m_interpolatorDebugMessage);
        case IndexSimulatorDebugMessages:
            return CVariant::fromValue(m_simulatorDebugMessages);
        case IndexForceFullInterpolation:
            return CVariant::fromValue(m_forceFullInterpolation);
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CInterpolationAndRenderingSetup::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (index.isMyself())
        {
            *this = variant.value<CInterpolationAndRenderingSetup>();
            return;
        }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexInterpolatorDebugMessages:
            this->m_interpolatorDebugMessage = variant.toBool();
            break;
        case IndexSimulatorDebugMessages:
            this->m_simulatorDebugMessages = variant.toBool();
            break;
        case IndexForceFullInterpolation:
            this->m_forceFullInterpolation = variant.toBool();
            break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }
} // ns
