/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISCTEST_TESTHARDWARE_H
#define BLACKMISCTEST_TESTHARDWARE_H

#include <QtTest/QtTest>

namespace BlackMiscTest
{
    //! Hardware classes basic tests
    class CTestHardware : public QObject
    {
        Q_OBJECT
    public:
        //! Standard test case constructor
        explicit CTestHardware(QObject *parent = nullptr);

    signals:

    private slots:

        //! CKeyboardKey basic tests
        void keyboardKey();
    };
} // namespace

#endif // guard
