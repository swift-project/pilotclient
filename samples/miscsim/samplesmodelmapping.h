// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscsim

#ifndef SWIFT_SAMPLE_SAMPLESMODELMAPPING_H
#define SWIFT_SAMPLE_SAMPLESMODELMAPPING_H

class QTextStream;

namespace swift::sample
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
