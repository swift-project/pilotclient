/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQUNITSALL_H
#define BLACKMISC_PQUNITSALL_H

// All units / quantities, required for the instantiations of the template
// especially as CRTP (Curiously recurring template pattern) is used.
// http://www.parashift.com/c++-faq-lite/separate-template-fn-defn-from-decl.html
// http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern

#include "blackmisc/pqlength.h"
#include "blackmisc/pqfrequency.h"
#include "blackmisc/pqpressure.h"
#include "blackmisc/pqmass.h"
#include "blackmisc/pqspeed.h"
#include "blackmisc/pqtemperature.h"
#include "blackmisc/pqangle.h"
#include "blackmisc/pqtime.h"
#include "blackmisc/pqacceleration.h"
#include "blackmisc/pqunits.h"
#include "blackmisc/pqstring.h"

#endif // guard
