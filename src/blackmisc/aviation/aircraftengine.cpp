/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftengine.h"
#include "blackmisc/stringutils.h"

#include <QtGlobal>

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CAircraftEngine)

namespace BlackMisc::Aviation
{
    CAircraftEngine::CAircraftEngine(int number, bool on) : m_number(number), m_on(on)
    {
        Q_ASSERT_X(number > 0, "CAircraftEngine", "Engine numbers have to be > 1");
    }

    void CAircraftEngine::setNumber(int number)
    {
        Q_ASSERT_X(number > 0, "setNumber", "Engine numbers have to be > 1");
        m_number = number;
    }

    QString CAircraftEngine::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("%1: %2").arg(m_number).arg(BlackMisc::boolToOnOff(m_on));
    }
} // namespace
