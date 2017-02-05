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
//! \file
//! \ingroup testblackcore

#include "blackmisc/network/url.h"
#include <stdbool.h>
#include <QObject>

namespace BlackCore
{
    namespace Db
    {
        class CAirportDataReader;
        class CIcaoDataReader;
        class CModelDataReader;
    }
}

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

        //! Destructor
        virtual ~CTestReaders();

    private slots:
        //! Read ICAO data
        void readIcaoData();

        //! Read model data
        void readModelData();

        //! Read airport data
        void readAirportData();

    private:
        BlackCore::Db::CAirportDataReader *m_airportReader = nullptr;
        BlackCore::Db::CIcaoDataReader    *m_icaoReader = nullptr;
        BlackCore::Db::CModelDataReader   *m_modelReader = nullptr;

        //! Test if server is available
        static bool pingServer(const BlackMisc::Network::CUrl &url);
    };
} //namespace

//! \endcond

#endif // guard
