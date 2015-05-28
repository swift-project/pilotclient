/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISCTEST_TESTORIGINATOR_H
#define BLACKMISCTEST_TESTORIGINATOR_H

#include "blackmisc/originatoraware.h"
#include <QtTest/QtTest>

namespace BlackMiscTest
{
    //! Testing orignator
    class CTestOriginator : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTestOriginator(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        void originatorBasics();
    };

    //! Test originator aware
    class CTestOriginatorAware : public BlackMisc::COriginatorAware
    {
    public:
        //! Constructor
        CTestOriginatorAware(QObject *nameObject);
    };

} // namespace

#endif // guard
