/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testpluginfsxp3d

#include "testblacksimpluginfsxp3dmain.h"
#include "blackmisc/registermetadata.h"
#include <QCoreApplication>
#include <QtGlobal>

using namespace BlackSimPluginFsxP3D;

/*!
 * Starter for test cases
 * \brief main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Q_UNUSED(a);

    BlackMisc::registerMetadata();
    return CBlackSimPluginFsxP3DTest::unitMain(argc, argv);;
}

//! \endcond
