/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTSTRINGUTILS_H
#define BLACKMISCTEST_TESTSTRINGUTILS_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include <QObject>

namespace BlackMiscTest
{
    //! Testing string utilities
    class CTestStringUtils : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTestStringUtils(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        void testRemove();
        void testContains();
        void testIndexOf();
        void testSplit();
        void testTimestampParsing();
    };
}

//! \endcond

#endif
