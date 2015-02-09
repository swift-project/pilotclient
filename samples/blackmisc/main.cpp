/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include <QCoreApplication>
#include "sampleschangeobject.h"
#include "samplesmetadata.h"
#include "samplescontainer.h"
#include "samplesjson.h"
#include "samplesvariant.h"
#include "samplesperformance.h"
#include "samplesalgorithm.h"
#include "samplesconcurrent.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/pqallquantities.h"

using namespace BlackMisc;
using namespace BlackMiscTest;

/*!
 * Sample tests
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);;
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
        qtout << "5 .. Variant" << endl;
        qtout << "6 .. Performance" << endl;
        qtout << "7 .. Algorithms" << endl;
        qtout << "8 .. Concurrent (thread)" << endl;

        qtout << "-----" << endl;
        qtout << "x .. Bye" << endl;
        QString s = qtin.readLine().toLower().trimmed();

        if (s.startsWith("1")) { CSamplesJson::samples(); }
        else if (s.startsWith("2")) { CSamplesChangeObject::samples(); }
        else if (s.startsWith("3")) { CSamplesContainer::samples(); }
        else if (s.startsWith("4")) { CSamplesMetadata::samples(); }
        else if (s.startsWith("5")) { CSamplesVariant::samples(); }
        else if (s.startsWith("6")) { CSamplesPerformance::samples(); }
        else if (s.startsWith("7")) { CSamplesAlgorithm::samples(); }
        else if (s.startsWith("8")) { CSamplesConcurrent::samples(s, qtout, qtin); }
        else if (s.startsWith("x")) { break; }
    }
    while (true);
    return 0;
}
