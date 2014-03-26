/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QCoreApplication>
#include "sampleschangeobject.h"
#include "samplesmetadata.h"
#include "samplescontainer.h"
#include "samplesjson.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/pqallquantities.h"

using namespace BlackMisc;
using namespace BlackMiscTest;

/*!
 * Sample tests
 */
int main(int argc, char *argv[])
{
    BlackMisc::initResources();
    QCoreApplication a(argc, argv);
    CSamplesJson::samples();
//    CSamplesChangeObject::samples();
//    CSamplesContainer::samples();
//    CSamplesMetadata::samples();
    return a.exec();
}
