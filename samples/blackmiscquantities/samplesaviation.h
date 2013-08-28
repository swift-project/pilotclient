/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISCTEST_SAMPLESAVIATION_H
#define BLACKMISCTEST_SAMPLESAVIATION_H

#include "blackmisc/pqconstants.h"
#include "blackmisc/avheading.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avionavsystem.h"
#include "blackmisc/aviotransponder.h"
#include <QDebug>

namespace BlackMiscTest {

/*!
 * \brief Samples for physical quantities
 */
class CSamplesAviation
{
public:
    /*!
     * \brief Run the samples
     */
    static int samples();
};
} // namespace

#endif
