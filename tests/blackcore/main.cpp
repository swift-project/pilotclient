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
#include <QtGlobal>

using namespace BlackCoreTest;

//! Starter for test cases
int main(int argc, char *argv[])
{
    QCoreApplication qa(argc, argv);
    Q_UNUSED(qa);
    BlackCore::CApplication a;
    a.addVatlibOptions();
    a.start();
    return CBlackCoreTestMain::unitMain(argc, argv);
}

//! \endcond
