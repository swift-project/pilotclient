/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORETEST_TESTCONTEXT_H
#define BLACKCORETEST_TESTCONTEXT_H

#include <QObject>

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackcore

namespace BlackCoreTest
{
    /*!
     * Context implementation classes tests
     */
    class CTestContext : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        explicit CTestContext(QObject *parent = nullptr) :
            QObject(parent)
        {}

    private slots:
        //! Init tests for context
        void contextInitTest();

        //! Test the DBus signatures
        void dBusSignatures();
    };
} //namespace

//! \endcond

#endif // guard
