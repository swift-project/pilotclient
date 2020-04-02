/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscsim

#include "samplesfscommon.h"
#include "samplesfsx.h"
#include "samplesp3d.h"
#include "samplesfsuipc.h"
#include "samplesmodelmapping.h"
#include "samplesvpilotrules.h"
#include "blackcore/application.h"
#include "blackmisc/directoryutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QElapsedTimer>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackSample;

//! main
int main(int argc, char *argv[])
{
    QCoreApplication qa(argc, argv);
    BlackCore::CApplication a;
    Q_UNUSED(a);
    Q_UNUSED(qa);

    QTextStream streamIn(stdin);
    QTextStream streamOut(stdout);

    bool run = true;
    QElapsedTimer t;
    while (run)
    {
        streamOut << "Run samples:" << Qt::endl;
        streamOut << "1 .. FS common / Simulation (with cfg files reading)" << Qt::endl;
        streamOut << "2 .. FSX"           << Qt::endl;
        streamOut << "3 .. Mappings"      << Qt::endl;
        streamOut << "4 .. vPilot rules"  << Qt::endl;
        streamOut << "5 .. P3D cfg files" << Qt::endl;
        streamOut << "6 .. FSUIPC read"   << Qt::endl;
        streamOut << "x .. exit" << Qt::endl;
        QString i = streamIn.readLine().toLower().trimmed();

        t.start();
        if (i.startsWith("1")) { CSamplesFsCommon::samples(streamOut, streamIn); }
        else if (i.startsWith("2")) { CSamplesFsx::samplesMisc(streamOut); }
        else if (i.startsWith("3")) { CSamplesModelMapping::samples(streamOut, streamIn); }
        else if (i.startsWith("4")) { CSamplesVPilotRules::samples(streamOut, streamIn); }
        else if (i.startsWith("5")) { CSamplesP3D::samplesMisc(streamOut); }
        else if (i.startsWith("6")) { CSamplesFsuipc::samplesFsuipc(streamOut); }
        else if (i.startsWith("x")) { run = false; streamOut << "terminating" << Qt::endl; }

        streamOut << Qt::endl;
        streamOut << Qt::endl;
    }

    streamOut << Qt::endl;
    streamOut << "time elapsed: " << t.elapsed() << "ms" << Qt::endl;
    streamOut << "press key to exit" << Qt::endl;
    streamIn.readLine();
    return EXIT_SUCCESS;
}
