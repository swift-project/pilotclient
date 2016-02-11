/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTINPUT_H
#define BLACKMISCTEST_TESTINPUT_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include <QtTest/QtTest>

namespace BlackMiscTest
{
    //! Input classes basic tests
    class CTestInput : public QObject
    {
        Q_OBJECT
    public:
        //! Standard test case constructor
        explicit CTestInput(QObject *parent = nullptr);

    signals:

    private slots:

        //! CKeyboardKey basic tests
        void keyboardKey();

        //! CHotkeyCombination tests
        void hotkeyCombination();

        void hotKeyEqual();
        void hotKeyNonEqual();
        void hotkeyEqualFromVariant();
        void hotkeyFindSupersetOf();
        void hotkeyFindSubsetOf();
    };
} // namespace

//! \endcond

#endif // guard
