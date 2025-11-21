// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscsim

#include "samplesfscommon.h"

#include <QByteArray>
#include <QDir>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QString>
#include <QTemporaryFile>
#include <QTextStream>
#include <QtGlobal>

#include "sampleutils.h"

#include "misc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "misc/simulation/fscommon/aircraftcfgparser.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/simulatorinfo.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;
using namespace swift::misc::simulation::settings;

namespace swift::sample
{
    void CSamplesFsCommon::samples(QTextStream &streamOut, QTextStream &streamIn)
    {
        const QString fsDir = CSampleUtils::selectDirectory(
            { "C:/Program Files (x86)/Microsoft Games/Microsoft Flight Simulator X/SimObjects",
              "C:/Flight Simulator 9/Aircraft" },
            streamOut, streamIn);

        const QDir dir(fsDir);
        if (!dir.exists())
        {
            streamOut << "Directory does not exist:" << dir.absolutePath();
            return;
        }

        const CSimulatorInfo sim =
            fsDir.contains("simobjects", Qt::CaseInsensitive) ? CSimulatorInfo::FSX : CSimulatorInfo::FS9;
        CMultiSimulatorSettings multiSettings;
        const CSimulatorSettings originalSettings = multiSettings.getSettings(sim);
        CSimulatorSettings newSettings(originalSettings);
        newSettings.setModelDirectory(fsDir);
        multiSettings.setSettings(newSettings, sim); // set, but do NOT(!) save

        CAircraftCfgParser parser(sim);
        streamOut << "start reading, press RETURN" << Qt::endl;
        QString input = streamIn.readLine();
        Q_UNUSED(input);

        streamOut << "reading directly" << Qt::endl;
        QElapsedTimer time;
        time.start();
        streamOut << "reading " << parser.getFirstModelDirectoryOrDefault() << Qt::endl;
        parser.startLoading();
        streamOut << "read entries: " << parser.getAircraftCfgEntriesList().size() << " in " << time.restart() << "ms"
                  << Qt::endl;

        CAircraftCfgEntriesList entriesList = parser.getAircraftCfgEntriesList();
        QJsonDocument doc(entriesList.toJson());
        QByteArray jsonArray(doc.toJson());
        streamOut << "write JSON array with size " << jsonArray.size() << Qt::endl;
        QTemporaryFile tempFile;
        bool res = tempFile.open();
        SWIFT_VERIFY_X(res, Q_FUNC_INFO, "Failed to open file");
        tempFile.write(jsonArray);
        tempFile.close();
        streamOut << "written to " << tempFile.fileName() << " in " << time.restart() << "ms" << Qt::endl;

        // re-read
        res = tempFile.open();
        SWIFT_VERIFY_X(res, Q_FUNC_INFO, "Failed to open file");
        jsonArray = tempFile.readAll();
        doc = QJsonDocument::fromJson(jsonArray);
        entriesList.clear();
        entriesList.convertFromJson(doc.object());
        streamOut << "read JSON array with size " << jsonArray.size() << Qt::endl;
        streamOut << "read entries from disk: " << entriesList.size() << " in " << time.restart() << "ms" << Qt::endl;
        tempFile.close();

        // restore settings: DO NOT SAVE !!!
        multiSettings.setSettings(originalSettings, sim);
    }
} // namespace swift::sample
