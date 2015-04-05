/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_PQ_PQ_H
#define BLACKMISC_PQ_PQ_H

/*!
 * \namespace BlackMisc::PhysicalQuantities
 * \brief Classes for physical quantities and units such as length, mass, speed.
 */

// All units / quantities, required for the instantiations of the template
// especially as CRTP (Curiously recurring template pattern) is used.
// http://www.parashift.com/c++-faq-lite/separate-template-fn-defn-from-decl.html
// http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern

#include "blackmisc/pq/length.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/pq/mass.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/acceleration.h"
#include "blackmisc/pq/pqstring.h"

#endif // guard
