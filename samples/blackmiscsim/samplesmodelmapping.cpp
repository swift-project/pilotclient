/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesmodelmapping.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/sampleutils.h"
#include "blacksim/blacksimfreefunctions.h"
#include "blacksim/fscommon/vpilotmodelmappings.h"
#include "blacksim/fscommon/aircraftmapper.h"

#include <QDebug>
#include <memory>
#include <utility>

using namespace BlackMisc;
using namespace BlackSim;
using namespace BlackSim::FsCommon;
using namespace BlackMisc::Aviation;

namespace BlackSimTest
{

    /*
     * Samples
     */
    int CSamplesModelMapping::samples(QTextStream &streamOut, QTextStream &streamIn)
    {
        BlackMisc::registerMetadata();
        BlackSim::registerMetadata();

        std::unique_ptr<ISimulatorModelMappings> cvm(new CVPilotModelMappings(true));
        bool s = cvm->read();
        streamOut << "directory: " << CVPilotModelMappings::standardMappingsDirectory() << endl;
        streamOut << "loaded: " << (s ? "yes" : "no") << " size: " << cvm->size() << endl;

        // mapper with rule set, handing over ownership
        CAircraftMapper mapper(std::move(cvm));
        QString fsxDir = CSampleUtils::selectDirectory({"P:/FlightSimulatorX (MSI)/SimObjects", "P:/Temp/SimObjects"}, streamOut, streamIn);
        if (!mapper.changeCAircraftCfgEntriesDirectory(fsxDir))
        {
            streamOut << "Wrong or empty directoy " << fsxDir << endl;
            return 0;
        }

        streamOut << "Start reading models" << endl;
        mapper.changeCAircraftCfgEntriesDirectory(fsxDir);
        mapper.readSimObjects();
        streamOut << "Read models: " << mapper.countAircraftCfgEntries() << endl;
        streamOut << "Ambigious models: " << mapper.getAircraftCfgEntriesList().detectAmbiguousTitles().join(", ") << endl;

        // sync definitions, remove redundant ones
        streamOut << "Now synchronizing defintions: " << mapper.countMappingRules() << endl;
        int afterSync = mapper.synchronize();
        streamOut << "After synchronizing definitions: " << afterSync << endl;

        CAircraftIcao icao("C172");
        streamOut << "Searching for " << icao << endl;
        streamOut << mapper.getAircraftMappingList().findByIcaoCodeExact(icao) << endl;

        return 0;
    }

} // namespace
