/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/track.h"

#include <QCoreApplication>

using BlackMisc::PhysicalQuantities::CAngle;
using BlackMisc::PhysicalQuantities::CAngleUnit;

namespace BlackMisc::Aviation
{
    QString CTrack::convertToQString(bool i18n) const
    {
        QString s = CAngle::convertToQString(i18n).append(" ");
        if (i18n)
        {
            return s.append(this->isMagneticTrack() ?
                                QCoreApplication::translate("Aviation", "magnetic") :
                                QCoreApplication::translate("Aviation", "true"));
        }
        else
        {
            return s.append(this->isMagneticTrack() ? "magnetic" : "true");
        }
    }
} // namespace
