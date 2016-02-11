/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORETEST_TESTREADERS_H
#define BLACKCORETEST_TESTREADERS_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackcore
 */

#include "blackcore/networkvatlib.h"
#include "blackcore/modeldatareader.h"
#include "blackcore/icaodatareader.h"
#include <QtTest/QtTest>

namespace BlackCoreTest
{
    /*!
     * Test data readers (for bookings, JSON, etc.)
     */
    class CTestReaders : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        explicit CTestReaders(QObject *parent = nullptr);

    private slots:
        //! Read ICAO data
        void readIcaoData();

        //! Read model data
        void readModelData();

    private:
        BlackCore::CIcaoDataReader  m_icaoReader;
        BlackCore::CModelDataReader m_modelReader;
        BlackMisc::CData<BlackCore::Data::GlobalSetup> m_setup {this}; //!< setup cache

        //! Test if server is available
        static bool pingServer(const BlackMisc::Network::CUrl &url);
    };

} //namespace

//! \endcond

#endif // guard
