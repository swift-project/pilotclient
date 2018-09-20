/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscsim

#include "samplesfscommon.h"
#include "samplesfsx.h"
#include "samplesp3d.h"
#include "samplesmodelmapping.h"
#include "samplesvpilotrules.h"
#include "blackcore/application.h"
#include "blackmisc/directoryutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QTime>
#include <QtGlobal>

using namespace BlackMisc;

//! main
int main(int argc, char *argv[])
{
    QCoreApplication qa(argc, argv);
    BlackCore::CApplication a;
    Q_UNUSED(a);
    Q_UNUSED(qa);

    QTextStream streamIn(stdin);
    QTextStream streamOut(stdout);

    streamOut << "Run samples:" << endl;
    streamOut << "1 .. FS common / Simulation (with cfg files reading)" << endl;
    streamOut << "2 .. FSX" << endl;
    streamOut << "3 .. Mappings" << endl;
    streamOut << "4 .. vPilot rules" << endl;
    streamOut << "5 .. P3D cfg files" << endl;
    streamOut << "x .. exit" << endl;
    QString i = streamIn.readLine().toLower().trimmed();

    QTime t;
    t.start();
    if (i.startsWith("1"))
    {
        BlackSample::CSamplesFsCommon::samples(streamOut, streamIn);
    }
    else if (i.startsWith("2"))
    {
        BlackSample::CSamplesFsx::samplesMisc(streamOut);
    }
    else if (i.startsWith("3"))
    {
        BlackSample::CSamplesModelMapping::samples(streamOut, streamIn);
    }
    else if (i.startsWith("4"))
    {
        BlackSample::CSamplesVPilotRules::samples(streamOut, streamIn);
    }
    else if (i.startsWith("5"))
    {
        BlackSample::CSamplesP3D::samplesMisc(streamOut);
    }
    else if (i.startsWith("x"))
    {
        streamOut << "terminating" << endl;
    }

    streamOut << endl;
    streamOut << "time elapsed: " << t.elapsed() << "ms" << endl;
    streamOut << "press key to exit" << endl;
    streamIn.readLine();
    return EXIT_SUCCESS;
}
