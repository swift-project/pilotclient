/* Copyright (C) 2018
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PAIR_H
#define BLACKMISC_PAIR_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/variant.h"

#include <QPair>
#include <QDBusMetaType>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        using CAnglePair  = QPair<CAngle, CAngle>;   //!< Pair of angle
        using CLengthPair = QPair<CLength, CLength>; //!< Pair of length
        using CSpeedPair  = QPair<CSpeed, CSpeed>;   //!< Pair of speed
    }
    namespace Aviation
    {
        using CAltitudePair = QPair<CAltitude, CAltitude>; //!< Pair of altitude
    }
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAnglePair)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CLengthPair)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CSpeedPair)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAltitudePair)

#endif // guard
