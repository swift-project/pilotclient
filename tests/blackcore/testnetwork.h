/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORETEST_TESTNETWORK_H
#define BLACKCORETEST_TESTNETWORK_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackcore

#include "blackcore/networkvatlib.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/simulation/ownaircraftproviderdummy.h"
#include <QtTest/QtTest>

namespace BlackCoreTest
{
    /*!
     * INetwork implementation classes tests
     */
    class CTestNetwork : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        explicit CTestNetwork(QObject *parent = nullptr) :
            QObject(parent),
            m_networkVatlib(BlackMisc::Simulation::COwnAircraftProviderDummy::instance(), parent)
        {}

    private slots:
        //! Test the vatlib
        void networkVatlibTest() { networkTest(&m_networkVatlib); }

    private:
        //! Common part used by all tests.
        void networkTest(BlackCore::INetwork *);

        BlackCore::CNetworkVatlib m_networkVatlib ; //!< vatlib instance

        //! Test if server is available
        static bool pingServer(const BlackMisc::Network::CServer &server);
    };

} //namespace

//! \endcond

#endif // guard
