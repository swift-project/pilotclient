/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftparts.h"

namespace BlackMisc
{
    namespace Aviation
    {
        QString CAircraftParts::convertToQString(bool i18n) const
        {
            QString s;
            s += m_lights.toQString(i18n);
            s += " gear down: ";
            s += m_gearDown;
            s += " flaps pct: ";
            s += m_flapsPercent;
            s += " spoilers out: ";
            s += m_spoilersOut;
            s += " engines on: ";
            s += m_engines.toQString(i18n);
            s += " on ground: ";
            s += m_isOnGround;
            return s;
        }
    } // namespace
} // namespace
