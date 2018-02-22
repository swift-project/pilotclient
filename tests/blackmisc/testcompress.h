/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTCOMPRESS_H
#define BLACKMISCTEST_TESTCOMPRESS_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

#include <QObject>

namespace BlackMiscTest
{
    //! Testing process tools
    class CTestCompress : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTestCompress(QObject *parent = nullptr) : QObject(parent) {}

    private slots:
        //! Uncompress file
        void uncompressFile();
    };
}

//! \endcond

#endif
