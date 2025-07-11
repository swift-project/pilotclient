// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/application.h"

#include <cstdio>
#include <cstdlib>

#include <QCoreApplication>
#include <QDateTime>
#include <QEventLoop>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QRegularExpression>
#include <QSslSocket>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QStringList>
#include <QSysInfo>
#include <QTemporaryDir>
#include <QThread>
#include <QTimer>
#include <QWriteLocker>
#include <Qt>
#include <QtGlobal>

#include "config/buildconfig.h"
#include "core/context/contextapplication.h"
#include "core/context/contextaudio.h"
#include "core/context/contextnetwork.h"
#include "core/context/contextsimulatorimpl.h"
#include "core/cookiemanager.h"
#include "core/corefacade.h"
#include "core/inputmanager.h"
#include "core/registermetadata.h"
#include "core/setupreader.h"
#include "core/webdataservices.h"
#include "misc/applicationinfo.h"
#include "misc/crashhandler.h"
#include "misc/datacache.h"
#include "misc/dbusserver.h"
#include "misc/eventloop.h"
#include "misc/filelogger.h"
#include "misc/loghandler.h"
#include "misc/logmessage.h"
#include "misc/logpattern.h"
#include "misc/network/networkutils.h"
#include "misc/registermetadata.h"
#include "misc/settingscache.h"
#include "misc/stringutils.h"
#include "misc/swiftdirectories.h"
#include "misc/threadutils.h"
#include "misc/verify.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::db;
using namespace swift::misc::network;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::misc::weather;
using namespace swift::core;
using namespace swift::core::context;
using namespace swift::core::vatsim;
using namespace swift::core::data;
using namespace swift::core::db;

swift::core::CApplication *sApp = nullptr; // set by constructor

//! \private
static const QString &swiftDataRoot()
{
    static const QString path =
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/org.swift-project/";
    return path;
}

namespace swift::core
{
    CApplication::CApplication(CApplicationInfo::Application application, bool init)
        : CApplication(executable(), application, init)
    {}

    CApplication::CApplication(const QString &applicationName, CApplicationInfo::Application application, bool init)
        : CIdentifiable(this), m_accessManager(new QNetworkAccessManager(this)), m_applicationInfo(application),
          m_applicationName(applicationName), m_coreFacadeConfig(CCoreFacadeConfig::NotUsed)
    {
        Q_ASSERT_X(!sApp, Q_FUNC_INFO, "already initialized");
        Q_ASSERT_X(QCoreApplication::instance(), Q_FUNC_INFO, "no application object");

        m_applicationInfo.setApplicationDataDirectory(CSwiftDirectories::normalizedApplicationDataDirectory());
        QCoreApplication::setApplicationName(m_applicationName);
        QCoreApplication::setApplicationVersion(CBuildConfig::getVersionString());
        this->setObjectName(m_applicationName);
        this->thread()->setObjectName(
            m_applicationName); // normally no effect as thread already runs, but does not harm either

        // init skipped when called from CGuiApplication
        if (init) { this->init(true); }
    }

    void CApplication::init(bool withMetadata)
    {
        if (!sApp)
        {
            // notify when app goes down
            connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this,
                    &CApplication::gracefulShutdown);

            // metadata
            if (withMetadata) { CApplication::registerMetadata(); }

            // unit test
            if (this->getApplicationInfo().getApplication() == CApplicationInfo::UnitTest)
            {
                const QString tempPath(CApplication::getTemporaryDirectory());
                swift::misc::setMockCacheRootDirectory(tempPath);
            }
            m_alreadyRunning = CApplication::getRunningApplications().containsApplication(
                CApplication::getApplicationInfo().getApplication());
            this->initParser();
            this->initLogging();
            this->tagApplicationDataDirectory();

            //
            // cmd line arguments not yet parsed here
            //

            // main app
            sApp = this;

            this->initNetwork();

            // global setup
            m_setupReader.reset(new CSetupReader(this));

            // check for updates
            m_gitHubPackagesReader.reset(new CGitHubPackagesReader(this));
            connect(m_gitHubPackagesReader.data(), &CGitHubPackagesReader::updateInfoAvailable, this,
                    &CApplication::updateInfoAvailable, Qt::QueuedConnection);
            reloadUpdateInfo();

            // startup done
            connect(this, &CApplication::startUpCompleted, this, &CApplication::onStartUpCompleted,
                    Qt::QueuedConnection);
            connect(this, &CApplication::coreFacadeStarted, this, &CApplication::onCoreFacadeStarted,
                    Qt::QueuedConnection);

            if (!this->getApplicationInfo().isUnitTest())
            {
                m_inputManager = new CInputManager(this);
                m_inputManager->createDevices();
            }

            connect(this, &QObject::destroyed, [cat = CLogCategoryList(this)] {
                for (CWorkerBase *worker : CWorkerBase::allWorkers())
                {
                    CLogMessage(cat).debug(u"Worker named '%1' still exists after application destroyed")
                        << worker->objectName();
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
        const bool ok = CFileUtils::writeStringToLockedFile(apps.toJsonString(),
                                                            CFileUtils::appendFilePaths(swiftDataRoot(), "apps.json"));
        if (!ok)
        {
            CLogMessage(static_cast<CApplication *>(nullptr)).error(u"Failed to write to application list file");
        }
        return ok;
    }

    bool CApplication::unregisterAsRunning()
    {
        //! \fixme KB 2017-11 maybe this code can be encapsulated somewhere
        CApplicationInfoList apps = CApplication::getRunningApplications();
        const CApplicationInfo myself = CApplication::instance()->getApplicationInfo();
        if (!apps.contains(myself)) { return true; }
        apps.remove(myself);
        const bool ok = CFileUtils::writeStringToLockedFile(apps.toJsonString(),
                                                            CFileUtils::appendFilePaths(swiftDataRoot(), "apps.json"));
        if (!ok)
        {
            CLogMessage(static_cast<CApplication *>(nullptr)).error(u"Failed to write to application list file");
        }
        return ok;
    }

    int CApplication::exec()
    {
        Q_ASSERT_X(instance(), Q_FUNC_INFO, "missing application");
        emit this->startUpCompleted(true);
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
        CApplication::exit(0);
    }

    CApplication::~CApplication() { this->gracefulShutdown(); }

    CApplicationInfoList CApplication::getRunningApplications()
    {
        CApplicationInfoList apps;
        apps.convertFromJsonNoThrow(CFileUtils::readLockedFileToString(swiftDataRoot() + "apps.json"), {}, {});
        apps.removeIf([](const CApplicationInfo &info) { return !info.getProcessInfo().exists(); });
        return apps;
    }

    bool CApplication::isApplicationRunning(CApplicationInfo::Application application)
    {
        const CApplicationInfoList running = CApplication::getRunningApplications();
        return running.containsApplication(application);
    }

    bool CApplication::isAlreadyRunning() const
    {
        return getRunningApplications().containsBy([this](const CApplicationInfo &info) {
            return info.getApplication() == getApplicationInfo().getApplication();
        });
    }

    bool CApplication::isShuttingDown() const { return m_shutdown || m_shutdownInProgress; }

    bool CApplication::isIncognito() const { return m_incognito; }

    void CApplication::setIncognito(bool incognito) { m_incognito = incognito; }

    void CApplication::toggleIncognito() { m_incognito = !m_incognito; }

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

        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Call this function after parsing");

        // parsing itself is done
        CStatusMessageList msgs;
        do {
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
                QTimer::singleShot(10 * 1000, [=] {
                    if (!sApp || sApp->isShuttingDown()) { return; }
                    this->simulateCrash();
                });
            }

            Q_ASSERT_X(m_setupReader && m_setupReader->isSetupAvailable(), Q_FUNC_INFO, "Setup not available");

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
        else if (!msgs.isEmpty()) { CLogMessage::preformatted(msgs); }

        m_started = true;
        return m_started;
    }

    bool CApplication::isSetupAvailable() const
    {
        if (m_shutdown || !m_setupReader) { return false; }
        return m_setupReader->isSetupAvailable();
    }

    bool CApplication::hasMinimumMappingVersion() const
    {
        return (this->getGlobalSetup().isSwiftVersionMinimumMappingVersion());
    }

    bool CApplication::hasWebDataServices() const
    {
        if (this->isShuttingDown()) { return false; } // service will not survive for long
        return !m_webDataServices.isNull();
    }

    CWebDataServices *CApplication::getWebDataServices() const
    {
        // use hasWebDataServices() to test if services are available
        // getting the assert means web services are accessed before the are initialized

        Q_ASSERT_X(m_webDataServices, Q_FUNC_INFO,
                   "Missing web data services, use hasWebDataServices to test if existing");
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

        return false;
    }

    CStatusMessage CApplication::initLocalSettings()
    {
        if (m_localSettingsLoaded) { return CStatusMessage(); }
        m_localSettingsLoaded = true;

        // trigger loading and saving of settings in appropriate scenarios
        if (m_coreFacadeConfig.getMode() != CCoreFacadeConfig::Remote)
        {
            // facade running here locally
            const CStatusMessage msg = CSettingsCache::instance()->loadFromStore();
            if (msg.isFailure()) { return msg; }

            // Settings are distributed via DBus. So only one application is responsible for saving. `enableLocalSave()`
            // means "this is the application responsible for saving". If swiftgui requests a setting to be saved, it is
            // sent to swiftcore and saved by swiftcore.
            CSettingsCache::instance()->enableLocalSave();
        }
        return CStatusMessage();
    }

    bool CApplication::hasUnsavedSettings() const { return !this->getUnsavedSettingsKeys().isEmpty(); }

    void CApplication::saveSettingsOnShutdown(bool saveSettings) { m_saveSettingsOnShutdown = saveSettings; }

    QStringList CApplication::getUnsavedSettingsKeys() const
    {
        return this->supportsContexts() ? this->getIContextApplication()->getUnsavedSettingsKeys() :
                                          CSettingsCache::instance()->getAllUnsavedKeys();
    }

    CStatusMessage CApplication::saveSettingsByKey(const QStringList &keys)
    {
        if (keys.isEmpty()) { return CStatusMessage(); }
        return this->supportsContexts() ? this->getIContextApplication()->saveSettingsByKey(keys) :
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
        QString str = CBuildConfig::getVersionString() % u" " %
                      (CBuildConfig::isReleaseBuild() ? u"Release build" : u"Debug build") % separator %
                      u"Local dev.dbg.: " % boolToYesNo(CBuildConfig::isLocalDeveloperDebugBuild()) % separator %
                      u"dev.env.: " % boolToYesNo(this->isDeveloperFlagSet()) % separator % u"distribution: " %
                      this->getOwnDistribution().toQString(true) % separator % u"Windows NT: " %
                      boolToYesNo(CBuildConfig::isRunningOnWindowsNtPlatform()) % separator % u"Linux: " %
                      boolToYesNo(CBuildConfig::isRunningOnLinuxPlatform()) % " Unix: " %
                      boolToYesNo(CBuildConfig::isRunningOnUnixPlatform()) % separator % u"MacOS: " %
                      boolToYesNo(CBuildConfig::isRunningOnMacOSPlatform()) % separator % "Build Abi: " %
                      QSysInfo::buildAbi() % separator % u"Build CPU: " % QSysInfo::buildCpuArchitecture() % separator %
                      CBuildConfig::compiledWithInfo();

        if (this->supportsContexts()) { str += (separator % u"Supporting contexts"); }

        return str;
    }

    QNetworkReply *CApplication::getFromNetwork(const CUrl &url, const CApplication::CallbackSlot &callback,
                                                int maxRedirects)
    {
        const CApplication::ProgressSlot progress;
        return this->getFromNetwork(url, callback, progress, maxRedirects);
    }

    QNetworkReply *CApplication::getFromNetwork(const CUrl &url, const CApplication::CallbackSlot &callback,
                                                const CApplication::ProgressSlot &progress, int maxRedirects)
    {
        return this->getFromNetwork(url.toNetworkRequest(), NoLogRequestId, callback, progress, maxRedirects);
    }

    QNetworkReply *CApplication::getFromNetwork(const CUrl &url, int logId, const CApplication::CallbackSlot &callback,
                                                const CApplication::ProgressSlot &progress, int maxRedirects)
    {
        return this->getFromNetwork(url.toNetworkRequest(), logId, callback, progress, maxRedirects);
    }

    QNetworkReply *CApplication::getFromNetwork(const QNetworkRequest &request,
                                                const CApplication::CallbackSlot &callback, int maxRedirects)
    {
        const CApplication::ProgressSlot progress;
        return this->getFromNetwork(request, callback, progress, maxRedirects);
    }

    QNetworkReply *CApplication::getFromNetwork(const QNetworkRequest &request,
                                                const CApplication::CallbackSlot &callback,
                                                const CApplication::ProgressSlot &progress, int maxRedirects)
    {
        return this->getFromNetwork(request, NoLogRequestId, callback, progress, maxRedirects);
    }

    QNetworkReply *CApplication::getFromNetwork(const QNetworkRequest &request, int logId,
                                                const CApplication::CallbackSlot &callback,
                                                const CApplication::ProgressSlot &progress, int maxRedirects)
    {
        return this->httpRequestImpl(request, logId, callback, progress, maxRedirects,
                                     [](QNetworkAccessManager &qam, const QNetworkRequest &request) {
                                         QNetworkReply *nr = qam.get(request);
                                         return nr;
                                     });
    }

    QNetworkReply *CApplication::deleteResourceFromNetwork(const QNetworkRequest &request, int logId,
                                                           const CApplication::CallbackSlot &callback, int maxRedirects)
    {
        const CApplication::ProgressSlot progress;
        return this->httpRequestImpl(request, logId, callback, progress, maxRedirects,
                                     [](QNetworkAccessManager &qam, const QNetworkRequest &request) {
                                         QNetworkReply *nr = qam.deleteResource(request);
                                         return nr;
                                     });
    }

    QNetworkReply *CApplication::postToNetwork(const QNetworkRequest &request, int logId, const QByteArray &data,
                                               const CSlot<void(QNetworkReply *)> &callback)
    {
        return this->httpRequestImpl(request, logId, callback, NoRedirects,
                                     [data](QNetworkAccessManager &qam, const QNetworkRequest &request) {
                                         QNetworkReply *nr = qam.post(request, data);
                                         return nr;
                                     });
    }

    QNetworkReply *CApplication::postToNetwork(const QNetworkRequest &request, int logId, QHttpMultiPart *multiPart,
                                               const CSlot<void(QNetworkReply *)> &callback)
    {
        if (multiPart->thread() != m_accessManager->thread()) { multiPart->moveToThread(m_accessManager->thread()); }

        QPointer<CApplication> myself(this);
        return httpRequestImpl(request, logId, callback, NoRedirects,
                               [=](QNetworkAccessManager &qam, const QNetworkRequest &request) {
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

    QNetworkReply *CApplication::headerFromNetwork(const QNetworkRequest &request, const CallbackSlot &callback,
                                                   int maxRedirects)
    {
        return httpRequestImpl(
            request, NoLogRequestId, callback, maxRedirects,
            [](QNetworkAccessManager &qam, const QNetworkRequest &request) { return qam.head(request); });
    }

    QNetworkReply *CApplication::downloadFromNetwork(const CUrl &url, const QString &saveAsFileName,
                                                     const CSlot<void(const CStatusMessage &)> &callback,
                                                     int maxRedirects)
    {
        // upfront checks
        if (url.isEmpty()) { return nullptr; }
        if (saveAsFileName.isEmpty()) { return nullptr; }
        const QFileInfo fi(saveAsFileName);
        if (!fi.dir().exists()) { return nullptr; }

        // function called with reply when done
        CallbackSlot callbackSlot(this, [=](QNetworkReply *reply) {
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(reply);
            CStatusMessage msg;
            if (reply->error() != QNetworkReply::NoError)
            {
                msg = CStatusMessage(this, CStatusMessage::SeverityError, u"Download for '%1' failed: '%2'")
                      << url.getFullUrl() << nwReply->errorString();
            }
            else
            {
                const bool ok = CFileUtils::writeByteArrayToFile(reply->readAll(), saveAsFileName);
                msg = ok ? CStatusMessage(this, CStatusMessage::SeverityInfo, u"Saved file '%1' downloaded from '%2'")
                               << saveAsFileName << url.getFullUrl() :
                           CStatusMessage(this, CStatusMessage::SeverityError,
                                          u"Saving file '%1' downloaded from '%2' failed")
                               << saveAsFileName << url.getFullUrl();
            }
            nwReply->close();
            QTimer::singleShot(0, callback.object(), [=] {
                if (!sApp || sApp->isShuttingDown()) { return; }
                callback(msg);
            });
        });

        ProgressSlot progressSlot(this, [=](int, qint64, qint64, const QUrl &) {
            // so far not implemented
        });

        QNetworkReply *reply = this->getFromNetwork(url, callbackSlot, progressSlot, maxRedirects);
        return reply;
    }

    void CApplication::deleteAllCookies() { m_cookieManager->deleteAllCookies(); }

    void CApplication::exit(int retcode)
    {
        if (sApp) { instance()->gracefulShutdown(); }

        // when the event loop is not running, this does nothing
        QCoreApplication::exit(retcode);
    }

    QStringList CApplication::arguments() { return QCoreApplication::arguments(); }

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

    CStatusMessageList CApplication::initContextsAndStartCoreFacade(const CCoreFacadeConfig &coreConfig)
    {
        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Call this function after parsing");

        m_coreFacadeConfig = coreConfig;
        const CStatusMessage msg = this->initLocalSettings();
        if (msg.isFailure()) { return msg; }

        // now we can use settings
        // if not yet initialized, init web data services
        if (!m_webDataServices)
        {
            const CStatusMessageList msgs = this->initAndStartWebDataServices(
                CWebReaderFlags::AllReaders, CDatabaseReaderConfigList::forPilotClient());
            if (msgs.hasErrorMessages()) { return msgs; }
        }
        return this->startCoreFacade(); // will do nothing if setup is not yet loaded
    }

    CStatusMessageList CApplication::startCoreFacadeWithoutContexts()
    {
        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Call this function after parsing");

        m_coreFacadeConfig = CCoreFacadeConfig(CCoreFacadeConfig::NotUsed);
        const CStatusMessage msg = this->initLocalSettings();
        if (msg.isFailure()) { return msg; }

        return this->startCoreFacade(); // will do nothing if setup is not yet loaded
    }

    CStatusMessageList CApplication::initAndStartWebDataServices(CWebReaderFlags::WebReader webReader,
                                                                 const db::CDatabaseReaderConfigList &dbReaderConfig)
    {
        Q_ASSERT_X(m_webDataServices.isNull(), Q_FUNC_INFO, "Services already started");
        SWIFT_VERIFY_X(QSslSocket::supportsSsl(), Q_FUNC_INFO, "No SSL");
        if (!QSslSocket::supportsSsl()) { return CStatusMessage(this).error(u"No SSL supported, can`t be used"); }

        return this->startWebDataServices(webReader, dbReaderConfig);
    }

    bool CApplication::isLocalContext() const
    {
        return this->getIContextApplication() && this->getIContextApplication()->isUsingImplementingObject();
    }

    bool CApplication::isDBusContext() const
    {
        return this->getIContextApplication() && !this->getIContextApplication()->isUsingImplementingObject() &&
               !this->getIContextApplication()->isEmptyObject();
    }

    CStatusMessageList CApplication::startCoreFacade()
    {
        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Call this function after parsing");

        if (!m_setupReader || !m_setupReader->isSetupAvailable())
        {
            return CStatusMessage(this).error(u"No setup reader or setup available");
        }

        Q_ASSERT_X(m_coreFacade.isNull(), Q_FUNC_INFO, "Cannot alter facade");
        Q_ASSERT_X(m_setupReader, Q_FUNC_INFO, "No facade without setup possible");
        Q_ASSERT_X(m_webDataServices, Q_FUNC_INFO, "Need running web data services");

        const CStatusMessageList msgs(CStatusMessage(this).info(u"Will start core facade now"));
        m_coreFacade.reset(new CCoreFacade(m_coreFacadeConfig));
        emit this->coreFacadeStarted();
        return msgs;
    }

    CStatusMessageList CApplication::startWebDataServices(CWebReaderFlags::WebReader webReader,
                                                          const db::CDatabaseReaderConfigList &dbReaderConfig)
    {
        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Call this function after parsing");

        if (!m_setupReader || !m_setupReader->isSetupAvailable())
        {
            return CStatusMessage(this).error(u"No setup reader or setup available");
        }

        Q_ASSERT_X(m_setupReader, Q_FUNC_INFO, "No web data services without setup possible");
        CStatusMessageList msgs;
        if (!m_webDataServices)
        {
            msgs.push_back(CStatusMessage(this).info(u"Will start web data services now"));
            m_webDataServices.reset(new CWebDataServices(webReader, dbReaderConfig, this));
            Q_ASSERT_X(m_webDataServices, Q_FUNC_INFO, "Missing web services");

            emit this->webDataServicesStarted(true);
        }
        else { msgs.push_back(CStatusMessage(this).info(u"Web data services already running")); }

        return msgs;
    }

    void CApplication::initLogging()
    {
        CLogHandler::instance()->install(); // make sure we have a log handler!

        // File logger
        m_fileLogger.reset(new CFileLogger(this));
        connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, m_fileLogger.data(),
                &CFileLogger::writeStatusMessageToFile);
        connect(CLogHandler::instance(), &CLogHandler::remoteMessageLogged, m_fileLogger.data(),
                &CFileLogger::writeStatusMessageToFile);
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

        // Skip single application check
        m_cmdSkipSingleApp = QCommandLineOption(
            { "skipsa", "skipsingleapp" }, QCoreApplication::translate("application", "Skip the single app.test."));
        this->addParserOption(m_cmdSkipSingleApp);

        // reset caches upfront
        m_cmdClearCache = QCommandLineOption({ "ccache", "clearcache" },
                                             QCoreApplication::translate("application", "Clear (reset) the caches."));
        this->addParserOption(m_cmdClearCache);

        // test crashpad upload
        m_cmdTestCrashpad = QCommandLineOption(
            { "testcp", "testcrashpad" }, QCoreApplication::translate("application", "Trigger crashpad situation."));
        this->addParserOption(m_cmdTestCrashpad);
    }

    bool CApplication::isSet(const QCommandLineOption &option) const { return (m_parser.isSet(option)); }

    void CApplication::registerMetadata()
    {
        swift::misc::registerMetadata();
        swift::core::registerMetadata();
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

        // save settings (but only when application was really alive)
        if (m_parsed && m_saveSettingsOnShutdown)
        {
            const CStatusMessage m = this->supportsContexts() ? this->getIContextApplication()->saveSettings() :
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

        if (m_gitHubPackagesReader) { m_gitHubPackagesReader.reset(); }

        if (m_setupReader) { m_setupReader.reset(); }

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

    void CApplication::onStartUpCompleted()
    {
        // void
    }

    void CApplication::initNetwork()
    {
        if (!m_accessManager) { m_accessManager = new QNetworkAccessManager(this); }

        m_cookieManager = new CCookieManager(this);
        m_cookieManager->setParent(m_accessManager);
        m_accessManager->setCookieJar(m_cookieManager);

        // Init network
        Q_ASSERT_X(m_accessManager, Q_FUNC_INFO, "Need QAM");
    }

    CApplication *swift::core::CApplication::instance() { return sApp; }

    const QString &CApplication::executable()
    {
        static const QString e(QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName());
        return e;
    }

    const QStringList &CApplication::getLogCategories()
    {
        static const QStringList l({ "swift.application", "swift." % executable() });
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
        m_cmdDBusAddress = QCommandLineOption(
            { "dbus", "dbusaddress" },
            QCoreApplication::translate("application", "DBus address (session, system, P2P IP e.g. 192.168.23.5)"),
            "dbusaddress");
        this->addParserOption(m_cmdDBusAddress);
    }

    void CApplication::addNetworkOptions() { this->addParserOptions(IContextNetwork::getCmdLineOptions()); }

    void CApplication::addAudioOptions() { this->addParserOptions(CContextAudioBase::getCmdLineOptions()); }

    QString CApplication::getCmdDBusAddressValue() const
    {
        if (!this->isParserOptionSet(m_cmdDBusAddress)) { return {}; }
        const QString v(this->getParserValue(m_cmdDBusAddress));
        const QString dBusAddress(CDBusServer::normalizeAddress(v));
        return dBusAddress;
    }

    bool CApplication::isParserOptionSet(const QString &option) const { return m_parser.isSet(option); }

    bool CApplication::skipSingleApplicationCheck() const { return this->isParserOptionSet(m_cmdSkipSingleApp); }

    bool CApplication::isParserOptionSet(const QCommandLineOption &option) const { return m_parser.isSet(option); }

    QString CApplication::getParserValue(const QString &option) const { return m_parser.value(option).trimmed(); }

    QString CApplication::getParserValue(const QCommandLineOption &option) const
    {
        return m_parser.value(option).trimmed();
    }

    bool CApplication::parseCommandLineArgsAndLoadSetup()
    {
        if (!this->startupCheck()) return false;
        if (!this->parseCommandLineArguments()) return false;
        if (!this->loadSetupAndHandleErrors()) return false;
        return true;
    }

    bool CApplication::parseCommandLineArguments()
    {
        if (m_parsed) { return m_parsed; } // already done

        // we call parse because we also want to display a GUI error message when applicable
        const QStringList args(QCoreApplication::arguments());
        if (!m_parser.parse(args))
        {
            this->cmdLineErrorMessage("Parser error:", m_parser.errorText());
            return false;
        }

        if (m_alreadyRunning && !this->skipSingleApplicationCheck())
        {
            this->cmdLineErrorMessage("Program must only run once",
                                      "You cannot run two or more instances side-by-side.");
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
        m_parsed = true;
        return true;
    }

    bool CApplication::startupCheck() const
    {
        const QStringList verifyErrors = CSwiftDirectories::verifyRuntimeDirectoriesAndFiles();
        if (!verifyErrors.isEmpty() && !m_applicationInfo.isUnitTest())
        {
            cmdLineErrorMessage("Missing runtime directories/files:", verifyErrors.join(", "));
            return false;
        }
        return true;
    }

    bool CApplication::loadSetupAndHandleErrors()
    {
        const CStatusMessageList msgs = loadSetup();

        if (msgs.isFailure()) { displaySetupLoadFailure(msgs); }
        return msgs.isSuccess();
    }

    void CApplication::displaySetupLoadFailure(swift::misc::CStatusMessageList)
    {
        // Ignore for CLI application
        // Already logged to console
    }

    void CApplication::cmdLineErrorMessage(const QString &text, const QString &informativeText) const
    {
        fputs(qPrintable(text + informativeText), stderr);
    }

    void CApplication::cmdLineErrorMessage(const CStatusMessageList &msgs) const
    {
        if (msgs.isEmpty()) { return; }
        if (!msgs.hasErrorMessages()) { return; }
        CApplication::cmdLineErrorMessage(msgs.toQString(true), "");
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

    void CApplication::cmdLineVersionMessage()
    {
        m_parser.showVersion(); // terminates
        Q_UNREACHABLE();
    }

    QStringList CApplication::argumentsJoined(const QStringList &newArguments, const QStringList &removeArguments) const
    {
        QStringList joinedArguments = CApplication::arguments();
        QStringList newArgumentsChecked = newArguments;

        // remove the executable argument if it exists at position 0
        if (!joinedArguments.isEmpty() && !joinedArguments.at(0).startsWith("-"))
        {
            joinedArguments.removeFirst();
        } // was cmd line argument
        if (!newArgumentsChecked.isEmpty() && !newArgumentsChecked.at(0).startsWith("-"))
        {
            newArgumentsChecked.removeFirst();
        } // was cmd line argument

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
                if (n >= 0) { argumentsWithoutOption(option, joinedArguments); }
            }
        }

        joinedArguments.append(newArgumentsChecked);
        return joinedArguments;
    }

    // ---------------------------------------------------------------------------------
    // Contexts
    // ---------------------------------------------------------------------------------

    shared_state::CDataLinkDBus *CApplication::getDataLinkDBus() { return getCoreFacade()->getDataLinkDBus(); }

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

    bool CApplication::hasSetupReader() const { return !m_setupReader.isNull(); }

    CSetupReader *CApplication::getSetupReader() const { return m_setupReader.data(); }

    CStatusMessageList CApplication::loadSetup()
    {
        if (m_shutdown) { return CStatusMessage(this).warning(u"Shutting down, not reading"); }
        if (!m_setupReader) { return CStatusMessage(this).error(u"No reader for setup/version"); }
        Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Not yet parsed");
        const CStatusMessageList requestMsgs = m_setupReader->loadSetup();
        return requestMsgs;
    }

    CUrl CApplication::getVatsimMetarUrl() const
    {
        if (m_shutdown) { return {}; }
        if (m_webDataServices)
        {
            const CUrl url(m_webDataServices->getVatsimMetarUrl());
            if (!url.isEmpty()) { return url; }
        }
        if (m_setupReader) { return m_setupReader->getSetup().getVatsimMetarsUrl(); }
        return {};
    }

    CUrl CApplication::getVatsimDataFileUrl() const
    {
        if (m_shutdown) { return {}; }
        if (m_webDataServices)
        {
            const CUrl url(m_webDataServices->getVatsimDataFileUrl());
            if (!url.isEmpty()) { return url; }
        }
        if (m_setupReader) { return m_setupReader->getSetup().getVatsimDataFileUrl(); }
        return {};
    }

    CUrl CApplication::getVatsimServerFileUrl() const
    {
        if (m_shutdown || !m_setupReader) { return {}; }

        return m_setupReader->getSetup().getVatsimServerFileUrl();
    }

    CUrl CApplication::getVatsimFsdHttpUrl() const
    {
        if (m_shutdown || !m_setupReader) { return {}; }

        return m_setupReader->getSetup().getVatsimFsdHttpUrl();
    }

    void CApplication::onCrashDumpUploadEnabledChanged()
    {
        const bool enabled = CBuildConfig::isReleaseBuild() && m_crashDumpUploadEnabled.getThreadLocal();
        this->enableCrashDumpUpload(enabled);
    }

    void CApplication::simulateCrash() { CCrashHandler::instance()->simulateCrash(); }

    void CApplication::simulateAssert() { CCrashHandler::instance()->simulateAssert(); }

    void CApplication::enableCrashDumpUpload(bool enable) { CCrashHandler::instance()->setUploadsEnabled(enable); }

    bool CApplication::isSupportingCrashpad() const
    {
#ifdef SWIFT_USE_CRASHPAD
        return true;
#else
        return false;
#endif
    }

    void CApplication::httpRequestImplInQAMThread(const QNetworkRequest &request, int logId,
                                                  const CallbackSlot &callback, const ProgressSlot &progress,
                                                  int maxRedirects, NetworkRequestOrPostFunction getPostOrDeleteRequest)
    {
        // run in QAM thread
        if (this->isShuttingDown()) { return; }
        QTimer::singleShot(0, m_accessManager, [=] {
            // should be now in QAM thread
            if (!sApp || sApp->isShuttingDown()) { return; }
            Q_ASSERT_X(CThreadUtils::isInThisThread(sApp->m_accessManager), Q_FUNC_INFO,
                       "Wrong thread, must be QAM thread");
            this->httpRequestImpl(request, logId, callback, progress, maxRedirects, getPostOrDeleteRequest);
        });
    }

    QNetworkReply *CApplication::httpRequestImpl(const QNetworkRequest &request, int logId,
                                                 const CApplication::CallbackSlot &callback, int maxRedirects,
                                                 NetworkRequestOrPostFunction requestOrPostMethod)
    {
        ProgressSlot progress;
        return this->httpRequestImpl(request, logId, callback, progress, maxRedirects, requestOrPostMethod);
    }

    QNetworkReply *CApplication::httpRequestImpl(const QNetworkRequest &request, int logId,
                                                 const CallbackSlot &callback, const ProgressSlot &progress,
                                                 int maxRedirects, NetworkRequestOrPostFunction getPostOrDeleteRequest)
    {
        if (this->isShuttingDown()) { return nullptr; }

        QWriteLocker locker(&m_accessManagerLock);
        Q_ASSERT_X(m_accessManager->thread() == qApp->thread(), Q_FUNC_INFO,
                   "Network manager supposed to be in main thread");
        if (!CThreadUtils::isInThisThread(m_accessManager))
        {
            this->httpRequestImplInQAMThread(request, logId, callback, progress, maxRedirects, getPostOrDeleteRequest);
            return nullptr; // not yet started, will be called again in QAM thread
        }

        Q_ASSERT_X(CThreadUtils::isInThisThread(m_accessManager), Q_FUNC_INFO, "Network manager thread mismatch");
        QNetworkRequest copiedRequest =
            CNetworkUtils::getSwiftNetworkRequest(request, this->getApplicationNameAndVersion());

        QNetworkReply *reply = getPostOrDeleteRequest(*m_accessManager, copiedRequest);
        reply->setProperty("started", QVariant(QDateTime::currentMSecsSinceEpoch()));
        reply->setProperty(CUrlLog::propertyNameId(), QVariant(logId));
        const QUrl url(reply->url());
        QString urlStr = url.toString();

        if (progress)
        {
            connect(reply, &QNetworkReply::downloadProgress, progress.object(),
                    [=](qint64 current, qint64 max) { progress(logId, current, max, url); });
        }

        if (callback)
        {
            Q_ASSERT_X(callback.object(), Q_FUNC_INFO, "Need callback object (to determine thread)");
            connect(
                reply, &QNetworkReply::finished, callback.object(),
                [=] {
                    // Called when finished!
                    // QNetworkRequest::FollowRedirectsAttribute would allow auto redirect, but we use our approach as
                    // it gives us better control \fixme: Check again on Qt 5.9: Added redirects policy to
                    // QNetworkAccessManager (ManualRedirectsPolicy, NoLessSafeRedirectsPolicy,
                    // SameOriginRedirectsPolicy, UserVerifiedRedirectsPolicy)
                    const bool isRedirect = CNetworkUtils::isHttpStatusRedirect(reply);
                    if (isRedirect && maxRedirects > 0)
                    {
                        const QUrl redirectUrl = CNetworkUtils::getHttpRedirectUrl(reply);
                        if (!redirectUrl.isEmpty())
                        {
                            QNetworkRequest redirectRequest(redirectUrl);
                            const int redirectsLeft = maxRedirects - 1;
                            CLogMessage(sApp).info(u"Redirecting '%1' to '%2'") << urlStr << redirectUrl.toString();
                            this->httpRequestImplInQAMThread(redirectRequest, logId, callback, progress, redirectsLeft,
                                                             getPostOrDeleteRequest);
                            return;
                        }
                    }
                    // called when there are no more callbacks
                    callback(reply);
                },
                Qt::QueuedConnection); // called in callback thread
        }
        return reply;
    }

    void CApplication::tagApplicationDataDirectory()
    {
        const QString d = CSwiftDirectories::normalizedApplicationDataDirectory();
        const QDir dir(d);
        if (!dir.exists() || !dir.isReadable()) { return; }
        const QString aiStr(this->getApplicationInfo().toJsonString());
        const QString filePath(CFileUtils::appendFilePaths(
            dir.path(), CApplicationInfo::fileName())); // will be overridden by next swift app
        CFileUtils::writeStringToFile(aiStr, filePath);
    }
} // namespace swift::core
