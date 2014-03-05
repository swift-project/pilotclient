/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesfscommon.h"
#include "blacksim/fscommon/aircraftcfgentrieslist.h"
#include <QDebug>
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

        BlackSim::FsCommon::CAircraftCfgEntriesList entriesList(fsxDir);
        if (entriesList.existsDir())
        {
            streamOut << "reading " << entriesList.getRootDirectory() << endl;
            entriesList.read();
            streamOut << "read entries: " << entriesList.size() << endl;
            // streamOut << entriesList << endl;
        }

        streamOut << "-----------------------------------------------" << endl;
        return 0;
    }

} // namespace
