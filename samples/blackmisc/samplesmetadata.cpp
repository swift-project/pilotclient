/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesmetadata.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/metadatautils.h"
#include "blackmisc/aviation/atcstationlist.h"
#include <QDebug>
#include <QMetaType>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;

namespace BlackSample
{
    int CSamplesMetadata::samples()
    {
        QTextStream cin(stdin);
        QTextStream cout(stdout);
        BlackMisc::registerMetadata();
        BlackMisc::displayAllUserMetatypesTypes(cout);

        cin.readLine();
        cout << "------- Enter --------" << endl;
        return 0;
    }

} // namespace
