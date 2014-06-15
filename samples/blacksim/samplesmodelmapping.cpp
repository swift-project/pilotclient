/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesmodelmapping.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blacksim/blacksimfreefunctions.h"
#include "blacksim/vpilotmodelmappings.h"

#include <QDebug>

using namespace BlackSim;

namespace BlackSimTest
{

    /*
     * Samples
     */
    int CSamplesModelMapping::samples()
    {
        BlackMisc::registerMetadata();
        BlackSim::registerMetadata();

        CVPilotModelMappings cvm;
        cvm.addDirectory(CVPilotModelMappings::standardMappingsDirectory());
        bool s = cvm.load();
        qDebug() << "loaded:" << s << "size:" << cvm.size();

        BlackMisc::Aviation::CAircraftIcao icao("C172");
        qDebug() << cvm.findByIcao(icao);

        return 0;
    }

} // namespace
