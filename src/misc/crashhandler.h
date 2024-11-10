// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_CRASHHANDLER_H
#define SWIFT_MISC_CRASHHANDLER_H

#include "misc/swiftmiscexport.h"
#include "misc/crashinfo.h"
#include "misc/digestsignal.h"

#include <memory>

namespace crashpad
{
    class CrashpadClient;
    class CrashReportDatabase;
}

namespace swift::misc
{
    //! Crash handler
    class SWIFT_MISC_EXPORT CCrashHandler : public QObject
    {
        Q_OBJECT

    public:
        //! Get singleton instance
        static CCrashHandler *instance();

        //! Destructor
        virtual ~CCrashHandler();

        //! Initialize and start crashpad handler process
        void init();

        //! Enable/disable automatic uploading
        void setUploadsEnabled(bool enable);

        //! Is automatic dump uploading enabled?
        bool isCrashDumpUploadEnabled() const;

        // ----------------------- Crash info ---------------------------------

        //! Extra annotation for crash to easier identify annotation
        void setCrashInfo(const swift::misc::CCrashInfo &info);

        //! Set user name for crash info
        void crashAndLogInfoUserName(const QString &name);

        //! Set simulator string in crash info
        void crashAndLogInfoSimulator(const QString &simulator);

        //! Set flight network in crash info
        void crashAndLogInfoFlightNetwork(const QString &flightNetwork);

        //! Append crash info
        void crashAndLogAppendInfo(const QString &info);

        //! Get crash info
        const swift::misc::CCrashInfo &getCrashInfo() const { return m_crashAndLogInfo; }

        //! Simulate a crash
        void simulateCrash();

        //! Simulate an ASSERT
        void simulateAssert();

    private:
        CCrashHandler(QObject *parent = nullptr);

        // crash info
        void triggerCrashInfoWrite();

        swift::misc::CCrashInfo m_crashAndLogInfo; //!< info representing details
        swift::misc::CDigestSignal m_dsCrashAndLogInfo { this, &CCrashHandler::triggerCrashInfoWrite, 10000, 5 };

#ifdef SWIFT_USE_CRASHPAD
        std::unique_ptr<crashpad::CrashpadClient> m_crashpadClient;
        std::unique_ptr<crashpad::CrashReportDatabase> m_crashReportDatabase;
#endif
    };
}

#endif
