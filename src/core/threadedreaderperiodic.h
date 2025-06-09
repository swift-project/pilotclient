// SPDX-FileCopyrightText: Copyright (C) 2025 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_THREADED_READER_PERIODIC_H
#define SWIFT_CORE_THREADED_READER_PERIODIC_H

#include <QObject>
#include <QString>

#include "core/swiftcoreexport.h"
#include "core/threadedreader.h"
#include "misc/threadedtimer.h"

namespace swift::core
{
    //! Periodically executes doWorkImpl() in a separate thread
    class SWIFT_CORE_EXPORT CThreadedReaderPeriodic : public swift::core::CThreadedReader
    {
        Q_OBJECT
    public:
        //! Destructor
        ~CThreadedReaderPeriodic() override = default;

        //! Starts the reader
        //! \threadsafe
        void startReader();

    protected:
        //! Constructor
        CThreadedReaderPeriodic(QObject *owner, const QString &name);

        //! This method does the actual work in the derived class
        virtual void doWorkImpl() = 0;

        //! Set initial and periodic times
        //! Changes only apply after the next time the timer restarts
        //! \threadsafe
        void setInitialAndPeriodicTime(std::chrono::milliseconds initialTime, std::chrono::milliseconds periodicTime);

    private:
        //! Trigger doWorkImpl
        void doWork();

        std::atomic<std::chrono::milliseconds> m_initialTime = std::chrono::milliseconds(0); //!< Initial start delay
        std::atomic<std::chrono::milliseconds> m_periodicTime = std::chrono::milliseconds(0); //!< Periodic time after which the task is repeated

        misc::CThreadedTimer m_updateTimer; //!< Update timer
    };
} // namespace swift::core

#endif // SWIFT_CORE_THREADED_READER_PERIODIC_H
