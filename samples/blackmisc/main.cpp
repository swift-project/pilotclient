/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
    // QCoreApplication a(argc, argv);
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    BlackMisc::initResources();
    CSamplesJson::samples();
    CSamplesChangeObject::samples();
    CSamplesContainer::samples();
    CSamplesMetadata::samples();
    return 0;
}
