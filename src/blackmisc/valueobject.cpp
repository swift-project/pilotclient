/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "valueobject.h"
#include "iconlist.h"

namespace BlackMisc
{
    CIcon CEmpty::toIcon() const
    {
        return CIconList::iconByIndex(CIcons::StandardIconUnknown16);
    }

    QPixmap CEmpty::toPixmap() const
    {
        return this->toIcon().toPixmap();
    }
}
