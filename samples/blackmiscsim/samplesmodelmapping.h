/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscsim

#ifndef BLACKSAMPLE_SAMPLESMODELMAPPING_H
#define BLACKSAMPLE_SAMPLESMODELMAPPING_H

class QTextStream;

namespace BlackSample
{
    //! Samples for model mapping classes
    class CSamplesModelMapping
    {
    public:
        //! Run the samples
        static void samples(QTextStream &streamOut, QTextStream &streamIn);
    };
} // namespace

#endif
