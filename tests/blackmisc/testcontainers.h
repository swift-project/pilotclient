/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTCONTAINERS_H
#define BLACKMISCTEST_TESTCONTAINERS_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include <QObject>

namespace BlackMiscTest
{
    //! Testing containers
    class CTestContainers : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTestContainers(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        void collectionBasics();
        void sequenceBasics();
        void joinAndSplit();
        void findTests();
        void dictionaryBasics();
        void timestampList();
        void offsetTimestampList();
    };
} // namespace

//! \endcond

#endif // guard
