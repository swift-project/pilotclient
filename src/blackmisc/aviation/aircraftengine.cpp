// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
