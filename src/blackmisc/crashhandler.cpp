/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/crashhandler.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/filelogger.h"
#include "blackconfig/buildconfig.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QStringBuilder>

#ifdef BLACK_USE_CRASHPAD
#    if defined(Q_OS_WIN) && !defined(NOMINMAX)
#        define NOMINMAX
#    endif
#    include "crashpad/client/crashpad_client.h"
#    include "crashpad/client/crash_report_database.h"
#    include "crashpad/client/settings.h"
#    include "crashpad/client/simulate_crash.h"
#endif

using namespace BlackConfig;
using namespace BlackMisc;
using namespace crashpad;

namespace BlackMisc
{
    CCrashHandler *CCrashHandler::instance()
    {
        static CCrashHandler crashHandler;
        return &crashHandler;
    }

    CCrashHandler::~CCrashHandler()
    {}

#ifdef BLACK_USE_CRASHPAD
    //! Convert to file path
    base::FilePath qstringToFilePath(const QString &str)
    {
#    ifdef Q_OS_WIN
        return base::FilePath(str.toStdWString());
#    else
        return base::FilePath(str.toStdString());
#    endif
    }
#endif

    void CCrashHandler::init()
    {
#ifdef BLACK_USE_CRASHPAD
        static const QString crashpadHandler(CBuildConfig::isRunningOnWindowsNtPlatform() ? "swift_crashpad_handler.exe" : "swift_crashpad_handler");
        static const QString handler = CFileUtils::appendFilePaths(CSwiftDirectories::binDirectory(), crashpadHandler);
        const QString database = CSwiftDirectories::crashpadDatabaseDirectory();
        const QString metrics = CSwiftDirectories::crashpadMetricsDirectory();

        if (!QFileInfo::exists(handler)) { return; }

        const std::string serverUrl("http://swift-project.sp.backtrace.io:6097/");
        std::map<std::string, std::string> annotations;

        // Backtrace annotations
        annotations["token"] = CBuildConfig::backtraceToken().toStdString();
        annotations["format"] = "minidump";
        annotations["commit"] = CBuildConfig::gitHeadSha1().toStdString();
        annotations["version"] = CBuildConfig::getVersionString().toStdString();
        annotations["short_version"] = CBuildConfig::getShortVersionString().toStdString();
        annotations["platform"] = CBuildConfig::getPlatformString().toStdString();
        annotations["qtversion"] = QT_VERSION_STR;

        // add our logfile
        const QString logAttachment = QStringLiteral("--attachment=attachment_%1=%2").arg(CFileLogger::getLogFileName(), CFileLogger::getLogFilePath());

        std::vector<std::string> arguments;
        arguments.push_back(logAttachment.toStdString());

        // and the simplified crash info if any
        const QString crashInfoFileName("swiftcrashinfo.txt");
        const QString crashInfoFilePath(CFileUtils::appendFilePaths(CFileUtils::stripFileFromPath(CFileLogger::getLogFilePath()), crashInfoFileName));
        m_crashAndLogInfo.setLogPathAndFileName(crashInfoFilePath);
        const QString crashAttachment = QStringLiteral("--attachment=attachment_%1=%2").arg(crashInfoFileName, crashInfoFilePath);
        arguments.push_back(crashAttachment.toStdString());

        // for testing purposes
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            arguments.push_back("--no-rate-limit");
        }

        QDir().mkpath(database);

        m_crashReportDatabase = CrashReportDatabase::Initialize(qstringToFilePath(database));
        m_crashpadClient = std::make_unique<CrashpadClient>();
        m_crashpadClient->StartHandler(qstringToFilePath(handler),
                                       qstringToFilePath(database),
                                       qstringToFilePath(metrics),
                                       serverUrl,
                                       annotations,
                                       arguments,
                                       false, true);

        this->crashAndLogAppendInfo(u"Init crash info at " % QDateTime::currentDateTimeUtc().toString());
#endif
    }

    void CCrashHandler::setUploadsEnabled(bool enable)
    {
#ifdef BLACK_USE_CRASHPAD
        if (!m_crashReportDatabase)
        {
            return;
        }
        crashpad::Settings *settings = m_crashReportDatabase->GetSettings();
        settings->SetUploadsEnabled(enable);
#else
        Q_UNUSED(enable);
#endif
    }

    bool CCrashHandler::isCrashDumpUploadEnabled() const
    {
#ifdef BLACK_USE_CRASHPAD
        if (!m_crashReportDatabase)
        {
            return false;
        }
        crashpad::Settings *settings = m_crashReportDatabase->GetSettings();
        bool enabled = false;
        bool ok = settings->GetUploadsEnabled(&enabled);
        return ok && enabled;
#else
        return false;
#endif
    }

    void CCrashHandler::triggerCrashInfoWrite()
    {
        m_crashAndLogInfo.triggerWritingFile();
    }

    void CCrashHandler::setCrashInfo(const CCrashInfo &info)
    {
        m_crashAndLogInfo = info;
        m_dsCrashAndLogInfo.inputSignal();
    }

    void CCrashHandler::crashAndLogInfoUserName(const QString &name)
    {
        m_crashAndLogInfo.setUserName(name);
        m_dsCrashAndLogInfo.inputSignal();
    }

    void CCrashHandler::crashAndLogInfoSimulator(const QString &simulator)
    {
        m_crashAndLogInfo.setSimulatorString(simulator);
        m_dsCrashAndLogInfo.inputSignal();
    }

    void CCrashHandler::crashAndLogInfoFlightNetwork(const QString &flightNetwork)
    {
        m_crashAndLogInfo.setFlightNetworkString(flightNetwork);
        m_dsCrashAndLogInfo.inputSignal();
    }

    void CCrashHandler::crashAndLogAppendInfo(const QString &info)
    {
        m_crashAndLogInfo.appendInfo(info);
        m_dsCrashAndLogInfo.inputSignal();
    }

    void CCrashHandler::simulateCrash()
    {
#ifdef BLACK_USE_CRASHPAD
        CLogMessage(this).info(u"Simulated crash dump!");
        m_crashAndLogInfo.appendInfo("Simulated crash dump!");
        m_crashAndLogInfo.writeToFile();
        CRASHPAD_SIMULATE_CRASH();
        // real crash
        // raise(SIGSEGV); #include <signal.h>
#else
        CLogMessage(this).warning(u"This compiler or platform does not support crashpad. Cannot simulate crash dump!");
#endif
    }

    void CCrashHandler::simulateAssert()
    {
#ifdef BLACK_USE_CRASHPAD
        CLogMessage(this).info(u"Simulated ASSERT!");
        m_crashAndLogInfo.appendInfo("Simulated ASSERT!");
        m_crashAndLogInfo.writeToFile();
        Q_ASSERT_X(false, Q_FUNC_INFO, "Test server to test Crash handler");
#else
        CLogMessage(this).warning(u"This compiler or platform does not support crashpad. Cannot simulate crash dump!");
#endif
    }

    CCrashHandler::CCrashHandler(QObject *parent) : QObject(parent)
    {}
}
