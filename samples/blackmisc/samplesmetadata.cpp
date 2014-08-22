/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
        QTextStream cin(stdin);

        BlackMisc::registerMetadata();
        BlackMisc::displayAllUserMetatypesTypes();

        cin.readLine();
        qDebug() << "------- Enter --------";

        return 0;
    }

} // namespace
