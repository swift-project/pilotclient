/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 */

#ifndef BLACKCORETEST_TESTUTILS_H
#define BLACKCORETEST_TESTUTILS_H

#include "blackgui/guiutility.h"
#include <QtTest/QtTest>

namespace BlackCoreTest
{
    /**
     * Test GUI utilities
     */
    class CTestGuiUtilities : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        explicit CTestGuiUtilities(QObject *parent = nullptr);

    private slots:
        //! Test the utility functions
        void utilityFunctions();
    };

} //namespace

//! \endcond

#endif // guard
