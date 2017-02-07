/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORETEST_TESTDBUS_H
#define BLACKCORETEST_TESTDBUS_H

#include "blackmisc/variant.h"
#include <QObject>

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackcore

namespace BlackCoreTest
{
    /*!
     * DBus implementation classes tests
     */
    class CTestDBus : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        explicit CTestDBus(QObject *parent = nullptr) :
            QObject(parent)
        {}

    private slots:
        //! Test marshalling/unmarshalling
        void marshallUnmarshall();

        //! Signature size
        void signatureSize();
    };
} //namespace

//! \endcond

#endif // guard
