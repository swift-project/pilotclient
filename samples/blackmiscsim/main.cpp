/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/blackmiscfreefunctions.h"
#include "samplesfscommon.h"
#include "samplesfsx.h"
#include "samplesmodelmapping.h"

#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QTime>

/*!
 * Samples
 */
int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QTextStream streamIn(stdin);
    QTextStream streamOut(stdout);
    BlackMisc::registerMetadata();

    streamOut << "Run samples:" << endl;
    streamOut << "1 .. FS common / Simulation" << endl;
    streamOut << "2 .. FSX" << endl;
    streamOut << "3 .. Mappings" << endl;
    streamOut << "x .. exit" << endl;
    QString i = streamIn.readLine().toLower().trimmed();

    QTime t;
    t.start();
    if (i.startsWith("1"))
    {
        BlackSimTest::CSamplesFsCommon::samples(streamOut, streamIn);
    }
    else if (i.startsWith("2"))
    {
        BlackSimTest::CSamplesFsx::samplesMisc(streamOut);
    }
    else if (i.startsWith("3"))
    {
        BlackSimTest::CSamplesModelMapping::samples(streamOut, streamIn);
    }
    else if (i.startsWith("x"))
    {
        return 0;
    }

    streamOut << endl;
    streamOut << "time elapsed: " << t.elapsed() << "ms" << endl;
    streamOut << "press key to exit" << endl;
    streamIn.readLine();
    return 0;
}
