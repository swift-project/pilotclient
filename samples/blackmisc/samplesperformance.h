/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmisc

#ifndef BLACKSAMPLE_SAMPLESPERFORMANCE_H
#define BLACKSAMPLE_SAMPLESPERFORMANCE_H

#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/simulation/aircraftmodellist.h"

#include <QTextStream>
#include <QtGlobal>

class QTextStream;

namespace BlackSample
{
    //! Samples for our containers
    class CSamplesPerformance
    {
    public:
        //! Copy, create, RegEx
        static int samplesMisc(QTextStream &out);

        //! Impact of implementation type
        static int samplesImplementationType(QTextStream &out, int numberOfCallsigns, int numberOfTimes);

        //! Interpolator scenario
        static int interpolatorScenario(QTextStream &out, int numberOfCallsigns, int numberOfTimes);

        //! JSON (de)serialization
        static int samplesJson(QTextStream &out);

        //! JSON loading (database vs. own format
        static int samplesJsonModel(QTextStream &out);

        //! String manipulation (inserter)
        static int samplesStringUtilsVsRegEx(QTextStream &out);

        //! String manipulation (concatenation)
        static int samplesStringConcat(QTextStream &out);

        //! Situation values for testing
        static BlackMisc::Aviation::CAircraftSituationList createSituations(qint64 baseTimeEpoch, int numberOfCallsigns, int numberOfTimes);

        //! Model values for testing
        static BlackMisc::Simulation::CAircraftModelList createModels(int numberOfModels, int numberOfMemoParts);

    private:
        static const qint64 DeltaTime = 10;

    };
} // namespace

#endif
