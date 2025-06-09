// SPDX-FileCopyrightText: Copyright (C) 2025 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_THREADED_READER_PERIODIC_H
#define SWIFT_CORE_THREADED_READER_PERIODIC_H

#include <QObject>
#include <QString>

#include "core/swiftcoreexport.h"
#include "core/threadedreader.h"

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
        void setInitialAndPeriodicTime(int initialTime, int periodicTime);

    private:
        //! Trigger doWorkImpl
        void doWork();

        int m_initialTime = -1; //!< Initial start delay
        int m_periodicTime = -1; //!< Periodic time after which the task is repeated
    };
} // namespace swift::core

#endif // SWIFT_CORE_THREADED_READER_PERIODIC_H
