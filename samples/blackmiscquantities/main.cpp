/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscquantities

#include "samplesaviation.h"
#include "samplesgeo.h"
#include "samplesphysicalquantities.h"
#include "blackcore/application.h"
#include "blackmisc/directoryutils.h"

#include <stdio.h>
#include <QCoreApplication>
#include <QIODevice>
#include <QTextStream>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackSample;
using namespace BlackCore;

//! main
int main(int argc, char *argv[])
{
    QCoreApplication qa(argc, argv);
    CApplication a;
    Q_UNUSED(a);
    Q_UNUSED(qa);

    QTextStream out(stdout, QIODevice::WriteOnly);
    CSamplesPhysicalQuantities::samples(out);
    CSamplesAviation::samples(out);
    CSamplesGeo::samples(out);
    return 0;
}
