/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISCTEST_SAMPLESCONCURRENT_H
#define BLACKMISCTEST_SAMPLESCONCURRENT_H

#include "blackmisc/worker.h"
#include <QTextStream>
#include <atomic>

namespace BlackMiscTest
{

    //! Samples for metadata
    class CSamplesConcurrent
    {
    public:
        //! Run the samples
        static int samples(const QString &type, QTextStream &out, QTextStream &in);
    };

    //! Doing some work
    class CThreadOutput : public BlackMisc::CContinuousWorker
    {
        Q_OBJECT

    public:
        //! Constructor
        CThreadOutput(const QString &name, QObject *parent = nullptr);

        //! Working task
        void doWork();

        //! Stop
        void stop();

    protected slots:
        //! \copydoc CContinuousWorker::initialize
        virtual void initialize() override;

    private:
        std::atomic<bool> m_run { true };
    };

    //! Doing some work
    class CConcurrentOutput : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CConcurrentOutput(const QString &name, QObject *parent = nullptr);

        //! Stop
        void stop();

    public slots:
        //! Working task
        void doWork();

    private:
        std::atomic<bool> m_run { true };
        QString m_name;
    };



} // namespace

#endif
