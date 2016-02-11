/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTIDENTIFIER_H
#define BLACKMISCTEST_TESTIDENTIFIER_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "blackmisc/identifiable.h"
#include <QtTest/QtTest>

namespace BlackMiscTest
{
    //! Testing object identifier
    class CTestIdentifier : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTestIdentifier(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Identifier tests
        void identifierBasics();
    };

    //! Test identifiable object
    class CTestIdentifiable : public BlackMisc::CIdentifiable
    {
    public:
        //! Constructor
        CTestIdentifiable(QObject *nameObject);
    };

} // namespace

//! \endcond

#endif // guard
