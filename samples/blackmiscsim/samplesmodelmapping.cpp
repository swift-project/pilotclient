/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscsim

#include "samplesmodelmapping.h"
#include "sampleutils.h"
#include "blackcore/aircraftmatcher.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/simulation/fscommon/vpilotrulesreader.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/stringutils.h"

#include <QFlags>
#include <QString>
#include <QStringList>
#include <QTextStream>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Aviation;

namespace BlackSample
{
    void CSamplesModelMapping::samples(QTextStream &streamOut, QTextStream &streamIn)
    {
        CVPilotRulesReader vpRulesReader;
        const bool s = vpRulesReader.read(true);
        streamOut << "directory: " << CVPilotRulesReader::standardMappingsDirectory() << Qt::endl;
        streamOut << "loaded: " << BlackMisc::boolToYesNo(s) << " size: " << vpRulesReader.getAsModelsFromCache().size() << Qt::endl;

        // mapper with rule set, handing over ownership
        const QString fsDir = CSampleUtils::selectDirectory(
            { QStringLiteral("P:/FlightSimulatorX (MSI)/SimObjects"),
              QStringLiteral("P:/Temp/SimObjects"),
              QStringLiteral("C:/Flight Simulator 9/Aircraft") },
            streamOut, streamIn);

        const CSimulatorInfo sim = fsDir.contains("simobjects", Qt::CaseInsensitive) ? CSimulatorInfo::FSX : CSimulatorInfo::FS9;
        CMultiSimulatorSettings multiSettings;
        const CSimulatorSettings originalSettings = multiSettings.getSettings(sim);
        CSimulatorSettings newSettings(originalSettings);
        newSettings.setModelDirectory(fsDir);
        multiSettings.setSettings(newSettings, sim); // set, but do NOT(!) save

        CAircraftCfgParser cfgParser(sim);
        streamOut << "Start reading models" << Qt::endl;
        cfgParser.startLoading(CAircraftCfgParser::CacheSkipped | CAircraftCfgParser::LoadDirectly);
        streamOut << "Read models: " << cfgParser.getAircraftCfgEntriesList().size() << Qt::endl;
        streamOut << "Ambigious models: " << cfgParser.getAircraftCfgEntriesList().detectAmbiguousTitles().join(", ") << Qt::endl;

        // sync definitions, remove redundant ones
        CAircraftMatcher matcher;
        matcher.setModelSet(vpRulesReader.getAsModelsFromCache(), CSimulatorInfo::FSX, true);

        const CAircraftIcaoCode icao("C172");
        streamOut << "Searching for " << icao << Qt::endl;
        streamOut << matcher.getModelSetRef().findByIcaoDesignators(icao, CAirlineIcaoCode()) << Qt::endl;

        // restore settings: DO NOT SAVE !!!
        multiSettings.setSettings(originalSettings, sim);
    }
} // namespace
