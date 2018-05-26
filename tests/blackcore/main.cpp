/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackcore
 */

#include "testblackcoremain.h"
#include "blackcore/application.h"
#include <QCoreApplication>
#include <QDebug>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackCoreTest;

//! Starter for test cases
int main(int argc, char *argv[])
{
    QCoreApplication qa(argc, argv);
    Q_UNUSED(qa);
    CApplication a(CApplicationInfo::UnitTest);
    a.addVatlibOptions();
    const bool setup = a.parseAndSynchronizeSetup();
    if (!setup) { qWarning() << "No setup loaded"; }
    int r = EXIT_FAILURE;
    if (a.start())
    {
        r = CBlackCoreTestMain::unitMain(argc, argv);
    }
    a.gracefulShutdown();
    return r;
}

//! \endcond
