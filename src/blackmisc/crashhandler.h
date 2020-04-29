/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CRASHHANDLER_H
#define BLACKMISC_CRASHHANDLER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/crashinfo.h"
#include "blackmisc/digestsignal.h"

#include <memory>

namespace crashpad
{
    class CrashpadClient;
    class CrashReportDatabase;
}

namespace BlackMisc
{
    //! Crash handler
    class BLACKMISC_EXPORT CCrashHandler : public QObject
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
        void setCrashInfo(const BlackMisc::CCrashInfo &info);

        //! Set user name for crash info
        void crashAndLogInfoUserName(const QString &name);

        //! Set simulator string in crash info
        void crashAndLogInfoSimulator(const QString &simulator);

        //! Set flight network in crash info
        void crashAndLogInfoFlightNetwork(const QString &flightNetwork);

        //! Append crash info
        void crashAndLogAppendInfo(const QString &info);

        //! Get crash info
        const BlackMisc::CCrashInfo &getCrashInfo() const { return m_crashAndLogInfo; }

        //! Simulate a crash
        void simulateCrash();

        //! Simulate an ASSERT
        void simulateAssert();

    private:
        CCrashHandler(QObject *parent = nullptr);

        // crash info
        void triggerCrashInfoWrite();

        BlackMisc::CCrashInfo    m_crashAndLogInfo; //!< info representing details
        BlackMisc::CDigestSignal m_dsCrashAndLogInfo { this, &CCrashHandler::triggerCrashInfoWrite, 10000, 5 };

#ifdef BLACK_USE_CRASHPAD
        std::unique_ptr<crashpad::CrashpadClient> m_crashpadClient;
        std::unique_ptr<crashpad::CrashReportDatabase> m_crashReportDatabase;
#endif
    };
}

#endif
