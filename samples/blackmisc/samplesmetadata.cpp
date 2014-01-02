/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesmetadata.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/avatcstationlist.h"
#include <QDebug>
#include <QMetaType>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;

namespace BlackMiscTest
{

    /*
     * Samples
     */
    int CSamplesMetadata::samples()
    {
        BlackMisc::registerMetadata();
        BlackMisc::displayAllUserMetatypesTypes();
        return 0;
    }

} // namespace
