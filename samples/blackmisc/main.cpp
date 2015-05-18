/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include <QGuiApplication>
#include "sampleschangeobject.h"
#include "samplesmetadata.h"
#include "samplescontainer.h"
#include "samplesjson.h"
#include "samplesperformance.h"
#include "samplesalgorithm.h"
#include "samplesconcurrent.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/pq/pq.h"

using namespace BlackMisc;
using namespace BlackMiscTest;

//! Sample tests
int main(int argc, char *argv[])
{
    // I use QGuiApplication and not core application
    // otherwise no QPixmap metadata (metadata sample)
    QGuiApplication a(argc, argv);
    Q_UNUSED(a);

    QTextStream qtout(stdout);
    QTextStream qtin(stdin);

    BlackMisc::initResources();
    BlackMisc::registerMetadata();

    do
    {
        qtout << "1 .. JSON" << endl;
        qtout << "2 .. Change object" << endl;
        qtout << "3 .. Containers" << endl;
        qtout << "4 .. Metadata" << endl;
        qtout << "6a .. Performance create / copy / ..." << endl;
        qtout << "6b .. 25/100 Performance impl. type" << endl;
        qtout << "6c .. 25/20 Performance impl. type" << endl;
        qtout << "6d .. 40/20 Interpolator scenario" << endl;
        qtout << "7 .. Algorithms" << endl;
        qtout << "8 .. Concurrent (thread)" << endl;
        qtout << "-----" << endl;
        qtout << "x .. Bye" << endl;
        QString s = qtin.readLine().toLower().trimmed();

        if (s.startsWith("1")) { CSamplesJson::samples(); }
        else if (s.startsWith("2")) { CSamplesChangeObject::samples(); }
        else if (s.startsWith("3")) { CSamplesContainer::samples(); }
        else if (s.startsWith("4")) { CSamplesMetadata::samples(); }
        else if (s.startsWith("6a")) { CSamplesPerformance::samplesMisc(qtout); }
        else if (s.startsWith("6b")) { CSamplesPerformance::samplesImplementationType(qtout, 25, 100); }
        else if (s.startsWith("6c")) { CSamplesPerformance::samplesImplementationType(qtout, 25, 20); }
        else if (s.startsWith("6d")) { CSamplesPerformance::interpolatorScenario(qtout, 40, 20); }
        else if (s.startsWith("7")) { CSamplesAlgorithm::samples(); }
        else if (s.startsWith("8")) { CSamplesConcurrent::samples(s, qtout, qtin); }
        else if (s.startsWith("x")) { break; }
    }
    while (true);
    return 0;
}
