/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blacksim/blacksimfreefunctions.h"
#include "samplesfscommon.h"
#include "samplesfsx.h"
#include <QCoreApplication>

/*!
 * Samples
 */
int main(int argc, char *argv[])
{
    BlackSim::registerMetadata();
    // BlackSimTest::CSamplesFsCommon::samples();
    BlackSimTest::CSamplesFsx::samples();
    QCoreApplication a(argc, argv);
    return a.exec();
}
