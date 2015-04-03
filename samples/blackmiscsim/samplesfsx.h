/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMTEST_SAMPLESFSX_H
#define BLACKSIMTEST_SAMPLESFSX_H

#include <QTextStream>

namespace BlackSimTest
{
    //! Samples for FSX classes
    class CSamplesFsx
    {
    public:
        //! Run the misc. samples
        static int samplesMisc(QTextStream &streamOut);

        //! Driver test / SimConnect test
        static int driverTest(QTextStream &streamOut);

    };
} // namespace

#endif
