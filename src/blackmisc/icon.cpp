/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "icon.h"
#include "blackmisc/pq/angle.h"

namespace BlackMisc
{

    /*
     * Pixmap
     */
    QPixmap CIcon::toPixmap() const
    {
        return CIcons::pixmapByIndex(getIndex(), this->m_rotateDegrees);
    }

    /*
     * Icon
     */
    QIcon CIcon::toQIcon() const
    {
        return QIcon(toPixmap());
    }

    /*
     * Rotate
     */
    void CIcon::setRotation(const PhysicalQuantities::CAngle &rotate)
    {
        this->m_rotateDegrees = rotate.valueRounded(PhysicalQuantities::CAngleUnit::deg(), 0);
    }

    /*
     * String
     */
    QString CIcon::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s = QString(this->m_descriptiveText).append(" ").append(this->m_index);
        return s;
    }

} // namespace
