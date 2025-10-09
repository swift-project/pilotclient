// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/fscommon/bcdconversions.h"

#include "misc/aviation/transponder.h"
#include "misc/pq/units.h"

using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;

namespace swift::misc::simulation::fscommon
{
    quint32 CBcdConversions::comFrequencyToBcdHz(const swift::misc::physical_quantities::CFrequency &comFrequency)
    {
        // FSX documentation is wrong, we need to use kHz + 2 digits, not Hz
        const double f = comFrequency.valueRounded(CFrequencyUnit::kHz(), 0) / 10;
        const quint32 fInt = dec2Bcd(static_cast<quint32>(f));
        return fInt;
    }

    quint32 CBcdConversions::transponderCodeToBcd(const swift::misc::aviation::CTransponder &transponder)
    {
        // FSX documentation is wrong, we need to use kHz + 2 digits, not Hz
        auto t = static_cast<quint32>(transponder.getTransponderCode());
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
} // namespace swift::misc::simulation::fscommon
