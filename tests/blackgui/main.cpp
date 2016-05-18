/* Copyright (C) 2015
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
 * \ingroup testblackgui
 */

#include "testblackguimain.h"

#include <QCoreApplication>
#include <QtGlobal>

using namespace BlackGuiTest;

//! Starter for test cases
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Q_UNUSED(a);

    return CBlackGuiTestMain::unitMain(argc, argv);
}

//! \endcond
