/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMTEST_TESTSIMCOMMON_H
#define BLACKSIMTEST_TESTSIMCOMMON_H

#include <QtTest/QtTest>

namespace BlackSimTest
{
    //! Test for simulator common classes
    class CTestFsCommon : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestFsCommon(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! FsCommon tests
        void misc();
    };

} // namespace

#endif // guard
