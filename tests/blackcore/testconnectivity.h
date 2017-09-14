/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORETEST_TESTCONNECTIVITY_H
#define BLACKCORETEST_TESTCONNECTIVITY_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackcore

#include "blackcore/db/networkwatchdog.h"
#include "blackcore/setupreader.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/statusmessagelist.h"
#include <QObject>

namespace BlackCoreTest
{
    /*!
     * Test connectivity such as \c canConnect \c ping and
     * BlackCore::Db::CNetworkWatchdog
     */
    class CTestConnectivity : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        explicit CTestConnectivity(QObject *parent = nullptr);

        //! Destructor
        virtual ~CTestConnectivity();

    private slots:
        //! Init
        void initTestCase();

        //! Setup reader
        void checkSetupReader();

        //! Connecting test server
        void connectServer();

        //! ping test server
        void pingServer();

        //! Test the watchdog BlackCore::Db::CNetworkWatchdog
        void testNetworkWatchdog();

    private:
        int m_networkCheckCount = -1;
    };
} //namespace

//! \endcond

#endif // guard
