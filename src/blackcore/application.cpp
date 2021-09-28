/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackconfig/buildconfig.h"
#include "blackcore/db/networkwatchdog.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulatorimpl.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/cookiemanager.h"
#include "blackcore/corefacade.h"
#include "blackcore/registermetadata.h"
#include "blackcore/setupreader.h"
#include "blackcore/webdataservices.h"
#include "blackcore/inputmanager.h"
#include "blackmisc/atomicfile.h"
#include "blackmisc/applicationinfo.h"
#include "blackmisc/crashhandler.h"
#include "blackmisc/datacache.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/eventloop.h"
#include "blackmisc/filelogger.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/logpattern.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/slot.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/verify.h"

#include <stdbool.h>
#include <stdio.h>
#include <QCoreApplication>
#include <QDateTime>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkConfigurationManager>
#include <QProcess>
#include <QRegularExpression>
#include <QSslSocket>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QStringList>
#include <QTemporaryDir>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QTranslator>
#include <QWriteLocker>
#include <Qt>
#include <QtGlobal>
#include <QSysInfo>
#include <cstdlib>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;
using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackCore::Vatsim;
using namespace BlackCore::Data;
using namespace BlackCore::Db;

BlackCore::CApplication *sApp = nullptr; // set by constructor

//! \private
static const QString &swiftDataRoot()
{
    static const QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/org.swift-project/";
    return path;
}

namespace BlackCore
{
    CApplication::CApplication(CApplicationInfo::Application application, bool init) :
        CApplication(executable(), application, init)
    { }

    CApplication::CApplication(const QString &applicationName, CApplicationInfo::Application application, bool init) :
        CIdentifiable(this),
        m_networkConfigManager(new QNetworkConfigurationManager(this)),
        m_accessManager(new QNetworkAccessManager(this)),
        m_applicationInfo(application),
        m_applicationName(applicationName), m_coreFacadeConfig(CCoreFacadeConfig::allEmpty())
    {
        Q_ASSERT_X(!sApp, Q_FUNC_INFO, "already initialized");
        Q_ASSERT_X(QCoreApplication::instance(), Q_FUNC_INFO, "no application object");

        m_applicationInfo.setApplicationDataDirectory(CSwiftDirectories::normalizedApplicationDataDirectory());
        QCoreApplication::setApplicationName(m_applicationName);
        QCoreApplication::setApplicationVersion(CBuildConfig::getVersionString());
        this->setObjectName(m_applicationName);
        this->thread()->setObjectName(m_applicationName); // normally no effect as thread already runs, but does not harm either

        // init skipped when called from CGuiApplication
        if (init)
        {
            this->init(true);
        }
    }

    void CApplication::init(bool withMetadata)
    {
        if (!sApp)
        {
            // notify when app goes down
            connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &CApplication::gracefulShutdown);

            // metadata
            if (withMetadata) { CApplication::registerMetadata(); }

            // unit test
            if (this->getApplicationInfo().getApplication() == CApplicationInfo::UnitTest)
            {
                const QString tempPath(this->getTemporaryDirectory());
                BlackMisc::setMockCacheRootDirectory(tempPath);
            }
            m_alreadyRunning = CApplication::getRunningApplications().containsApplication(CApplication::getApplicationInfo().getApplication());
            this->initParser();
            this->initLogging();
            this->tagApplicationDataDirectory();

            //
            // cmd line arguments not yet parsed here
            //

            // Translations
            QTranslator translator;
            if (translator.load("blackmisc_i18n_de", ":blackmisc/translations/")) { CLogMessage(this).debug() << "Translator loaded"; }
            QCoreApplication::instance()->installTranslator(&translator);

            // main app
            sApp = this;

            this->initNetwork();

            // global setup
            m_setupReader.reset(new CSetupReader(this));
            connect(m_setupReader.data(), &CSetupReader::setupHandlingCompleted, this, &CApplication::onSetupHandlingCompleted, Qt::QueuedConnection);
            connect(m_setupReader.data(), &CSetupReader::setupHandlingCompleted, this, &CApplication::setupHandlingCompleted, Qt::QueuedConnection); // hand thru
            connect(m_setupReader.data(), &CSetupReader::successfullyReadSharedUrl, m_networkWatchDog, &CNetworkWatchdog::setWorkingSharedUrl, Qt::QueuedConnection);

            this->addParserOptions(m_setupReader->getCmdLineOptions()); // add options from reader

            // check for updates
            m_gitHubPackagesReader.reset(new CGitHubPackagesReader(this));
            connect(m_gitHubPackagesReader.data(), &CGitHubPackagesReader::updateInfoAvailable, this, &CApplication::updateInfoAvailable, Qt::QueuedConnection);
            reloadUpdateInfo();

            // startup done
            connect(this, &CApplication::startUpCompleted, this, &CApplication::onStartUpCompleted, Qt::QueuedConnection);
            connect(this, &CApplication::coreFacadeStarted, this, &CApplication::onCoreFacadeStarted, Qt::QueuedConnection);

            if (!this->getApplicationInfo().isUnitTest())
            {
                m_inputManager = new CInputManager(this);
                m_inputManager->createDevices();
            }

            connect(this, &QObject::destroyed, [cat = CLogCategoryList(this)]
            {
                for (CWorkerBase *worker : CWorkerBase::allWorkers())
                {
                    CLogMessage(cat).debug(u"Worker named '%1' still exists after application destroyed") << worker->objectName();
                }
            });
        }
    }

    bool CApplication::registerAsRunning()
    {
        //! \fixme KB 2017-11 maybe this code can be encapsulated somewhere
        CApplicationInfoList apps = CApplication::getRunningApplications();
        const CApplicationInfo myself = CApplication::instance()->getApplicationInfo();
        if (!apps.contains(myself)) { apps.push_back(myself); }
        const bool ok = CFileUtils::writeStringToLockedFile(apps.toJsonString(), CFileUtils::appendFilePaths(swiftDataRoot(), "apps.json"));
        if (!ok) { CLogMessage(static_cast<CApplication *>(nullptr)).error(u"Failed to write to application list file"); }
        return ok;
    }

    bool CApplication::unregisterAsRunning()
    {
        //! \fixme KB 2017-11 maybe this code can be encapsulated somewhere
        CApplicationInfoList apps = CApplication::getRunningApplications();
        const CApplicationInfo myself = CApplication::instance()->getApplicationInfo();
        if (!apps.contains(myself)) { return true; }
        apps.remove(myself);
        const bool ok = CFileUtils::writeStringToLockedFile(apps.toJsonString(), CFileUtils::appendFilePaths(swiftDataRoot(), "apps.json"));
        if (!ok) { CLogMessage(static_cast<CApplication *>(nullptr)).error(u"Failed to write to application list file"); }
        return ok;
    }

    int CApplication::exec()
    {
        Q_ASSERT_X(instance(), Q_FUNC_INFO, "missing application");
        CApplication::registerAsRunning();
        return QCoreApplication::exec();
    }

    void CApplication::restartApplication(const QStringList &newArguments, const QStringList &removeArguments)
    {
        CApplication::unregisterAsRunning();
        const QString prg = QCoreApplication::applicationFilePath();
        const QStringList args = this->argumentsJoined(newArguments, removeArguments);
        this->gracefulShutdown();
        QProcess::startDetached(prg, args);
        this->exit(0);
    }

    CApplication::~CApplication()
    {
        this->gracefulShutdown();
    }

    CApplicationInfoList CApplication::getRunningApplications()
    {
        CApplicationInfoList apps;
        apps.convertFromJsonNoThrow(CFileUtils::readLockedFileToString(swiftDataRoot() + "apps.json"), {}, {});
        apps.removeIf([](const CApplicationInfo & info) { return !info.getProcessInfo().exists(); });
        return apps;
    }

    bool CApplication::isApplicationRunning(CApplicationInfo::Application application)
    {
        const CApplicationInfoList running = CApplication::getRunningApplications();
        return running.containsApplication(application);
    }

    bool CApplication::isAlreadyRunning() const
    {
        return getRunningApplications().containsBy([this](const CApplicationInfo & info) { return info.getApplication() == getApplicationInfo().getApplication(); });
    }

    bool CApplication::isShuttingDown() const
    {
        return m_shutdown || m_shutdownInProgress;
    }

    bool CApplication::isIncognito() const
    {
        return m_incognito;
    }

    void CApplication::setIncognito(bool incognito)
    {
        m_incognito = incognito;
    }

    void CApplication::toggleIncognito()
    {
        m_incognito = !m_incognito;
    }

    const QString &CApplication::getApplicationNameAndVersion() const
    {
        static const QString s(m_applicationName % u" " % CBuildConfig::getVersionString());
        return s;
    }

    const QString &CApplication::getApplicationNameVersionDetailed() const
    {
        static const QString s(m_applicationName % u" " % this->versionStringDetailed());
        return s;
    }

    void CApplication::setSingleApplication(bool singleApplication)
    {
        m_singleApplication = singleApplication;
    }

    QString CApplication::getExecutableForApplication(CApplicationInfo::Application application) const
    {
        QString searchFor;
        switch (application)
        {
        case CApplicationInfo::PilotClientCore: searchFor = "core"; break;
        case CApplicationInfo::Laucher:         searchFor = "launcher"; break;
        case CApplicationInfo::MappingTool:     searchFor = "data"; break;
        case CApplicationInfo::PilotClientGui:  searchFor = "gui"; break;
        default: break;
        }
        if (searchFor.isEmpty()) { return {}; }

        for (const QString &executable : CFileUtils::getSwiftExecutables())
        {
            if (!executable.contains("swift", Qt::CaseInsensitive)) { continue; }
            if (executable.contains(searchFor, Qt::CaseInsensitive)) { return executable; }
        }
        return {};
    }

    bool CApplication::startLauncher()
    {
        static const QString launcher = CApplication::getExecutableForApplication(CApplicationInfo::Application::Laucher);
        if (launcher.isEmpty() || CApplication::isApplicationRunning(CApplicationInfo::Laucher)) { return false; }

        // const QStringList args = this->argumentsJoined({}, { "--dbus", "--core", "--coreaudio" });
        const QStringList args = this->argumentsJoined({}, { "--dbus", "--core" });
        return QProcess::startDetached(launcher, args);
    }

    bool CApplication::startLauncherAndQuit()
    {
        const bool started = CApplication::startLauncher();
        if (!started) { return false; }
        this->gracefulShutdown();
        CApplication::exit();
        return true;
    }

    CGlobalSetup CApplication::getGlobalSetup() const
    {
        if (m_shutdown) { return CGlobalSetup(); }
        const CSetupReader *r = m_setupReader.data();
        if (!r) { return CGlobalSetup(); }
        return r->getSetup();
    }

    CUpdateInfo CApplication::getUpdateInfo() const
    {
        if (m_shutdown) { return CUpdateInfo(); }
        if (!m_gitHubPackagesReader) { return CUpdateInfo(); }
        return m_gitHubPackagesReader->getUpdateInfo();
    }

    void CApplication::reloadUpdateInfo()
    {
        if (m_shutdown) { return; }
        if (!m_gitHubPackagesReader) { return; }
        m_gitHubPackagesReader->readUpdateInfo();
    }

    CDistribution CApplication::getOwnDistribution() const
    {
        if (CBuildConfig::isLocalDeveloperDebugBuild()) { return CDistribution::localDeveloperBuild(); }
        const CUpdateInfo u = this->getUpdateInfo();
        return u.anticipateOwnDistribution();
    }

    bool CApplication::start()
    {
        m_started = false; // reset

        // parse if needed, parsing contains its own error handling
        if (!m_parsed)
        {
            const bool s = this->parseAndStartupCheck();
            if (!s) { return false; }
        }

        // parsing itself is done
        CStatusMessageList msgs;
        do
        {
            // clear cache?
            if (this->isSet(m_cmdClearCache))
            {
                const QStringList files(CApplication::clearCaches());
                msgs.push_back(CLogMessage(this).debug() << "Cleared cache, " << files.size() << " files");
            }

            // crashpad dump
            if (this->isSet(m_cmdTestCrashpad))
            {
                msgs.push_back(CLogMessage(this).info(u"About to simulate crash"));
                QTimer::singleShot(10 * 1000, [ = ]
                {
                    if (!sApp || sApp->isShuttingDown()) { return; }
                    this->simulateCrash();
                });
            }

            //! \fixme KB 9/17 waiting for setup reader here is supposed to be replaced by explicitly waiting for reader
            if (!m_setupReader->isSetupAvailable())
            {
                msgs = this->requestReloadOfSetupAndVersion();
                if (msgs.isFailure()) { break; }
                if (msgs.isSuccess()) { msgs.push_back(this->waitForSetup()); }
            }

            // start hookin
            msgs.push_back(this->startHookIn());
            if (msgs.isFailure()) { break; }

            // Settings if not already initialized
            msgs.push_back(this->initLocalSettings());
            if (msgs.isFailure()) { break; }
        }
        while (false);

        // terminate with failures, otherwise log messages
        if (msgs.isFailure())
        {
            this->cmdLineErrorMessage(msgs);
            return false;
        }
        else if (!msgs.isEmpty())
        {
            CLogMessage::preformatted(msgs);
        }

        m_started = true;
        return m_started;
    }

    CStatusMessageList CApplication::waitForSetup(int timeoutMs)
    {
        if (!m_setupReader) { return CStatusMessage(this).error(u"No setup reader"); }
        CEventLoop eventLoop;
        eventLoop.stopWhen(this, &CApplication::setupHandlingCompleted);
        if (!m_setupReader->isSetupAvailable())
        {
            eventLoop.exec(timeoutMs);
        }

        // setup handling completed with success or failure, or we run into time out
        CStatusMessageList msgs;
        bool forced = false;
        if (!m_setupReader->isSetupAvailable())
        {
            forced = true;
            m_setupReader->forceAvailabilityUpdate(); // maybe web reading still hanging
        }
        if (m_setupReader->isSetupAvailable())
        {
            msgs.push_back(CStatusMessage(this).info(forced ? QStringLiteral("Setup available after forcing (so likely web read still pending)")
                           : QStringLiteral("Setup available")));
            return msgs;
        }

        // getting here means no "real" read success, and NO available cache
        msgs.push_back(CStatusMessage(this).error(u"Setup not available, setup reading failed or timed out."));
        if (m_setupReader->getLastSetupReadErrorMessages().hasErrorMessages())
        {
            msgs.push_back(m_setupReader->getLastSetupReadErrorMessages());
        }
        if (m_setupReader->hasCmdLineBootstrapUrl())
        {
            msgs.push_back(CStatusMessage(this).info(u"Bootstrap URL cmd line argument '%1'") << m_setupReader->getCmdLineBootstrapUrl());
        }
        return msgs;
    }

    bool CApplication::isSetupAvailable() const
    {
        if (m_shutdown || !m_setupReader) { return false; }
        return m_setupReader->isSetupAvailable();
    }

    CStatusMessageList CApplication::requestReloadOfSetupAndVersion()
    {
        if (m_shutdown)     { return CStatusMessage(this).warning(u"Shutting down, not reading"); }
        if (!m_setupReader) { return CStatusMessage(this).error(u"No reader for setup/version"); }
        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Not yet parsed");
        return m_setupReader->asyncLoad();
    }

    bool CApplication::hasMinimumMappingVersion() const
    {
        return (this->getGlobalSetup().isSwiftVersionMinimumMappingVersion());
    }

    bool CApplication::hasWebDataServices() const
    {
        if (this->isShuttingDown()) { return false; } // service will not survive for long
        return m_webDataServices;
    }

    CWebDataServices *CApplication::getWebDataServices() const
    {
        // use hasWebDataServices() to test if services are available
        // getting the assert means web services are accessed before the are initialized

        Q_ASSERT_X(m_webDataServices, Q_FUNC_INFO, "Missing web data services, use hasWebDataServices to test if existing");
        return m_webDataServices.data();
    }

    const QString &CApplication::versionStringDetailed() const
    {
        if (this->isDeveloperFlagSet() && CBuildConfig::isLocalDeveloperDebugBuild())
        {
            static const QString s(CBuildConfig::getVersionStringPlatform() % u" [dev,DEVDBG]");
            return s;
        }
        if (isDeveloperFlagSet())
        {
            static const QString s(CBuildConfig::getVersionStringPlatform() % u" [dev]");
            return s;
        }
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            static const QString s(CBuildConfig::getVersionStringPlatform() % u" [DEVDBG]");
            return s;
        }
        return CBuildConfig::getVersionStringPlatform();
    }

    const QString &CApplication::swiftVersionString() const
    {
        static const QString s(QStringLiteral("swift %1").arg(versionStringDetailed()));
        return s;
    }

    const char *CApplication::swiftVersionChar()
    {
        static const QByteArray a(swiftVersionString().toUtf8());
        return a.constData();
    }

    bool CApplication::initIsRunningInDeveloperEnvironment() const
    {
        //
        // assumption: restricted distributions are development versions
        //

        if (this->getApplicationInfo().isSampleOrUnitTest()) { return true; }
        if (CBuildConfig::isLocalDeveloperDebugBuild()) { return true; }

        const CDistribution d(this->getOwnDistribution());
        if (d.isRestricted() && this->isSet(m_cmdDevelopment)) { return true; }

        // we can globally set a dev.flag
        if (this->isSetupAvailable())
        {
            // assume value from setup
            return this->getGlobalSetup().isDevelopment();
        }
        return false;
    }

    CStatusMessage CApplication::initLocalSettings()
    {
        if (m_localSettingsLoaded) { return CStatusMessage(); }
        m_localSettingsLoaded = true;

        // trigger loading and saving of settings in appropriate scenarios
        if (m_coreFacadeConfig.getModeApplication() != CCoreFacadeConfig::Remote)
        {
            // facade running here locally
            const CStatusMessage msg = CSettingsCache::instance()->loadFromStore();
            if (msg.isFailure()) { return msg; }

            // Settings are distributed via DBus. So only one application is responsible for saving. `enableLocalSave()` means
            // "this is the application responsible for saving". If swiftgui requests a setting to be saved, it is sent to swiftcore and saved by swiftcore.
            CSettingsCache::instance()->enableLocalSave();
        }
        return CStatusMessage();
    }

    bool CApplication::hasUnsavedSettings() const
    {
        return !this->getUnsavedSettingsKeys().isEmpty();
    }

    void CApplication::saveSettingsOnShutdown(bool saveSettings)
    {
        m_saveSettingsOnShutdown = saveSettings;
    }

    QStringList CApplication::getUnsavedSettingsKeys() const
    {
        return this->supportsContexts() ?
               this->getIContextApplication()->getUnsavedSettingsKeys() :
               CSettingsCache::instance()->getAllUnsavedKeys();
    }

    CStatusMessage CApplication::saveSettingsByKey(const QStringList &keys)
    {
        if (keys.isEmpty()) { return CStatusMessage(); }
        return this->supportsContexts() ?
               this->getIContextApplication()->saveSettingsByKey(keys) :
               CSettingsCache::instance()->saveToStore(keys);
    }

    QString CApplication::getTemporaryDirectory()
    {
        static const QTemporaryDir tempDir;
        if (tempDir.isValid()) { return tempDir.path(); }
        return QDir::tempPath();
    }

    QString CApplication::getInfoString(const QString &separator) const
    {
        QString str =
            CBuildConfig::getVersionString() %
            u" " % (CBuildConfig::isReleaseBuild() ? u"Release build" : u"Debug build") %
            separator %
            u"Local dev.dbg.: " %
            boolToYesNo(CBuildConfig::isLocalDeveloperDebugBuild()) %
            separator %
            u"dev.env.: " %
            boolToYesNo(this->isDeveloperFlagSet()) %
            separator %
            u"distribution: " %
            this->getOwnDistribution().toQString(true) %
            separator %
            u"Windows NT: " %
            boolToYesNo(CBuildConfig::isRunningOnWindowsNtPlatform()) %
            u" Windows 10: " %
            boolToYesNo(CBuildConfig::isRunningOnWindows10()) %
            separator %
            u"Linux: " %
            boolToYesNo(CBuildConfig::isRunningOnLinuxPlatform()) %
            " Unix: " %
            boolToYesNo(CBuildConfig::isRunningOnUnixPlatform()) %
            separator %
            u"MacOS: " %
            boolToYesNo(CBuildConfig::isRunningOnMacOSPlatform()) %
            separator %
            "Build Abi: " %
            QSysInfo::buildAbi() %
            separator %
            u"Build CPU: " %
            QSysInfo::buildCpuArchitecture() %
            separator %
            CBuildConfig::compiledWithInfo(false);

        if (this->supportsContexts())
        {
            str += (separator % u"Supporting contexts");
            if (this->getIContextNetwork())
            {
                str += (separator % this->getIContextNetwork()->getLibraryInfo(true));
            }
        }

        return str;
    }


    QNetworkReply *CApplication::getFromNetwork(const CUrl &url, const CApplication::CallbackSlot &callback, int maxRedirects)
    {
        const CApplication::ProgressSlot progress;
        return this->getFromNetwork(url, callback, progress, maxRedirects);
    }

    QNetworkReply *CApplication::getFromNetwork(const CUrl &url, const CApplication::CallbackSlot &callback, const CApplication::ProgressSlot &progress, int maxRedirects)
    {
        return this->getFromNetwork(url.toNetworkRequest(), NoLogRequestId, callback, progress, maxRedirects);
    }

    QNetworkReply *CApplication::getFromNetwork(const CUrl &url, int logId, const CApplication::CallbackSlot &callback, const CApplication::ProgressSlot &progress, int maxRedirects)
    {
        return this->getFromNetwork(url.toNetworkRequest(), logId, callback, progress, maxRedirects);
    }

    QNetworkReply *CApplication::getFromNetwork(const QNetworkRequest &request, const CApplication::CallbackSlot &callback, int maxRedirects)
    {
        const CApplication::ProgressSlot progress;
        return this->getFromNetwork(request, callback, progress, maxRedirects);
    }

    QNetworkReply *CApplication::getFromNetwork(const QNetworkRequest &request, const CApplication::CallbackSlot &callback, const CApplication::ProgressSlot &progress, int maxRedirects)
    {
        return this->getFromNetwork(request, NoLogRequestId, callback, progress, maxRedirects);
    }

    QNetworkReply *CApplication::getFromNetwork(const QNetworkRequest &request, int logId, const CApplication::CallbackSlot &callback, const CApplication::ProgressSlot &progress, int maxRedirects)
    {
        return this->httpRequestImpl(request, logId, callback, progress, maxRedirects, [](QNetworkAccessManager & qam, const QNetworkRequest & request)
        {
            QNetworkReply *nr = qam.get(request);
            return nr;
        });
    }

    QNetworkReply *CApplication::deleteResourceFromNetwork(const QNetworkRequest &request, int logId, const CApplication::CallbackSlot &callback, int maxRedirects)
    {
        const CApplication::ProgressSlot progress;
        return this->httpRequestImpl(request, logId, callback, progress, maxRedirects, [](QNetworkAccessManager & qam, const QNetworkRequest & request)
        {
            QNetworkReply *nr = qam.deleteResource(request);
            return nr;
        });
    }

    QNetworkReply *CApplication::postToNetwork(const QNetworkRequest &request, int logId, const QByteArray &data, const CSlot<void(QNetworkReply *)> &callback)
    {
        return this->httpRequestImpl(request, logId, callback, NoRedirects, [ data ](QNetworkAccessManager & qam, const QNetworkRequest & request)
        {
            QNetworkReply *nr = qam.post(request, data);
            return nr;
        });
    }

    QNetworkReply *CApplication::postToNetwork(const QNetworkRequest &request, int logId, QHttpMultiPart *multiPart, const CSlot<void(QNetworkReply *)> &callback)
    {
        if (!this->isNetworkAccessible()) { return nullptr; }
        if (multiPart->thread() != m_accessManager->thread())
        {
            multiPart->moveToThread(m_accessManager->thread());
        }

        QPointer<CApplication> myself(this);
        return httpRequestImpl(request, logId, callback, NoRedirects, [ = ](QNetworkAccessManager & qam, const QNetworkRequest & request)
        {
            QNetworkReply *nr = nullptr;
            if (!myself) { return nr; }
            if (!multiPart) { return nr; }
            nr = qam.post(request, multiPart);
            multiPart->setParent(nr);
            return nr;
        });
    }

    QNetworkReply *CApplication::headerFromNetwork(const CUrl &url, const CallbackSlot &callback, int maxRedirects)
    {
        return headerFromNetwork(url.toNetworkRequest(), callback, maxRedirects);
    }

    QNetworkReply *CApplication::headerFromNetwork(const QNetworkRequest &request, const CallbackSlot &callback, int maxRedirects)
    {
        return httpRequestImpl(request, NoLogRequestId, callback, maxRedirects, [ ](QNetworkAccessManager & qam, const QNetworkRequest & request) { return qam.head(request); });
    }

    QNetworkReply *CApplication::downloadFromNetwork(const CUrl &url, const QString &saveAsFileName, const CSlot<void (const CStatusMessage &)> &callback, int maxRedirects)
    {
        // upfront checks
        if (url.isEmpty()) { return nullptr; }
        if (saveAsFileName.isEmpty()) { return nullptr; }
        const QFileInfo fi(saveAsFileName);
        if (!fi.dir().exists()) { return nullptr; }

        // function called with reply when done
        CallbackSlot callbackSlot([ = ](QNetworkReply * reply)
        {
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(reply);
            CStatusMessage msg;
            if (reply->error() != QNetworkReply::NoError)
            {
                msg = CStatusMessage(this, CStatusMessage::SeverityError, u"Download for '%1' failed: '%2'") << url.getFullUrl() << nwReply->errorString();
            }
            else
            {
                const bool ok = CFileUtils::writeByteArrayToFile(reply->readAll(), saveAsFileName);
                msg = ok ?
                      CStatusMessage(this, CStatusMessage::SeverityInfo,  u"Saved file '%1' downloaded from '%2'") << saveAsFileName << url.getFullUrl() :
                      CStatusMessage(this, CStatusMessage::SeverityError, u"Saving file '%1' downloaded from '%2' failed") << saveAsFileName << url.getFullUrl();
            }
            nwReply->close();
            QTimer::singleShot(0, callback.object(), [ = ]
            {
                if (!sApp || sApp->isShuttingDown()) { return; }
                callback(msg);
            });
        });
        callbackSlot.setObject(this); // object for thread

        ProgressSlot progressSlot([ = ](int, qint64, qint64, const QUrl &)
        {
            // so far not implemented
        });

        QNetworkReply *reply = this->getFromNetwork(url, callbackSlot, progressSlot, maxRedirects);
        return reply;
    }

    void CApplication::deleteAllCookies()
    {
        m_cookieManager->deleteAllCookies();
    }

    CNetworkWatchdog *CApplication::getNetworkWatchdog() const
    {
        return m_networkWatchDog;
    }

    void CApplication::setSwiftDbAccessibility(bool accessible)
    {
        if (!m_networkWatchDog) { return; }
        m_networkWatchDog->setDbAccessibility(accessible);
    }

    int CApplication::triggerNetworkWatchdogChecks()
    {
        if (!m_networkWatchDog) { return -1; }
        return m_networkWatchDog->triggerCheck();
    }

    bool CApplication::isNetworkAccessible() const
    {
        // skip test if there is no proper network config
        if (m_networkWatchDog && m_networkWatchDog->isNetworkAccessibilityCheckDisabled()) { return true; }

        Q_ASSERT_X(m_accessManager, Q_FUNC_INFO, "no access manager");
        const QNetworkAccessManager::NetworkAccessibility a = m_accessManager->networkAccessible();
        if (a == QNetworkAccessManager::Accessible) { return true; }

        // currently I also accept unknown because of that issue with Network Manager
        return a == QNetworkAccessManager::UnknownAccessibility;
    }

    bool CApplication::isInternetAccessible() const
    {
        return m_networkWatchDog && m_networkWatchDog->isInternetAccessible();
    }

    bool CApplication::isSwiftDbAccessible() const
    {
        return m_networkWatchDog && m_networkWatchDog->isSwiftDbAccessible();
    }

    bool CApplication::hasWorkingSharedUrl() const
    {
        return m_networkWatchDog && m_networkWatchDog->hasWorkingSharedUrl();
    }

    CUrl CApplication::getWorkingSharedUrl() const
    {
        if (!m_networkWatchDog || !this->isNetworkAccessible()) { return CUrl(); }
        return m_networkWatchDog->getWorkingSharedUrl();
    }

    void CApplication::exit(int retcode)
    {
        if (sApp) { instance()->gracefulShutdown(); }

        // when the event loop is not running, this does nothing
        QCoreApplication::exit(retcode);
    }

    QStringList CApplication::arguments()
    {
        return QCoreApplication::arguments();
    }

    int CApplication::indexOfCommandLineOption(const QCommandLineOption &option, const QStringList &args)
    {
        const QStringList names = option.names();
        if (names.isEmpty() || args.isEmpty()) { return -1; }
        int i = -1;
        for (const QString &arg : args)
        {
            i++;
            QString a;
            if (arg.startsWith("--")) { a = arg.mid(2); }
            else if (arg.startsWith("-")) { a = arg.mid(1); }
            else { continue; }

            if (names.contains(a, Qt::CaseInsensitive)) { return i; }
        }
        return -1;
    }

    void CApplication::argumentsWithoutOption(const QCommandLineOption &option, QStringList &args)
    {
        const int index = indexOfCommandLineOption(option, args);
        if (index < 0) { return; }

        // remove argument and its value
        args.removeAt(index);
        if (!option.valueName().isEmpty() && args.size() > index) { args.removeAt(index); }
    }

    void CApplication::processEventsFor(int milliseconds)
    {
        // sApp check allows to use it in test cases without sApp
        if (sApp && sApp->isShuttingDown()) { return; }
        QEventLoop eventLoop;
        QTimer::singleShot(milliseconds, &eventLoop, &QEventLoop::quit);
        connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();
    }

    CStatusMessageList CApplication::useContexts(const CCoreFacadeConfig &coreConfig)
    {
        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Call this function after parsing");

        m_useContexts = true;
        m_coreFacadeConfig = coreConfig;
        const CStatusMessage msg = this->initLocalSettings();
        if (msg.isFailure()) { return msg; }

        // now we can use settings
        // if not yet initialized, init web data services
        if (!m_useWebData)
        {
            const CStatusMessageList msgs = this->useWebDataServices(CWebReaderFlags::AllReaders, CDatabaseReaderConfigList::forPilotClient());
            if (msgs.hasErrorMessages()) { return msgs; }
        }
        return this->startCoreFacadeAndWebDataServices(); // will do nothing if setup is not yet loaded
    }

    CStatusMessageList CApplication::useFacadeNoContexts()
    {
        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Call this function after parsing");

        m_useContexts = true; // otherwise startCoreFacadeAndWebDataServices will early-return
        m_coreFacadeConfig = CCoreFacadeConfig::allEmpty();
        const CStatusMessage msg = this->initLocalSettings();
        if (msg.isFailure()) { return msg; }

        return this->startCoreFacadeAndWebDataServices(); // will do nothing if setup is not yet loaded
    }

    CStatusMessageList CApplication::useWebDataServices(const CWebReaderFlags::WebReader webReaders, const CDatabaseReaderConfigList &dbReaderConfig)
    {
        Q_ASSERT_X(m_webDataServices.isNull(), Q_FUNC_INFO, "Services already started");
        BLACK_VERIFY_X(QSslSocket::supportsSsl(), Q_FUNC_INFO, "No SSL");
        if (!QSslSocket::supportsSsl())
        {
            return CStatusMessage(this).error(u"No SSL supported, can`t be used");
        }

        m_webReadersUsed = webReaders;
        m_dbReaderConfig = dbReaderConfig;
        m_useWebData = true;
        return this->startWebDataServices();
    }

    bool CApplication::isLocalContext() const
    {
        return this->getIContextApplication() && this->getIContextApplication()->isUsingImplementingObject();
    }

    bool CApplication::isDBusContext() const
    {
        return this->getIContextApplication() && !this->getIContextApplication()->isUsingImplementingObject() && !this->getIContextApplication()->isEmptyObject();
    }

    CStatusMessageList CApplication::startCoreFacadeAndWebDataServices()
    {
        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Call this function after parsing");

        if (!m_useContexts) { return CStatusMessage(this).error(u"No need to start core facade"); } // we do not use context, so no need to startup
        if (!m_setupReader || !m_setupReader->isSetupAvailable()) { return CStatusMessage(this).error(u"No setup reader or setup available"); }

        Q_ASSERT_X(m_coreFacade.isNull(), Q_FUNC_INFO, "Cannot alter facade");
        Q_ASSERT_X(m_setupReader, Q_FUNC_INFO, "No facade without setup possible");
        Q_ASSERT_X(m_useWebData, Q_FUNC_INFO, "Need web data services");

        this->startWebDataServices();

        const CStatusMessageList msgs(CStatusMessage(this).info(u"Will start core facade now"));
        m_coreFacade.reset(new CCoreFacade(m_coreFacadeConfig));
        emit this->coreFacadeStarted();
        return msgs;
    }

    CStatusMessageList CApplication::startWebDataServices()
    {
        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Call this function after parsing");

        if (!m_useWebData) { return CStatusMessage(this).warning(u"No need to start web data services"); }
        if (!m_setupReader || !m_setupReader->isSetupAvailable()) { return CStatusMessage(this).error(u"No setup reader or setup available"); }

        Q_ASSERT_X(m_setupReader, Q_FUNC_INFO, "No web data services without setup possible");
        CStatusMessageList msgs;
        if (!m_webDataServices)
        {
            msgs.push_back(CStatusMessage(this).info(u"Will start web data services now"));
            m_webDataServices.reset(
                new CWebDataServices(m_webReadersUsed, m_dbReaderConfig, {}, this)
            );
            Q_ASSERT_X(m_webDataServices, Q_FUNC_INFO, "Missing web services");

            // caches from local files (i.e. the files delivered)
            if (this->isInstallerOptionSet())
            {
                const QDateTime ts = m_webDataServices->getLatestDbEntityCacheTimestamp();
                if (!ts.isValid() || ts < QDateTime::currentDateTimeUtc().addYears(-2))
                {
                    // we only init, if there are:
                    // a) no cache timestamps b) or it was not updated for some years
                    msgs.push_back(m_webDataServices->initDbCachesFromLocalResourceFiles(false));
                }
            }

            // watchdog
            if (m_networkWatchDog)
            {
                connect(m_webDataServices.data(), &CWebDataServices::swiftDbDataRead, m_networkWatchDog, &CNetworkWatchdog::setDbAccessibility);
            }

            emit this->webDataServicesStarted(true);
        }
        else
        {
            msgs.push_back(CStatusMessage(this).info(u"Web data services already running"));
        }

        return msgs;
    }

    void CApplication::initLogging()
    {
        CLogHandler::instance()->install(); // make sure we have a log handler!

        // File logger
        m_fileLogger.reset(new CFileLogger(this));
        connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, m_fileLogger.data(), &CFileLogger::writeStatusMessageToFile);
        connect(CLogHandler::instance(), &CLogHandler::remoteMessageLogged, m_fileLogger.data(), &CFileLogger::writeStatusMessageToFile);
        m_fileLogger->changeLogPattern(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityDebug));
    }

    void CApplication::initParser()
    {
        m_parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
        m_parser.setApplicationDescription(m_applicationName);
        m_cmdHelp = m_parser.addHelpOption();
        m_cmdVersion = m_parser.addVersionOption();
        m_allOptions.append(m_cmdHelp);
        m_allOptions.append(m_cmdVersion);

        // dev. system
        m_cmdDevelopment = QCommandLineOption({ "dev", "development" },
                                              QCoreApplication::translate("application", "Dev. system features?"));
        this->addParserOption(m_cmdDevelopment);

        // can read a local bootstrap file
        m_cmdSharedDir = QCommandLineOption({ "shared", "shareddir" },
                                            QCoreApplication::translate("application", "Local shared directory."),
                                            "shared");
        this->addParserOption(m_cmdSharedDir);

        // Skip single application check
        m_cmdSkipSingleApp = QCommandLineOption({ "skipsa", "skipsingleapp" },
                                                QCoreApplication::translate("application", "Skip the single app.test."));
        this->addParserOption(m_cmdSkipSingleApp);

        // reset caches upfront
        m_cmdClearCache = QCommandLineOption({ "ccache", "clearcache" },
                                             QCoreApplication::translate("application", "Clear (reset) the caches."));
        this->addParserOption(m_cmdClearCache);

        // test crashpad upload
        m_cmdTestCrashpad = QCommandLineOption({ "testcp", "testcrashpad" },
                                               QCoreApplication::translate("application", "Trigger crashpad situation."));
        this->addParserOption(m_cmdTestCrashpad);
    }

    bool CApplication::isSet(const QCommandLineOption &option) const
    {
        return (m_parser.isSet(option));
    }

    void CApplication::registerMetadata()
    {
        BlackMisc::registerMetadata();
        BlackCore::registerMetadata();
    }

    QStringList CApplication::clearCaches()
    {
        const QStringList files(CDataCache::instance()->enumerateStore());
        CDataCache::instance()->clearAllValues();
        return files;
    }

    void CApplication::gracefulShutdown()
    {
        if (m_shutdown) { return; }
        if (m_shutdownInProgress) { return; }
        m_shutdownInProgress = true;

        CLogMessage(this).info(u"Graceful shutdown of CApplication started");

        // info that we will shutdown
        emit this->aboutToShutdown();

        // Release all input devices to not cause any accidental hotkey triggers anymore.
        // This is also necessary to properly free platform specific instances at a defined point in time.
        if (m_inputManager)
        {
            CLogMessage(this).info(u"Graceful shutdown of CApplication, released devices");
            m_inputManager->releaseDevices();
        }

        // mark as shutdown
        if (m_networkWatchDog) { m_networkWatchDog->gracefulShutdown(); }

        // save settings (but only when application was really alive)
        if (m_parsed && m_saveSettingsOnShutdown)
        {
            const CStatusMessage m = this->supportsContexts() ?
                                     this->getIContextApplication()->saveSettings() :
                                     CSettingsCache::instance()->saveToStore();
            CLogMessage(this).preformatted(m);
        }

        // from here on we really rip apart the application object
        // and it should no longer be used

        if (this->supportsContexts(true))
        {
            CLogMessage(this).info(u"Graceful shutdown of CApplication, shutdown of contexts");

            // clean up facade
            m_coreFacade->gracefulShutdown();
            m_coreFacade.reset();
        }

        if (m_webDataServices)
        {
            CLogMessage(this).info(u"Graceful shutdown of CApplication, shutdown of web services");

            m_webDataServices->gracefulShutdown();
            m_webDataServices.reset();
        }

        if (m_gitHubPackagesReader)
        {
            m_gitHubPackagesReader.reset();
        }

        if (m_setupReader)
        {
            m_setupReader->gracefulShutdown();
            m_setupReader.reset();
        }

        if (m_networkWatchDog)
        {
            m_networkWatchDog->quitAndWait();
            m_networkWatchDog = nullptr;
        }

        CLogMessage(this).info(u"Graceful shutdown of CApplication, shutdown of logger");
        m_fileLogger->close();

        // clean up all in "deferred delete state"
        qApp->sendPostedEvents(nullptr, QEvent::DeferredDelete);
        sApp->processEventsFor(500);

        // completed
        m_shutdown = true;
        sApp = nullptr;

        disconnect(this);
    }

    void CApplication::onSetupHandlingCompleted(bool available)
    {
        if (available)
        {
            // start follow ups when setup is avaialable
            const CStatusMessageList msgs = this->asyncWebAndContextStart();
            m_started = msgs.isSuccess();
        }

        if (m_signalStartup) { emit this->startUpCompleted(m_started); }
    }

    void CApplication::onStartUpCompleted()
    {
        // void
    }

    void CApplication::onChangedNetworkAccessibility(QNetworkAccessManager::NetworkAccessibility accessible)
    {
        switch (accessible)
        {
        case QNetworkAccessManager::Accessible:
            m_accessManager->setNetworkAccessible(accessible); // for some reasons the queried value still is unknown
            CLogMessage(this).info(u"Network is accessible");
            break;
        case QNetworkAccessManager::NotAccessible:
            CLogMessage(this).error(u"Network not accessible");
            break;
        default:
            CLogMessage(this).warning(u"Network accessibility unknown");
            break;
        }
    }

    void CApplication::onChangedInternetAccessibility(bool accessible)
    {
        if (accessible) { CLogMessage(this).info(u"Internet reported accessible"); }
        else { CLogMessage(this).warning(u"Internet not accessible"); }

        emit this->changedInternetAccessibility(accessible);
    }

    void CApplication::onChangedSwiftDbAccessibility(bool accessible, const CUrl &url)
    {
        if (accessible)
        {
            CLogMessage(this).info(u"swift DB reported accessible: '%1'") << url.toQString();
        }
        else
        {
            CLogMessage(this).warning(u"swift DB not accessible: '%1'") << url.toQString();
            if (m_networkWatchDog)
            {
                CLogMessage(this).warning(m_networkWatchDog->getCheckInfo());
            }
            this->triggerNetworkAccessibilityCheck(10 * 1000); // crosscheck after some time
        }

        emit this->changedSwiftDbAccessibility(accessible, url);
    }

    void CApplication::onNetworkConfigurationsUpdateCompleted()
    {
        Q_ASSERT_X(m_networkConfigManager, Q_FUNC_INFO, "Need network config manager");
        if (this->isShuttingDown()) { return; }
        const QList<QNetworkConfiguration> allConfigurations = m_networkConfigManager->allConfigurations();
        if (allConfigurations.isEmpty())
        {
            // this is an odd situation we cannot handle, network check will be disabled
            if (m_networkWatchDog && m_networkWatchDog->isNetworkAccessibilityCheckEnabled())
            {
                m_networkWatchDog->disableNetworkAccessibilityCheck(true);
                m_accessManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
                CLogMessage(this).warning(u"No network configurations found, disabling network accessibility checks");
            }
        }
        else
        {
            int activeCount = 0;
            int validCount  = 0;
            for (const QNetworkConfiguration &config : allConfigurations)
            {
                if (config.state() == QNetworkConfiguration::Active) { activeCount++; m_noNwAccessPoint = false; }
                if (config.isValid()) { validCount++; }
            }
            Q_UNUSED(validCount)

            const bool canStartIAP = (m_networkConfigManager->capabilities() & QNetworkConfigurationManager::CanStartAndStopInterfaces);
            const bool disable = activeCount < 1; // only inactive
            if (disable && m_networkWatchDog && m_networkWatchDog->isNetworkAccessibilityCheckEnabled())
            {
                CLogMessage(this).warning(u"Disabling network accessibility check in watchdog");
                m_networkWatchDog->disableNetworkAccessibilityCheck(disable);
            }

            // Is there default access point, use it
            const QNetworkConfiguration config = m_networkConfigManager->defaultConfiguration();
            if (!config.isValid() || (!canStartIAP && config.state() != QNetworkConfiguration::Active))
            {
                if (!m_noNwAccessPoint)
                {
                    m_noNwAccessPoint = true;
                    CLogMessage(this).warning(u"No network access point found for swift");
                }
            }
        }
    }

    void CApplication::initNetwork()
    {
        if (!m_accessManager) { m_accessManager = new QNetworkAccessManager(this); }
        if (!m_networkConfigManager) { m_networkConfigManager = new QNetworkConfigurationManager(this); }

        if (!m_networkWatchDog)
        {
            // CNetworkWatchdog *nwWatchdog = new CNetworkWatchdog(this->isNetworkAccessible(), this);
            CNetworkWatchdog *nwWatchdog = new CNetworkWatchdog(true, this); // WLAN bug, default to true
            m_networkWatchDog = nwWatchdog; // not yet started
            m_cookieManager = new CCookieManager({}, this);
            m_cookieManager->setParent(m_accessManager);
            m_accessManager->setCookieJar(m_cookieManager);
        }

        // Init network
        Q_ASSERT_X(m_accessManager, Q_FUNC_INFO, "Need QAM");
        Q_ASSERT_X(m_networkConfigManager, Q_FUNC_INFO, "Need config manager");

        // into watchdog
        connect(m_accessManager, &QNetworkAccessManager::networkAccessibleChanged, m_networkWatchDog, &CNetworkWatchdog::setNetworkAccessibility, Qt::QueuedConnection);
        connect(m_networkConfigManager, &QNetworkConfigurationManager::onlineStateChanged, m_networkWatchDog, &CNetworkWatchdog::setOnline, Qt::QueuedConnection);
        connect(m_networkConfigManager, &QNetworkConfigurationManager::updateCompleted, m_networkWatchDog, &CNetworkWatchdog::networkConfigurationsUpdateCompleted, Qt::QueuedConnection);
        connect(m_networkConfigManager, &QNetworkConfigurationManager::updateCompleted, this, &CApplication::onNetworkConfigurationsUpdateCompleted, Qt::QueuedConnection);
        m_networkConfigManager->updateConfigurations();

        // out from watchdog to application
        connect(m_networkWatchDog, &CNetworkWatchdog::changedNetworkAccessible, this, &CApplication::onChangedNetworkAccessibility, Qt::QueuedConnection);
        connect(m_networkWatchDog, &CNetworkWatchdog::changedInternetAccessibility, this, &CApplication::onChangedInternetAccessibility, Qt::QueuedConnection);
        connect(m_networkWatchDog, &CNetworkWatchdog::changedSwiftDbAccessibility, this, &CApplication::onChangedSwiftDbAccessibility, Qt::QueuedConnection);

        CLogMessage::preformatted(CNetworkUtils::createNetworkReport(m_accessManager));
        m_networkWatchDog->start(QThread::LowestPriority);
        m_networkWatchDog->startUpdating(10);

        // enable by setting accessible
        // http://doc.qt.io/qt-5/qnetworkaccessmanager.html#setNetworkAccessible
        m_accessManager->setNetworkAccessible(QNetworkAccessManager::Accessible);


        // create a network report in the log
        QTimer::singleShot(4000, this, [ = ]
        {
            if (!sApp || sApp->isShuttingDown()) { return; }
            const QString r = CNetworkUtils::createNetworkConfigurationReport(m_networkConfigManager, m_accessManager);
            CLogMessage(this).info(u"Network report:\n%1") << r;
        });
    }

    CStatusMessageList CApplication::asyncWebAndContextStart()
    {
        if (m_started) { return CStatusMessage(this).info(u"Already started "); }

        // follow up startups
        CStatusMessageList msgs = this->startWebDataServices();
        if (msgs.isFailure()) return msgs;
        msgs.push_back(this->startCoreFacadeAndWebDataServices());
        return msgs;
    }

    void CApplication::severeStartupProblem(const CStatusMessage &message)
    {
        CLogMessage::preformatted(message);
        this->cmdLineErrorMessage(message.getMessage());
        this->exit(EXIT_FAILURE);

        // if I get here the event loop was not yet running
        std::exit(EXIT_FAILURE);
    }

    CApplication *BlackCore::CApplication::instance()
    {
        return sApp;
    }

    const QString &CApplication::executable()
    {
        static const QString e(QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName());
        return e;
    }

    const QStringList &CApplication::getLogCategories()
    {
        static const QStringList l({ "swift.application", "swift." % executable()});
        return l;
    }

    // ---------------------------------------------------------------------------------
    // Parsing
    // ---------------------------------------------------------------------------------

    bool CApplication::addParserOption(const QCommandLineOption &option)
    {
        m_allOptions.append(option);
        return m_parser.addOption(option);
    }

    bool CApplication::addParserOptions(const QList<QCommandLineOption> &options)
    {
        m_allOptions.append(options);
        return m_parser.addOptions(options);
    }

    void CApplication::addDBusAddressOption()
    {
        m_cmdDBusAddress = QCommandLineOption({ "dbus", "dbusaddress" },
                                              QCoreApplication::translate("application", "DBus address (session, system, P2P IP e.g. 192.168.23.5)"),
                                              "dbusaddress");
        this->addParserOption(m_cmdDBusAddress);
    }

    void CApplication::addVatlibOptions()
    {
        this->addParserOptions(IContextNetwork::getCmdLineOptions());
    }

    void CApplication::addAudioOptions()
    {
        this->addParserOptions(CContextAudioBase::getCmdLineOptions());
    }

    QString CApplication::getCmdDBusAddressValue() const
    {
        if (!this->isParserOptionSet(m_cmdDBusAddress)) { return {}; }
        const QString v(this->getParserValue(m_cmdDBusAddress));
        const QString dBusAddress(CDBusServer::normalizeAddress(v));
        return dBusAddress;
    }

    QString CApplication::getCmdSwiftPrivateSharedDir() const
    {
        return m_parser.value(m_cmdSharedDir);
    }

    bool CApplication::isParserOptionSet(const QString &option) const
    {
        return m_parser.isSet(option);
    }

    bool CApplication::isInstallerOptionSet() const
    {
        return this->isParserOptionSet("installer");
    }

    bool CApplication::skipSingleApplicationCheck() const
    {
        return this->isParserOptionSet(m_cmdSkipSingleApp);
    }

    bool CApplication::isParserOptionSet(const QCommandLineOption &option) const
    {
        return m_parser.isSet(option);
    }

    QString CApplication::getParserValue(const QString &option) const
    {
        return m_parser.value(option).trimmed();
    }

    QString CApplication::getParserValue(const QCommandLineOption &option) const
    {
        return m_parser.value(option).trimmed();
    }

    bool CApplication::parseAndStartupCheck()
    {
        if (m_parsed) { return m_parsed; } // already done

        // checks
        if (CBuildConfig::isLifetimeExpired())
        {
            this->cmdLineErrorMessage("Program expired since " + CBuildConfig::getEol().date().toString(),
                                      "This version is no longer supported and usable. You have to install a newer version.");
            return false;
        }

        if(CBuildConfig::daysTillLifetimeExpiry() <= 30)
        {
            this->cmdLineWarningMessage("This version will expire in " + QString::number(CBuildConfig::daysTillLifetimeExpiry()) + " days!",
                                            "You'll need to update swift in order to use it thereafter.");
        }

        const QStringList verifyErrors = CSwiftDirectories::verifyRuntimeDirectoriesAndFiles();
        if (!verifyErrors.isEmpty() && !m_applicationInfo.isUnitTest())
        {
            this->cmdLineErrorMessage("Missing runtime directories/files:", verifyErrors.join(", "));
            return false;
        }

        // we call parse because we also want to display a GUI error message when applicable
        const QStringList args(QCoreApplication::instance()->arguments());
        if (!m_parser.parse(args))
        {
            this->cmdLineErrorMessage("Parser error:", m_parser.errorText());
            return false;
        }

        if (m_singleApplication && m_alreadyRunning && !this->skipSingleApplicationCheck())
        {
            this->cmdLineErrorMessage("Program must only run once", "You cannot run two or more instances side-by-side.");
            return false;
        }

        // help/version
        if (m_parser.isSet(m_cmdHelp))
        {
            // Important: parser help will already stop application
            this->cmdLineHelpMessage();
            return false;
        }
        if (m_parser.isSet(m_cmdVersion))
        {
            // Important: version will already stop application
            this->cmdLineVersionMessage();
            return false;
        }

        // dev.
        m_devFlag = this->initIsRunningInDeveloperEnvironment();

        // Hookin, other parsing
        if (!this->parsingHookIn()) { return false; }

        // setup reader
        m_setupReader->parseCmdLineArguments();
        m_parsed = true;
        return true;
    }

    bool CApplication::parseAndSynchronizeSetup(int timeoutMs)
    {
        if (!this->parseAndStartupCheck()) return false;
        return !this->synchronizeSetup(timeoutMs).hasErrorMessages();
    }

    bool CApplication::cmdLineWarningMessage(const QString &text, const QString &informativeText) const
    {
        fputs(qPrintable(text + informativeText), stderr);
        return false;
    }

    bool CApplication::cmdLineErrorMessage(const QString &text, const QString &informativeText, bool retry) const
    {
        Q_UNUSED(retry) // only works with UI version
        fputs(qPrintable(text + informativeText), stderr);
        return false;
    }

    bool CApplication::cmdLineErrorMessage(const CStatusMessageList &msgs, bool retry) const
    {
        Q_UNUSED(retry) // only works with UI version
        if (msgs.isEmpty()) { return false; }
        if (!msgs.hasErrorMessages())  { return false; }
        CApplication::cmdLineErrorMessage(
            msgs.toQString(true)
        );
        return false;
    }

    QString CApplication::cmdLineArgumentsAsString(bool withExecutable)
    {
        QStringList args = QCoreApplication::arguments();
        if (!withExecutable && !args.isEmpty()) args.removeFirst();
        if (args.isEmpty()) return {};
        return args.join(' ');
    }

    QPointer<ISimulator> CApplication::getISimulator() const
    {
        if (!this->hasSimulator()) { return nullptr; }
        return this->getCoreFacade()->getCContextSimulator()->simulator();
    }

    bool CApplication::hasSimulator() const
    {
        if (!this->getCoreFacade()) { return false; }
        if (!this->getCoreFacade()->getIContextSimulator()->isUsingImplementingObject()) { return false; }
        return (this->getCoreFacade()->getCContextSimulator()); // should always be true
    }

    void CApplication::cmdLineHelpMessage()
    {
        m_parser.showHelp(); // terminates
        Q_UNREACHABLE();
    }

    void CApplication::cmdLineVersionMessage() const
    {
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()), qPrintable(QCoreApplication::applicationVersion()));
    }

    QStringList CApplication::argumentsJoined(const QStringList &newArguments, const QStringList &removeArguments) const
    {
        QStringList joinedArguments = CApplication::arguments();
        QStringList newArgumentsChecked = newArguments;

        // remove the executable argument if it exists at position 0
        if (!joinedArguments.isEmpty() && !joinedArguments.at(0).startsWith("-")) { joinedArguments.removeFirst(); } // was cmd line argument
        if (!newArgumentsChecked.isEmpty() && !newArgumentsChecked.at(0).startsWith("-")) { newArgumentsChecked.removeFirst(); } // was cmd line argument

        // remove all values before checking options
        static const QRegularExpression regExp("^-");
        QStringList toBeRemoved(newArgumentsChecked.filter(regExp));
        toBeRemoved.append(removeArguments.filter(regExp));
        toBeRemoved.append("--installer");
        toBeRemoved.removeDuplicates();

        if (!joinedArguments.isEmpty() && !toBeRemoved.isEmpty())
        {
            // remove all options from removeArguments
            // consider alias names, that is why we check on option
            for (const QCommandLineOption &option : m_allOptions)
            {
                const int n = indexOfCommandLineOption(option, toBeRemoved);
                if (n >= 0)
                {
                    argumentsWithoutOption(option, joinedArguments);
                }
            }
        }

        joinedArguments.append(newArgumentsChecked);
        return joinedArguments;
    }

    // ---------------------------------------------------------------------------------
    // Contexts
    // ---------------------------------------------------------------------------------

    SharedState::CDataLinkDBus *CApplication::getDataLinkDBus()
    {
        return getCoreFacade()->getDataLinkDBus();
    }

    bool CApplication::supportsContexts(bool ignoreShutdownTest) const
    {
        if (!ignoreShutdownTest && m_shutdown) { return false; }
        if (m_coreFacade.isNull()) { return false; }
        if (!m_coreFacade->getIContextApplication()) { return false; }
        return (!m_coreFacade->getIContextApplication()->isEmptyObject());
    }

    const IContextNetwork *CApplication::getIContextNetwork() const
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getIContextNetwork();
    }

    const IContextAudio *CApplication::getIContextAudio() const
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getIContextAudio();
    }

    const CContextAudioBase *CApplication::getCContextAudioBase() const
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getCContextAudioBase();
    }

    const IContextApplication *CApplication::getIContextApplication() const
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getIContextApplication();
    }

    const IContextOwnAircraft *CApplication::getIContextOwnAircraft() const
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getIContextOwnAircraft();
    }

    const IContextSimulator *CApplication::getIContextSimulator() const
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getIContextSimulator();
    }

    IContextNetwork *CApplication::getIContextNetwork()
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getIContextNetwork();
    }

    IContextAudio *CApplication::getIContextAudio()
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getIContextAudio();
    }

    CContextAudioBase *CApplication::getCContextAudioBase()
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getCContextAudioBase();
    }

    IContextApplication *CApplication::getIContextApplication()
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getIContextApplication();
    }

    IContextOwnAircraft *CApplication::getIContextOwnAircraft()
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getIContextOwnAircraft();
    }

    IContextSimulator *CApplication::getIContextSimulator()
    {
        if (!supportsContexts()) { return nullptr; }
        return m_coreFacade->getIContextSimulator();
    }

    void CApplication::onCoreFacadeStarted()
    {
        // void
    }

    // ---------------------------------------------------------------------------------
    // Setup
    // ---------------------------------------------------------------------------------

    bool CApplication::hasSetupReader() const
    {
        return m_setupReader;
    }

    CSetupReader *CApplication::getSetupReader() const
    {
        return m_setupReader.data();
    }

    QString CApplication::getLastSuccesfulSetupUrl() const
    {
        if (!this->hasSetupReader()) { return {}; }
        return m_setupReader->getLastSuccessfulSetupUrl();
    }

    CStatusMessageList CApplication::synchronizeSetup(int timeoutMs)
    {
        const CStatusMessageList requestMsgs = this->requestReloadOfSetupAndVersion();
        if (requestMsgs.isFailure()) { return requestMsgs; } // request already failed
        return this->waitForSetup(timeoutMs);
    }

    CUrlList CApplication::getVatsimMetarUrls() const
    {
        if (m_shutdown) { return CUrlList(); }
        if (m_webDataServices)
        {
            const CUrlList urls(m_webDataServices->getVatsimMetarUrls());
            if (!urls.isEmpty()) { return urls; }
        }
        if (m_setupReader)
        {
            return m_setupReader->getSetup().getVatsimMetarsUrls();
        }
        return CUrlList();
    }

    CUrlList CApplication::getVatsimDataFileUrls() const
    {
        if (m_shutdown) { return CUrlList(); }
        if (m_webDataServices)
        {
            const CUrlList urls(m_webDataServices->getVatsimDataFileUrls());
            if (!urls.isEmpty()) { return urls; }
        }
        if (m_setupReader)
        {
            return m_setupReader->getSetup().getVatsimDataFileUrls();
        }
        return CUrlList();
    }

    void CApplication::onCrashDumpUploadEnabledChanged()
    {
        const bool enabled = CBuildConfig::isReleaseBuild() && m_crashDumpSettings.getThreadLocal().isEnabled();
        this->enableCrashDumpUpload(enabled);
    }

    void CApplication::simulateCrash()
    {
        CCrashHandler::instance()->simulateCrash();
    }

    void CApplication::simulateAssert()
    {
        CCrashHandler::instance()->simulateAssert();
    }

    void CApplication::enableCrashDumpUpload(bool enable)
    {
        CCrashHandler::instance()->setUploadsEnabled(enable);
    }

    bool CApplication::isSupportingCrashpad() const
    {
#ifdef BLACK_USE_CRASHPAD
        return true;
#else
        return false;
#endif
    }

    void CApplication::httpRequestImplInQAMThread(const QNetworkRequest &request, int logId, const CallbackSlot &callback, const ProgressSlot &progress, int maxRedirects, NetworkRequestOrPostFunction getPostOrDeleteRequest)
    {
        // run in QAM thread
        if (this->isShuttingDown()) { return; }
        QTimer::singleShot(0, m_accessManager, [ = ]
        {
            // should be now in QAM thread
            if (!sApp || sApp->isShuttingDown()) { return; }
            Q_ASSERT_X(CThreadUtils::isInThisThread(sApp->m_accessManager), Q_FUNC_INFO, "Wrong thread, must be QAM thread");
            this->httpRequestImpl(request, logId, callback, progress, maxRedirects, getPostOrDeleteRequest);
        });
    }

    void CApplication::triggerNetworkAccessibilityCheck(int deferredMs)
    {
        if (this->isShuttingDown()) { return; }
        if (!m_networkWatchDog) { return; }
        QTimer::singleShot(deferredMs, m_accessManager, [ = ]
        {
            // should be now in QAM thread
            if (!sApp || sApp->isShuttingDown()) { return; }
            Q_ASSERT_X(CThreadUtils::isInThisThread(sApp->m_accessManager), Q_FUNC_INFO, "Wrong thread, must be QAM thread");
            const QNetworkAccessManager::NetworkAccessibility accessibility = m_accessManager->networkAccessible();
            m_networkWatchDog->setNetworkAccessibility(accessibility);
        });
    }

    QNetworkReply *CApplication::httpRequestImpl(
        const QNetworkRequest &request, int logId,
        const CApplication::CallbackSlot &callback, int maxRedirects, NetworkRequestOrPostFunction requestOrPostMethod)
    {
        ProgressSlot progress;
        return this->httpRequestImpl(request, logId, callback, progress, maxRedirects, requestOrPostMethod);
    }

    QNetworkReply *CApplication::httpRequestImpl(
        const QNetworkRequest &request, int logId,
        const CallbackSlot &callback, const ProgressSlot &progress, int maxRedirects, NetworkRequestOrPostFunction getPostOrDeleteRequest)
    {
        if (this->isShuttingDown()) { return nullptr; }
        if (!this->isNetworkAccessible()) { return nullptr; }

        QWriteLocker locker(&m_accessManagerLock);
        Q_ASSERT_X(m_accessManager->thread() == qApp->thread(), Q_FUNC_INFO, "Network manager supposed to be in main thread");
        if (!CThreadUtils::isInThisThread(m_accessManager))
        {
            this->httpRequestImplInQAMThread(request, logId, callback, progress, maxRedirects, getPostOrDeleteRequest);
            return nullptr; // not yet started, will be called again in QAM thread
        }

        Q_ASSERT_X(CThreadUtils::isInThisThread(m_accessManager), Q_FUNC_INFO, "Network manager thread mismatch");
        QNetworkRequest copiedRequest = CNetworkUtils::getSwiftNetworkRequest(request, this->getApplicationNameAndVersion());

        // If URL is one of the shared URLs, add swift client SSL certificate to request
        // CNetworkUtils::setSwiftClientSslCertificate(copiedRequest, this->getGlobalSetup().getSwiftSharedUrls());

        QNetworkReply *reply = getPostOrDeleteRequest(*m_accessManager, copiedRequest);
        reply->setProperty("started", QVariant(QDateTime::currentMSecsSinceEpoch()));
        reply->setProperty(CUrlLog::propertyNameId(), QVariant(logId));
        const QUrl url(reply->url());
        QString urlStr = url.toString();

        if (progress)
        {
            connect(reply, &QNetworkReply::downloadProgress, progress.object(), [ = ](qint64 current, qint64 max)
            {
                progress(logId, current, max, url);
            });
        }

        if (callback)
        {
            Q_ASSERT_X(callback.object(), Q_FUNC_INFO, "Need callback object (to determine thread)");
            connect(reply, &QNetworkReply::finished, callback.object(), [ = ]
            {
                // Called when finished!
                // QNetworkRequest::FollowRedirectsAttribute would allow auto redirect, but we use our approach as it gives us better control
                // \fixme: Check again on Qt 5.9: Added redirects policy to QNetworkAccessManager (ManualRedirectsPolicy, NoLessSafeRedirectsPolicy, SameOriginRedirectsPolicy, UserVerifiedRedirectsPolicy)
                const bool isRedirect = CNetworkUtils::isHttpStatusRedirect(reply);
                if (isRedirect && maxRedirects > 0)
                {
                    const QUrl redirectUrl = CNetworkUtils::getHttpRedirectUrl(reply);
                    if (!redirectUrl.isEmpty())
                    {
                        QNetworkRequest redirectRequest(redirectUrl);
                        const int redirectsLeft = maxRedirects - 1;
                        CLogMessage(sApp).info(u"Redirecting '%1' to '%2'") << urlStr << redirectUrl.toString();
                        this->httpRequestImplInQAMThread(redirectRequest, logId, callback, progress, redirectsLeft, getPostOrDeleteRequest);
                        return;
                    }
                }
                // called when there are no more callbacks
                callback(reply);

            }, Qt::QueuedConnection); // called in callback thread
        }
        return reply;
    }

    void CApplication::tagApplicationDataDirectory()
    {
        const QString d = CSwiftDirectories::normalizedApplicationDataDirectory();
        const QDir dir(d);
        if (!dir.exists() || !dir.isReadable()) { return; }
        const QString aiStr(this->getApplicationInfo().toJsonString());
        const QString filePath(CFileUtils::appendFilePaths(dir.path(), CApplicationInfo::fileName())); // will be overridden by next swift app
        CFileUtils::writeStringToFile(aiStr, filePath);
    }
} // ns
