/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackmisc/directoryutils.h"
#include "samplesalgorithm.h"
#include "sampleschangeobject.h"
#include "samplescontainer.h"
#include "samplesjson.h"
#include "samplesmetadata.h"
#include "samplesperformance.h"

#include <stdio.h>
#include <QCoreApplication>
#include <QString>
#include <QTextStream>

//! \file
//! \ingroup sampleblackmisc

using namespace BlackMisc;
using namespace BlackSample;
using namespace BlackCore;

//! main
int main(int argc, char *argv[])
{
    // I use CGuiApplication and not core application
    // otherwise no QPixmap metadata (metadata sample)
    QCoreApplication qa(argc, argv);
    CApplication a(CApplicationInfo::Sample);
    Q_UNUSED(qa);
    Q_UNUSED(a);
    QTextStream qtout(stdout);
    QTextStream qtin(stdin);

    do
    {
        qtout << endl;
        qtout << "1 .. JSON" << endl;
        qtout << "2 .. Change object" << endl;
        qtout << "3a .. Containers" << endl;
        qtout << "3b .. Callsign QMap vs QHash" << endl;
        qtout << "4 .. Metadata" << endl;
        qtout << "6a .. Performance create / copy / ..." << endl;
        qtout << "6b .. 40/20 Interpolator scenario" << endl;
        qtout << "6c .. JSON performance" << endl;
        qtout << "6d .. JSON model performance (database vs. own JSON)" << endl;
        qtout << "6e .. string utils vs.regex" << endl;
        qtout << "6f .. string concatenation (+=, arg, ..)" << endl;
        qtout << "6g .. const &QString vs. QStringLiteral" << endl;
        qtout << "7 .. Algorithms" << endl;
        qtout << "-----" << endl;
        qtout << "x .. Bye" << endl;
        QString s = qtin.readLine().toLower().trimmed();

        if (s.startsWith("1")) { CSamplesJson::samples(); }
        else if (s.startsWith("2"))  { CSamplesChangeObject::samples(); }
        else if (s.startsWith("3a")) { CSamplesContainer::samples(); }
        else if (s.startsWith("3b")) { CSamplesPerformance::sampleQMapVsQHashByCallsign(qtout); }
        else if (s.startsWith("4"))  { CSamplesMetadata::samples(); }
        else if (s.startsWith("6a")) { CSamplesPerformance::samplesMisc(qtout); }
        else if (s.startsWith("6b")) { CSamplesPerformance::interpolatorScenario(qtout, 40, 20); }
        else if (s.startsWith("6c")) { CSamplesPerformance::samplesJson(qtout); }
        else if (s.startsWith("6d")) { CSamplesPerformance::samplesJsonModelAndLivery(qtout); }
        else if (s.startsWith("6e")) { CSamplesPerformance::samplesStringUtilsVsRegEx(qtout); }
        else if (s.startsWith("6f")) { CSamplesPerformance::samplesStringConcat(qtout); }
        else if (s.startsWith("6g")) { CSamplesPerformance::samplesStringLiteralVsConstQString(qtout); }
        else if (s.startsWith("7"))  { CSamplesAlgorithm::samples(); }
        else if (s.startsWith("x"))  { break; }
    }
    while (true);
    return 0;
}
