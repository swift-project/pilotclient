/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesfscommon.h"
#include "blacksim/fscommon/aircraftcfgentrieslist.h"
#include "blacksim/fscommon/aircraftindexer.h"
#include <QDebug>
#include <QFuture>
#include <QTest>
#include <QTextStream>

namespace BlackSimTest
{

    /*
     * Samples
     */
    int CSamplesFsCommon::samples()
    {
        QTextStream streamIn(stdin);
        QTextStream streamOut(stdout);
        QString fsxDir = "P:/FlightSimulatorX (MSI)/SimObjects";
        streamOut << "Enter FSX directory:" << endl;
        streamOut << fsxDir << '\r';
        streamOut.flush();
        QString input = streamIn.readLine();
        if (!input.isEmpty()) fsxDir = input;
        streamOut << "d .. direct, b .. background" << endl;
        input = streamIn.readLine();
        if (!input.startsWith("b"))
        {
            qDebug() << "reading directly";
            BlackSim::FsCommon::CAircraftCfgEntriesList entriesList(fsxDir);
            if (entriesList.existsDir())
            {
                streamOut << "reading " << entriesList.getRootDirectory() << endl;
                entriesList.read();
                streamOut << "read entries: " << entriesList.size() << endl;
                // streamOut << entriesList << endl;
            }
        }
        else
        {
            qDebug() << "reading in background";
            QFuture<int> f = BlackSim::FsCommon::CAircraftIndexer::readInBackground(fsxDir);
            do
            {
                streamOut << ".";
                streamOut.flush();
                QTest::qSleep(1000 * 3);
            }
            while (!f.isFinished());
            streamOut << endl << f.result() << " entries" << endl;
        }
        streamOut << "-----------------------------------------------" << endl;
        return 0;
    }

} // namespace
