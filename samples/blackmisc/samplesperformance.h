/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISCTEST_SAMPLESPERFORMANCEINDEX_H
#define BLACKMISCTEST_SAMPLESPERFORMANCEINDEX_H

#include <QTextStream>

namespace BlackMiscTest
{
    //! Samples for our containers
    class CSamplesPerformance
    {
    public:
        //! Copy, create, RegEx
        static int samplesMisc(QTextStream &out);

        //! Impact of implementation type
        static int samplesImplementationType(QTextStream &out, int numberOfCallsigns, int numberOfTimes);

    };
} // namespace

#endif
