// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/aviation/transponder.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/simulation/fscommon/bcdconversions.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

namespace BlackMisc::Simulation::FsCommon
{
    quint32 CBcdConversions::comFrequencyToBcdHz(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency)
    {
        // FSX documentation is wrong, we need to use kHz + 2 digits, not Hz
        const double f = comFrequency.valueRounded(CFrequencyUnit::kHz(), 0) / 10;
        const quint32 fInt = dec2Bcd(static_cast<quint32>(f));
        return fInt;
    }

    quint32 CBcdConversions::transponderCodeToBcd(const BlackMisc::Aviation::CTransponder &transponder)
    {
        // FSX documentation is wrong, we need to use kHz + 2 digits, not Hz
        quint32 t = static_cast<quint32>(transponder.getTransponderCode());
        t = dec2Bcd(t);
        return t;
    }

    quint32 CBcdConversions::hornerScheme(quint32 num, quint32 divider, quint32 factor)
    {
        quint32 remainder = 0, quotient = 0, result = 0;
        remainder = num % divider;
        quotient = num / divider;
        if (!(quotient == 0 && remainder == 0))
        {
            result += hornerScheme(quotient, divider, factor) * factor + remainder;
        }
        return result;
    }
} // namespacee
