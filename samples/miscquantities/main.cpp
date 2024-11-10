// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscquantities

#include "samplesaviation.h"
#include "samplesgeo.h"
#include "samplesphysicalquantities.h"
#include "core/application.h"
#include "misc/directoryutils.h"

#include <stdio.h>
#include <QCoreApplication>
#include <QIODevice>
#include <QTextStream>
#include <QtGlobal>

using namespace swift::misc;
using namespace BlackSample;
using namespace swift::core;

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
