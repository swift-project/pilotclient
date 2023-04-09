/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscquantities

#ifndef BLACKSAMPLE_SAMPLESAVIATION_H
#define BLACKSAMPLE_SAMPLESAVIATION_H

class QTextStream;

namespace BlackSample
{

    //! Samples for aviation
    class CSamplesAviation
    {
    public:
        //! Run the samples
        static int samples(QTextStream &out);
    };
} // namespace

#endif
