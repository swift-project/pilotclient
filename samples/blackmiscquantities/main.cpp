/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscquantities

#include "samplesphysicalquantities.h"
#include "samplesaviation.h"
#include "samplesgeo.h"
#include "blackcore/application.h"
#include <QTextStream>

using namespace BlackMisc;
using namespace BlackSample;
using namespace BlackCore;

//! main
int main(int argc, char *argv[])
{
    QCoreApplication(argc, argv);
    CApplication a;
    Q_UNUSED(a);

    QTextStream out(stdout, QIODevice::WriteOnly);
    CSamplesPhysicalQuantities::samples(out);
    CSamplesAviation::samples(out);
    CSamplesGeo::samples(out);
    return 0;
}
