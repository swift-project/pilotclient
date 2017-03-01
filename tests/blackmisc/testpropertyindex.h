/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTPROPERTYINDEX_H
#define BLACKMISCTEST_TESTPROPERTYINDEX_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include <QObject>

namespace BlackMiscTest
{
    //! Testing property index access
    class CTestPropertyIndex : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestPropertyIndex(QObject *parent = nullptr);

    private slots:
        //! Simulated aircraft index checks
        void propertyIndexCSimulatedAircraft();
    };
} // namespace

//! \endcond

#endif // guard
