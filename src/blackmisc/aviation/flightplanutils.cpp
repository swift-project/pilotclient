/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "flightplanutils.h"
#include <QStringBuilder>

namespace BlackMisc
{
    namespace Aviation
    {
        CFlightPlanUtils::AirlineRemarks CFlightPlanUtils::parseFlightPlanAirlineRemarks(const QString &remarks)
        {
            AirlineRemarks ar;
            if (remarks.isEmpty()) { return ar; }
            const QString r = remarks.toUpper();
            if (r.contains("/CALLSIGN"))
            {
                // used similar to radio telephony
                ar.radioTelephony = cut(r, "/CALLSIGN");
            }
            else if (r.contains("/RT"))
            {
                // radio telephony designator
                ar.radioTelephony = cut(r, "/RT");
            }
            if (r.contains("/OPR"))
            {
                // operator, e.g. British airways
                ar.flightOperator = cut(r, "/OPR");
            }
            return ar;
        }

        QString CFlightPlanUtils::cut(const QString &remarks, const QString &marker)
        {
            const int maxIndex = remarks.size() - 1;
            int f = remarks.indexOf(marker);
            if (f < 0) { return ""; }
            f += marker.length();
            if (maxIndex <= f) { return ""; }
            int to = remarks.indexOf(' ', f + 1);
            if (to < 0) { to = maxIndex; } // no more spaces
            const QString cut = remarks.mid(f, to - f).replace('/', ' '); // variations like /OPR/EASYJET/
            // problem is that this cuts something like "Uzbekistan airways"
            return cut;
        }
    } // namespace
} // namespace

// RT/KESTREL OPR/MYTRAVEL REG/G-DAJC SEL/FP-ES PER/C NAV/RNP10
// VFPS = VATSIM Flightplan Prefile System
// OPR/UAL CALLSIGN/UNITED
