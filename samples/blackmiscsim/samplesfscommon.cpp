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
#include "blackmisc/sampleutils.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QString>
#include <QTemporaryFile>
#include <QTextStream>
#include <QTime>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;

namespace BlackSample
{
    void CSamplesFsCommon::samples(QTextStream &streamOut, QTextStream &streamIn)
    {
        const QString fsxDir = CSampleUtils::selectDirectory({"C:/Program Files (x86)/Microsoft Games/Microsoft Flight Simulator X/SimObjects",
                                                        "C:/Flight Simulator 9/Aircraft"}, streamOut, streamIn);

        CAircraftCfgParser parser(CSimulatorInfo(CSimulatorInfo::FSX), fsxDir);
        parser.changeRootDirectory(fsxDir);

        streamOut << "start reading, press RETURN" << endl;
        QString input = streamIn.readLine();
        Q_UNUSED(input);

        streamOut << "reading directly" << endl;
        QTime time;
        time.start();
        streamOut << "reading " << parser.getRootDirectory() << endl;
        parser.startLoading();
        streamOut << "read entries: " << parser.getAircraftCfgEntriesList().size() << " in " << time.restart() << "ms" << endl;

        CAircraftCfgEntriesList entriesList = parser.getAircraftCfgEntriesList();
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
} // namespace
