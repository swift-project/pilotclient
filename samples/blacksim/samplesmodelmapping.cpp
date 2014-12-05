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
#include "blacksim/blacksimfreefunctions.h"
#include "blacksim/fscommon/vpilotmodelmappings.h"

#include <QDebug>

using namespace BlackSim::FsCommon;

namespace BlackSimTest
{

    /*
     * Samples
     */
    int CSamplesModelMapping::samples(QTextStream &streamOut)
    {
        BlackMisc::registerMetadata();
        BlackSim::registerMetadata();

        CVPilotModelMappings cvm;
        cvm.addDirectory(CVPilotModelMappings::standardMappingsDirectory());
        bool s = cvm.load();
        streamOut << "loaded:" << s << "size:" << cvm.size() << endl;

        BlackMisc::Aviation::CAircraftIcao icao("C172");
        streamOut << cvm.findByIcaoWildcard(icao) << endl;

        return 0;
    }

} // namespace
