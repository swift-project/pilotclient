// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
