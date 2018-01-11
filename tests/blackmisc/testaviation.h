/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTAVIATIONBASE_H
#define BLACKMISCTEST_TESTAVIATIONBASE_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include <QObject>

namespace BlackMiscTest
{
    /*!
     * Aviation classes basic tests
     */
    class CTestAviation : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestAviation(QObject *parent = nullptr);

    private slots:
        //! Basic unit tests for physical units
        void headingBasics();

        //! COM and NAV units
        void comAndNav();

        //! Transponder
        void transponder();

        //! Callsigns and callsign containers
        void callsignWithContainers();

        //! Testing copying and equality of objects
        void copyAndEqual();
    };
} // namespace

//! \endcond

#endif // guard
