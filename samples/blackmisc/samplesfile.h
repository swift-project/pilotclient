/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmisc

#ifndef BLACKSAMPLE_SAMPLESFILE_H
#define BLACKSAMPLE_SAMPLESFILE_H

#include <QTextStream>

namespace BlackSample
{
    //! Samples for file/directory
    class CSamplesFile
    {
    public:
        //! Run the samples
        static int samples(QTextStream &out);
    };
} // namespace

#endif
