/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesfscommon.h"
#include "blacksim/fscommon/aircraftcfgentrieslist.h"
#include "blacksim/fscommon/aircraftindexer.h"

#include <QDebug>
#include <QFuture>
#include <QTime>
#include <QTextStream>
#include <QTemporaryFile>

using namespace BlackSim::FsCommon;

namespace BlackSimTest
{

    /*
     * Samples
     */
    int CSamplesFsCommon::samples(QTextStream &streamOut, QTextStream &streamIn)
    {
        // QString fsxDir = "P:/FlightSimulatorX (MSI)/SimObjects";
        QString fsxDir = "P:/Temp/SimObjects";
        streamOut << "Enter FSX directory:" << endl;
        streamOut << fsxDir << '\r';
        streamOut.flush();
        QString input = streamIn.readLine();
        if (!input.isEmpty()) fsxDir = input;
        streamOut << "d .. direct, b .. background" << endl;
        input = streamIn.readLine();
        if (!input.startsWith("b"))
        {
            streamOut << "reading directly" << endl;
            CAircraftCfgEntriesList entriesList(fsxDir);
            if (entriesList.existsDir())
            {
                QTime time;
                time.start();
                streamOut << "reading " << entriesList.getRootDirectory() << endl;
                entriesList.read();
                streamOut << "read entries: " << entriesList.size() << " in " << time.restart() << "ms" << endl;

                QJsonDocument doc(entriesList.toJson());
                QByteArray jsonArray(doc.toJson());
                streamOut << "write JSON array with size " << jsonArray.size() << endl;
                QTemporaryFile tempFile;
                tempFile.open();
                tempFile.write(jsonArray);
                tempFile.close();
                streamOut << "written to " << tempFile.fileName() << " in " << time.restart() << "ms" <<  endl;

                // re-read
                tempFile.open();
                jsonArray = tempFile.readAll();
                doc = QJsonDocument::fromJson(jsonArray);
                entriesList.clear();
                entriesList.convertFromJson(doc.object());
                streamOut << "read JSON array with size " << jsonArray.size() << endl;
                streamOut << "read entries from disk: " << entriesList.size() << " in " << time.restart() << "ms" << endl;
                tempFile.close();
            }
        }
        else
        {
            streamOut << "reading in background" << endl;
            QFuture<int> f = BlackSim::FsCommon::CAircraftIndexer::readInBackground(fsxDir);
            int i = 0;
            do
            {
                if (i % 20 == 0)
                {
                    streamOut << ".";
                    streamOut.flush();
                }
                QCoreApplication::processEvents(QEventLoop::AllEvents, 1000 * 5);
            }
            while (!f.isFinished());
            streamOut << endl << f.result() << " entries" << endl;
        }
        streamOut << "-----------------------------------------------" << endl;
        return 0;
    }

} // namespace
