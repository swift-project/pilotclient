// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/track.h"

#include <QCoreApplication>

using swift::misc::physical_quantities::CAngle;
using swift::misc::physical_quantities::CAngleUnit;

namespace swift::misc::aviation
{
    QString CTrack::convertToQString(bool i18n) const
    {
        QString s = CAngle::convertToQString(i18n).append(" ");
        if (i18n)
        {
            return s.append(this->isMagneticTrack() ? QCoreApplication::translate("Aviation", "magnetic") :
                                                      QCoreApplication::translate("Aviation", "true"));
        }
        else { return s.append(this->isMagneticTrack() ? "magnetic" : "true"); }
    }
} // namespace swift::misc::aviation
