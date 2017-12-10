/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTICON_H
#define BLACKMISCTEST_TESTICON_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include <QObject>

namespace BlackMiscTest
{
    //! Testing icons
    class CTestIcon : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTestIcon(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Icon order/index
        void iconIndex();
    };
} // namespace

//! \endcond

#endif // guard
