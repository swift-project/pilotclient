// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/application.h"
#include "misc/directoryutils.h"
#include "samplesalgorithm.h"
#include "sampleschangeobject.h"
#include "samplescontainer.h"
#include "samplesjson.h"
#include "samplesmetadata.h"
#include "samplesperformance.h"
#include "samplesfile.h"

#include <stdio.h>
#include <QCoreApplication>
#include <QString>
#include <QTextStream>

//! \file
//! \ingroup samplekmisc

using namespace swift::misc;
using namespace BlackSample;
using namespace swift::core;

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
        qtout << Qt::endl;
        qtout << "1 .. JSON" << Qt::endl;
        qtout << "2 .. Change object" << Qt::endl;
        qtout << "3a .. Containers" << Qt::endl;
        qtout << "3b .. Callsign QMap vs QHash" << Qt::endl;
        qtout << "4 .. Metadata" << Qt::endl;
        qtout << "6a .. Performance create / copy / ..." << Qt::endl;
        qtout << "6b .. 40/20 Interpolator scenario" << Qt::endl;
        qtout << "6c .. JSON performance" << Qt::endl;
        qtout << "6d .. JSON model performance (database vs. own JSON)" << Qt::endl;
        qtout << "6e .. string utils vs.regex" << Qt::endl;
        qtout << "6f .. string concatenation (+=, arg, ..)" << Qt::endl;
        qtout << "6g .. const &QString vs. QStringLiteral" << Qt::endl;
        qtout << "7 .. Algorithms" << Qt::endl;
        qtout << "8 .. File/Directory" << Qt::endl;
        qtout << "-----" << Qt::endl;
        qtout << "x .. Bye" << Qt::endl;
        const QString s = qtin.readLine().toLower().trimmed();

        if (s.startsWith("1")) { CSamplesJson::samples(); }
        else if (s.startsWith("2")) { CSamplesChangeObject::samples(); }
        else if (s.startsWith("3a")) { CSamplesContainer::samples(); }
        else if (s.startsWith("3b")) { CSamplesPerformance::sampleQMapVsQHashByCallsign(qtout); }
        else if (s.startsWith("4")) { CSamplesMetadata::samples(); }
        else if (s.startsWith("6a")) { CSamplesPerformance::samplesMisc(qtout); }
        else if (s.startsWith("6b")) { CSamplesPerformance::interpolatorScenario(qtout, 40, 20); }
        else if (s.startsWith("6c")) { CSamplesPerformance::samplesJson(qtout); }
        else if (s.startsWith("6d")) { CSamplesPerformance::samplesJsonModelAndLivery(qtout); }
        else if (s.startsWith("6e")) { CSamplesPerformance::samplesStringUtilsVsRegEx(qtout); }
        else if (s.startsWith("6f")) { CSamplesPerformance::samplesStringConcat(qtout); }
        else if (s.startsWith("6g")) { CSamplesPerformance::samplesStringLiteralVsConstQString(qtout); }
        else if (s.startsWith("7")) { CSamplesAlgorithm::samples(); }
        else if (s.startsWith("8")) { CSamplesFile::samples(qtout); }
        else if (s.startsWith("x")) { break; }
    }
    while (true);
    return 0;
}
