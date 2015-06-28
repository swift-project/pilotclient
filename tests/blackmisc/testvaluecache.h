/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISCTEST_TESTVALUECACHE_H
#define BLACKMISCTEST_TESTVALUECACHE_H

#include "blackmisc/valuecache.h"
#include <QtTest/QtTest>
#include <future>

namespace BlackMiscTest
{

    /*!
     * Unit tests for value cache system.
     */
    class CTestValueCache : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        explicit CTestValueCache(QObject *parent = nullptr);

    private slots:
        //! Test insert and getAll functions of CValueCache.
        void insertAndGet();

        //! Test using CCached locally in one process.
        void localOnly();

        //! Test using CCached locally in one process, with multiple threads.
        void localOnlyWithThreads();

        //! Test using CCached distributed among two processes.
        void distributed();

        //! Test using batched changes.
        void batched();

        //! Test Json serialization.
        void json();
    };

    /*!
     * Simple class which uses CCached, for testing.
     */
    class CValueCacheUser : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor.
        explicit CValueCacheUser(BlackMisc::CValueCache *cache);

        //! Slot to be called when a cached value changes.
        void ps_valueChanged();

        //! Detect whether the slot was called, for verification.
        bool slotFired();

        std::promise<void> m_slotFired;     //!< Flag marking whether the slot was called.
        BlackMisc::CCached<int> m_value1;   //!< First cached value.
        BlackMisc::CCached<int> m_value2;   //!< Second cached value.
    };

}

#endif
