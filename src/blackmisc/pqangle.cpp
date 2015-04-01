/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "iconlist.h"
#include "pqangle.h"
#include "blackmisc/aviation/heading.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        BlackMisc::CIcon CAngle::toIcon() const
        {
            BlackMisc::CIcon i = CIconList::iconByIndex(CIcons::StandardIconArrowMediumNorth16);
            i.setRotation(*this);
            return i;
        }

        double CAngle::piFactor() const
        {
            return BlackMisc::Math::CMath::round(this->value(CAngleUnit::rad()) / BlackMisc::Math::CMath::PI() , 6);
        }

        const double &CAngle::PI()
        {
            return BlackMisc::Math::CMath::PI();
        }
    }
}
