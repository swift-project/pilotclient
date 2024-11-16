// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscquantities

#ifndef SWIFT_SAMPLE_SAMPLESAVIATION_H
#define SWIFT_SAMPLE_SAMPLESAVIATION_H

class QTextStream;

namespace swift::sample
{

    //! Samples for aviation
    class CSamplesAviation
    {
    public:
        //! Run the samples
        static int samples(QTextStream &out);
    };
} // namespace swift::sample

#endif
