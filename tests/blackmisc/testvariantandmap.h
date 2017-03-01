/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTVARIANTANDMAP_H
#define BLACKMISCTEST_TESTVARIANTANDMAP_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include <QObject>

namespace BlackMiscTest
{
    //! Variant and map related tests
    class CTestVariantAndMap : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestVariantAndMap(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Basic unit tests for value objects and variants
        void variant();

        //! Unit tests for value maps and value objects
        void valueMap();
    };
} // namespace

//! \endcond

#endif // guard
