// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscsim

#include "samplesmodelmapping.h"
#include "sampleutils.h"
#include "core/aircraftmatcher.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/aircraftmodelloader.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "misc/simulation/fscommon/aircraftcfgparser.h"
#include "misc/simulation/fscommon/vpilotrulesreader.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/stringutils.h"

#include <QFlags>
#include <QString>
#include <QStringList>
#include <QTextStream>

using namespace swift::core;
using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;
using namespace swift::misc::simulation::settings;
using namespace swift::misc::aviation;

namespace BlackSample
{
    void CSamplesModelMapping::samples(QTextStream &streamOut, QTextStream &streamIn)
    {
        CVPilotRulesReader vpRulesReader;
        const bool s = vpRulesReader.read(true);
        streamOut << "directory: " << CVPilotRulesReader::standardMappingsDirectory() << Qt::endl;
        streamOut << "loaded: " << swift::misc::boolToYesNo(s) << " size: " << vpRulesReader.getAsModelsFromCache().size() << Qt::endl;

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
