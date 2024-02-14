// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_APPLICATION_H
#define BLACKCORE_APPLICATION_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/db/databasereaderconfig.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/githubpackagesreader.h"
#include "blackcore/application/applicationsettings.h"
#include "blackcore/inputmanager.h"
#include "blackcore/webreaderflags.h"
#include "blackmisc/db/updateinfo.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/slot.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/applicationinfolist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/crashinfo.h"

#include <QByteArray>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QReadWriteLock>
#include <QStringList>
#include <QString>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QByteArray>
#include <atomic>
#include <functional>

class QHttpMultiPart;
class QNetworkReply;
class QNetworkRequest;

namespace BlackMisc
{
    class CFileLogger;
    class CLogCategoryList;
    namespace SharedState
    {
        class CDataLinkDBus;
    }
}

namespace BlackCore
{
    class CCoreFacade;
    class CCookieManager;
    class CSetupReader;
    class CWebDataServices;
    class ISimulator;
    namespace Context
    {
        class IContextApplication;
        class IContextAudio;
        class CContextAudioBase;
        class IContextNetwork;
        class IContextOwnAircraft;
        class IContextSimulator;
    }
    namespace Db
    {
        class CNetworkWatchdog;
    }

    /*!
     * Our runtime. Normally one instance is to be initialized at the beginning of main, and thereafter
     * it can be used everywhere via QApplication::instance
     *
     * - A swift standard cmd line parser is part of the application.
     *   Hence cmd arguments can be optained any time / everywhere when required.
     *   Also some standard swift cmd arguments do not need to be re-implemented for each swift application.
     * - The core facade (aka core runtime) is now part of the application. It can be started via cmd line arguments.
     * - Settings are loaded
     * - Setup is loaded (load the so called bootstrap file) to find servers and other resources
     * - Update information (new swift versions etc.) are loaded
     * - If applicable VATSIM status data (where are the VATSIM files?) are loaded
     * - An end of lifetime can be specified, aka time bombing
     *
     * \sa BlackGui::CGuiApplication for the GUI version of application
     */
    class BLACKCORE_EXPORT CApplication :
        public QObject,
        public BlackMisc::CIdentifiable
    {
        Q_OBJECT

    public:
        //! Similar to \sa QCoreApplication::instance() returns the single instance
        static CApplication *instance();

        //! Own log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CApplication(BlackMisc::CApplicationInfo::Application application, bool init = true);

        //! Constructor
        CApplication(const QString &applicationName = executable(), BlackMisc::CApplicationInfo::Application application = BlackMisc::CApplicationInfo::Unknown, bool init = true);

        //! Destructor
        virtual ~CApplication() override;

        //! Information about all running apps (including this one only if exec() has already been called)
        static BlackMisc::CApplicationInfoList getRunningApplications();

        //! Is application running?
        static bool isApplicationRunning(BlackMisc::CApplicationInfo::Application application);

        //! True if this swift application is already running (including different versions)
        bool isAlreadyRunning() const;

        //! Graceful shutdown
        virtual void gracefulShutdown();

        //! Is application shutting down?
        //! \threadsafe
        bool isShuttingDown() const;

        //! Is incognito mode?
        //! \threadsafe
        bool isIncognito() const;

        //! Set incognito mode
        //! \threadsafe
        void setIncognito(bool incognito);

        //! Toggle incognito mode
        //! \threadsafe
        void toggleIncognito();

        //! swift application running
        const BlackMisc::CApplicationInfo &getApplicationInfo() const { return m_applicationInfo; }

        //! Application name and version
        const QString &getApplicationName() const { return m_applicationName; }

        //! Application name and version
        const QString &getApplicationNameAndVersion() const;

        //! Version, name beta and dev info
        const QString &getApplicationNameVersionDetailed() const;

        //! Force single application (only one instance)
        void setSingleApplication(bool singleApplication);

        //! Executable names for the given applications
        QString getExecutableForApplication(BlackMisc::CApplicationInfo::Application application) const;

        //! Start the launcher
        bool startLauncher();

        //! Start the launcher and quit
        bool startLauncherAndQuit();

        //! Global setup
        //! \threadsafe
        Data::CGlobalSetup getGlobalSetup() const;

        //! Update info
        BlackMisc::Db::CUpdateInfo getUpdateInfo() const;

        //! Reload update info
        void reloadUpdateInfo();

        //! Own distribution
        //! \threadsafe
        BlackMisc::Db::CDistribution getOwnDistribution() const;

        //! String with beta, dev. and version
        const QString &versionStringDetailed() const;

        //! swift info string
        const QString &swiftVersionString() const;

        //! swift info string
        const char *swiftVersionChar();

        //! Running with dev.flag?
        bool isDeveloperFlagSet() const { return m_devFlag; }

        //! Comprehensive info
        QString getInfoString(const QString &separator) const;

        //! Stop and restart application
        void restartApplication(const QStringList &newArguments = {}, const QStringList &removeArguments = {});

        //! Finishes initialization and executes the event loop
        int exec();

        //! Directory for temporary files
        static QString getTemporaryDirectory();

        //! Register as running
        //! \note Normally done automatically when CApplication::exec is called
        static bool registerAsRunning();

        //! Unregister from running
        //! \note Normally done automatically, needed for restart
        static bool unregisterAsRunning();

        //! Exit application, perform graceful shutdown and exit
        static void exit(int retcode = EXIT_SUCCESS);

        //! Process all events for some time
        //! \remark unlike QCoreApplication::processEvents this will spend at least the given time in the function, using QThread::msleep
        //! \remark using processEventsFor can lead to undesired behaviour: A function may be called again before it is finished, even with only one thread
        //! \sa BlackMisc::CEventLoop
        static void processEventsFor(int milliseconds);

        //! Clear the caches
        //! \return all cache files
        static QStringList clearCaches();

        // ----------------------- settings -------------------------------

        //! Unsaved settings
        bool hasUnsavedSettings() const;

        //! Save settings on shutdown
        void saveSettingsOnShutdown(bool saveSettings);

        //! All unsaved settings
        QStringList getUnsavedSettingsKeys() const;

        //! Save all settings
        BlackMisc::CStatusMessage saveSettingsByKey(const QStringList &keys);

        // ----------------------- cmd line args / parsing ----------------------------------------

        //! Current parameters replaced by new arguments without the cmd line argument
        virtual QStringList argumentsJoined(const QStringList &newArguments = {}, const QStringList &removeArguments = {}) const;

        //! Similar to QCoreApplication::arguments
        static QStringList arguments();

        //! \name cmd line args and parsing of command line options
        //! @{

        //! \copydoc QCommandLineParser::addOption
        bool addParserOption(const QCommandLineOption &option);

        //! \copydoc QCommandLineParser::addOptions
        bool addParserOptions(const QList<QCommandLineOption> &options);

        //! CMD line argument for DBus address
        void addDBusAddressOption();

        //! DBus address from CMD line, otherwise ""
        QString getCmdDBusAddressValue() const;

        //! Add the VATLIB options
        void addVatlibOptions();

        //! Add the audio options
        void addAudioOptions();

        //! Private resource dir for developer's own resource files
        QString getCmdSwiftPrivateSharedDir() const;

        //! Called by installer?
        bool isInstallerOptionSet() const;

        //! Skip the single application check
        bool skipSingleApplicationCheck() const;

        //! Delegates to QCommandLineParser::isSet
        bool isParserOptionSet(const QString &option) const;

        //! Delegates to QCommandLineParser::isSet
        bool isParserOptionSet(const QCommandLineOption &option) const;

        //! Delegates to QCommandLineParser::value
        QString getParserValue(const QString &option) const;

        //! Delegates to QCommandLineParser::value
        QString getParserValue(const QCommandLineOption &option) const;

        //! Combined function that does a startup check, parses the command line arguments and loads the setup
        //! \see startupCheck
        //! \see parseCommandLineArguments
        //! \see loadSetupAndHandleErrors
        //! \remark This function cannot automatically called from the constructor because (1) it calls virtual
        //! functions to show error messages and (2) the some command line arguments are added after constructing the object
        bool parseCommandLineArgsAndLoadSetup();

        //! Display warning message
        virtual bool cmdLineWarningMessage(const QString &text, const QString &informativeText = "") const;

        //! Display error message
        virtual bool cmdLineErrorMessage(const QString &text, const QString &informativeText = "", bool retry = false) const;

        //! Display error message
        virtual bool cmdLineErrorMessage(const BlackMisc::CStatusMessageList &msgs, bool retry = false) const;

        //! cmd line arguments as string
        virtual QString cmdLineArgumentsAsString(bool withExecutable = true);

        //! @}

        //! Simulate a crash
        //! \private only for testing purposes
        void simulateCrash();

        //! Simulate an ASSERT
        //! \private only for testing purposes
        void simulateAssert();

        //! Enable crash upload
        //! \remark only change for testing
        void enableCrashDumpUpload(bool enable);

        //! Has crashpad support?
        bool isSupportingCrashpad() const;

        // ----------------------- Input ----------------------------------------

        //! The input manager, if available
        CInputManager *getInputManager() const { return m_inputManager; }

        // ----------------------- simulator ----------------------------------------

        //! The simulator plugin, if available
        QPointer<ISimulator> getISimulator() const;

        //! Simulator object available?
        bool hasSimulator() const;

        // ----------------------- contexts ----------------------------------------

        //! Transport mechanism for sharing state between applications
        BlackMisc::SharedState::CDataLinkDBus *getDataLinkDBus();

        //! \name Context / core facade related
        //! @{

        //! Supports contexts
        //! \remark checks the real availability of the contexts, so it can happen that we want to use contexts, and they are not yet initialized (false here)
        //! \sa m_useContexts we use or we will use contexts
        bool supportsContexts(bool ignoreShutdownTest = false) const;

        //! The core facade config
        const CCoreFacadeConfig &getCoreFacadeConfig() const { return m_coreFacadeConfig; }

        //! Init the contexts part and start core facade
        //! \sa coreFacadeStarted
        //! \remark requires setup loaded
        BlackMisc::CStatusMessageList useContexts(const CCoreFacadeConfig &coreConfig);

        //! Init the core facade without any contexts
        //! \sa coreFacadeStarted
        //! \remark requires setup loaded
        BlackMisc::CStatusMessageList useFacadeNoContexts();

        //! Init web data services and start them
        //! \sa webDataServicesStarted
        //! \remark requires setup loaded
        BlackMisc::CStatusMessageList useWebDataServices(const CWebReaderFlags::WebReader webReader, const Db::CDatabaseReaderConfigList &dbReaderConfig);

        //! Get the facade
        CCoreFacade *getCoreFacade() { return m_coreFacade.data(); }

        //! Local application? (not DBus)
        bool isLocalContext() const;

        //! DBus application? (not Local)
        bool isDBusContext() const;

        //! Get the facade
        const CCoreFacade *getCoreFacade() const { return m_coreFacade.data(); }

        //! @}

        //! Direct access to contexts if a CCoreFacade has been initialized
        //! @{
        const Context::IContextNetwork *getIContextNetwork() const;
        const Context::IContextAudio *getIContextAudio() const;
        const Context::CContextAudioBase *getCContextAudioBase() const;
        const Context::IContextApplication *getIContextApplication() const;
        const Context::IContextOwnAircraft *getIContextOwnAircraft() const;
        const Context::IContextSimulator *getIContextSimulator() const;
        Context::IContextNetwork *getIContextNetwork();
        Context::IContextAudio *getIContextAudio();
        Context::CContextAudioBase *getCContextAudioBase();
        Context::IContextApplication *getIContextApplication();
        Context::IContextOwnAircraft *getIContextOwnAircraft();
        Context::IContextSimulator *getIContextSimulator();
        //! @}

        // ----------------------- setup data ---------------------------------

        //! Minimum mapping version check
        virtual bool hasMinimumMappingVersion() const;

        //! Setup reader?
        bool hasSetupReader() const;

        //! Access to setup reader
        //! \remark supposed to be used only in special cases
        CSetupReader *getSetupReader() const;

        //! Setup already synchronized
        bool isSetupAvailable() const;

        //! Consolidated version of METAR URLs, either from CGlobalSetup or CVatsimSetup
        //! \threadsafe
        BlackMisc::Network::CUrlList getVatsimMetarUrls() const;

        //! Consolidated version of data file URLs, either from CGlobalSetup or CVatsimSetup
        //! \threadsafe
        BlackMisc::Network::CUrlList getVatsimDataFileUrls() const;

        //! Get URL to file which contains the list of VATSIM servers
        BlackMisc::Network::CUrl getVatsimServerFileUrl() const;

        //! Get VATSIM FSD HTTP URL
        BlackMisc::Network::CUrl getVatsimFsdHttpUrl() const;

        //! Start services, if not yet parsed call CApplication::parse
        virtual bool start();

        // ------------------------- network -----------------------------------------------

        //! \name network
        //! @{

        static constexpr int NoRedirects = -1; //!< network request not allowing redirects
        static constexpr int NoLogRequestId = -1; //!< network request without logging
        static constexpr int DefaultMaxRedirects = 2; //!< network request, default for max.redirects

        //! The network reply callback when request is completed
        using CallbackSlot = BlackMisc::CSlot<void(QNetworkReply *)>;

        //! The progress slot
        using ProgressSlot = BlackMisc::CSlot<void(int, qint64, qint64, const QUrl &)>;

        //! Delete all cookies from cookie manager
        void deleteAllCookies();

        //! Get the watchdog
        //! \private mostly for UNIT tests etc, normally not meant to be used directly
        Db::CNetworkWatchdog *getNetworkWatchdog() const;

        //! Allows to mark the DB as "up" or "down"
        //! \see BlackCore::Db::CNetworkWatchdog::setDbAccessibility
        void setSwiftDbAccessibility(bool accessible);

        //! \copydoc BlackCore::Db::CNetworkWatchdog::triggerCheck
        int triggerNetworkWatchdogChecks();

        //! Is network accessible
        bool isNetworkAccessible() const;

        //! \copydoc BlackCore::Db::CNetworkWatchdog::isInternetAccessible
        bool isInternetAccessible() const;

        //! \copydoc BlackCore::Db::CNetworkWatchdog::isSwiftDbAccessible
        bool isSwiftDbAccessible() const;

        //! \copydoc BlackCore::Db::CNetworkWatchdog::hasWorkingSharedUrl
        bool hasWorkingSharedUrl() const;

        //! \copydoc BlackCore::Db::CNetworkWatchdog::getWorkingSharedUrl
        BlackMisc::Network::CUrl getWorkingSharedUrl() const;

        //! Access to access manager
        //! \remark supposed to be used only in special cases
        const QNetworkAccessManager *getNetworkAccessManager() const { return m_accessManager; }

        //! Access to access manager
        //! \remark supposed to be used only in special cases
        QNetworkAccessManager *getNetworkAccessManager() { return m_accessManager; }

        //! Access to configuration manager
        //! \remark supposed to be used only in special cases
        const QNetworkConfigurationManager *getNetworkConfigurationManager() const { return m_networkConfigManager; }

        //! Web data services available?
        //! \threadsafe
        bool hasWebDataServices() const;

        //! Get the web data services
        //! \remark use hasWebDataServices to test if services are available
        CWebDataServices *getWebDataServices() const;

        //! Request to get network reply
        //! \threadsafe
        QNetworkReply *getFromNetwork(const BlackMisc::Network::CUrl &url,
                                      const CallbackSlot &callback, int maxRedirects = DefaultMaxRedirects);

        //! Request to get network reply
        //! \threadsafe
        QNetworkReply *getFromNetwork(const BlackMisc::Network::CUrl &url,
                                      const CallbackSlot &callback, const ProgressSlot &progress, int maxRedirects = DefaultMaxRedirects);

        //! Request to get network reply, supporting BlackMisc::Network::CUrlLog
        //! \threadsafe
        QNetworkReply *getFromNetwork(const BlackMisc::Network::CUrl &url, int logId,
                                      const CallbackSlot &callback, const ProgressSlot &progress, int maxRedirects = DefaultMaxRedirects);

        //! Request to get network reply
        //! \threadsafe
        QNetworkReply *getFromNetwork(const QNetworkRequest &request, const CallbackSlot &callback, int maxRedirects = DefaultMaxRedirects);

        //! Request to get network reply
        //! \threadsafe
        QNetworkReply *getFromNetwork(const QNetworkRequest &request,
                                      const CallbackSlot &callback, const ProgressSlot &progress, int maxRedirects = DefaultMaxRedirects);

        //! Request to get network reply, supporting BlackMisc::Network::CUrlLog
        //! \threadsafe
        QNetworkReply *getFromNetwork(const QNetworkRequest &request, int logId,
                                      const CallbackSlot &callback, const ProgressSlot &progress, int maxRedirects = DefaultMaxRedirects);

        //! Request to delete a network ressource from network, supporting BlackMisc::Network::CUrlLog
        //! \threadsafe
        QNetworkReply *deleteResourceFromNetwork(const QNetworkRequest &request, int logId,
                                                 const CallbackSlot &callback,
                                                 int maxRedirects = DefaultMaxRedirects);

        //! Post to network
        //! \threadsafe
        QNetworkReply *postToNetwork(const QNetworkRequest &request, int logId, const QByteArray &data, const CallbackSlot &callback);

        //! Post to network
        //! \note This method takes ownership over \c multiPart.
        //! \threadsafe
        QNetworkReply *postToNetwork(const QNetworkRequest &request, int logId, QHttpMultiPart *multiPart, const CallbackSlot &callback);

        //! Request to get network repy using HTTP's HEADER method
        //! \threadsafe
        QNetworkReply *headerFromNetwork(const BlackMisc::Network::CUrl &url, const CallbackSlot &callback, int maxRedirects = NoRedirects);

        //! Request to get network repy using HTTP's HEADER method
        //! \threadsafe
        QNetworkReply *headerFromNetwork(const QNetworkRequest &request, const CallbackSlot &callback, int maxRedirects = NoRedirects);

        //! Download file from network and store it as passed
        //! \threadsafe
        QNetworkReply *downloadFromNetwork(const BlackMisc::Network::CUrl &url, const QString &saveAsFileName,
                                           const BlackMisc::CSlot<void(const BlackMisc::CStatusMessage &)> &callback, int maxRedirects = DefaultMaxRedirects);
        //! @}

    signals:
        //! Update info available (cache, web load)
        void updateInfoAvailable(bool success);

        //! Startup has been completed
        //! Will be triggered shortly before starting the event loop
        void startUpCompleted(bool success);

        //! Facade started
        void coreFacadeStarted();

        //! Web data services started
        void webDataServicesStarted(bool success);

        //! Internet accessibility changed
        void changedInternetAccessibility(bool accessible);

        //! DB accessibility changed
        void changedSwiftDbAccessibility(bool accessible, const BlackMisc::Network::CUrl &testedUrl);

        //! About to shutdown
        void aboutToShutdown();

    protected:
        //! Display the failures caused by loading the setup file
        virtual void displaySetupLoadFailure(BlackMisc::CStatusMessageList msgs);

        //! Setup read/synchronized
        void onSetupHandlingCompleted(bool available);

        //! Startup completed
        virtual void onStartUpCompleted();

        //! Init class, allows to init from BlackGui::CGuiApplication as well (pseudo virtual)
        void init(bool withMetadata);

        //! Is the command line option represented in the given arguments?
        static int indexOfCommandLineOption(const QCommandLineOption &option, const QStringList &args = CApplication::arguments());

        //! Arguments without that given option
        static void argumentsWithoutOption(const QCommandLineOption &option, QStringList &args);

        //! Can be used to parse specialized arguments
        virtual bool parsingHookIn() { return true; }

        //! Called when facade/contexts have been started
        virtual void onCoreFacadeStarted();

        //! Can be used to start special services
        virtual BlackMisc::CStatusMessageList startHookIn() { return BlackMisc::CStatusMessageList(); }

        //! Flag set or explicitly set to true
        bool isSet(const QCommandLineOption &option) const;

        //! Severe issue during startup, most likely it does not make sense to continue
        //! \note call this here if the parsing stage is over and reaction to a runtime issue is needed
        [[noreturn]] void severeStartupProblem(const BlackMisc::CStatusMessage &message);

        //! Start the core facade
        //! \note does nothing when setup is not yet loaded
        BlackMisc::CStatusMessageList startCoreFacadeAndWebDataServices();

        //! Start the web data services
        //! \note does nothing when setup is not yet loaded
        BlackMisc::CStatusMessageList startWebDataServices();

        //! executable name
        static const QString &executable();

        //! Register metadata
        static void registerMetadata();

        // cmd parsing
        QList<QCommandLineOption> m_allOptions; //!< All registered options
        QCommandLineParser m_parser; //!< cmd parser
        QCommandLineOption m_cmdHelp { "help" }; //!< help option
        QCommandLineOption m_cmdVersion { "version" }; //!< version option
        QCommandLineOption m_cmdDBusAddress { "emptyDBus" }; //!< DBus address
        QCommandLineOption m_cmdDevelopment { "dev" }; //!< Development flag
        QCommandLineOption m_cmdSharedDir { "shared" }; //!< Shared directory
        QCommandLineOption m_cmdClearCache { "clearcache" }; //!< Clear cache
        QCommandLineOption m_cmdTestCrashpad { "testcrashpad" }; //!< Test a crasphpad upload
        QCommandLineOption m_cmdSkipSingleApp { "skipsa" }; //!< Skip test for single application
        bool m_parsed = false; //!< Parsing accomplished?
        bool m_started = false; //!< Started with success?
        bool m_singleApplication = true; //!< Only one instance of that application
        bool m_alreadyRunning = false; //!< Application already running
        std::atomic_bool m_shutdown { false }; //!< Is being shutdown?
        std::atomic_bool m_incognito { false }; //!< Incognito mode?
        std::atomic_bool m_shutdownInProgress { false }; //!< shutdown in progress?

    private:
        //! Read the setup
        BlackMisc::CStatusMessageList loadSetup();

        //! Display help message
        void cmdLineHelpMessage();

        //! Display version message
        void cmdLineVersionMessage();

        //! Problem with network access manager
        void onChangedNetworkAccessibility(QNetworkAccessManager::NetworkAccessibility accessible);

        //! Changed internet accessibility
        void onChangedInternetAccessibility(bool accessible);

        //! Changed swift DB accessibility
        void onChangedSwiftDbAccessibility(bool accessible, const BlackMisc::Network::CUrl &url);

        //! Network configurations update completed
        void onNetworkConfigurationsUpdateCompleted();

        //! Init network
        void initNetwork();

        //! init logging system
        void initLogging();

        //! Init parser
        void initParser();

        //! Dev.environment, return value will be used for m_devEnv
        bool initIsRunningInDeveloperEnvironment() const;

        //! Init the local settings
        BlackMisc::CStatusMessage initLocalSettings();

        //! Async. start when setup is loaded
        BlackMisc::CStatusMessageList asyncWebAndContextStart();

        using NetworkRequestOrPostFunction = std::function<QNetworkReply *(QNetworkAccessManager &, const QNetworkRequest &)>;

        //! Implementation for getFromNetwork(), postToNetwork() and headerFromNetwork()
        //! \return QNetworkReply reply will only be returned, if the QNetworkAccessManager is in the same thread
        QNetworkReply *httpRequestImpl(const QNetworkRequest &request,
                                       int logId, const CallbackSlot &callback,
                                       int maxRedirects, NetworkRequestOrPostFunction requestOrPostMethod);

        //! Implementation for getFromNetwork(), postToNetwork() and headerFromNetwork()
        //! \return QNetworkReply reply will only be returned, if the QNetworkAccessManager is in the same thread
        QNetworkReply *httpRequestImpl(const QNetworkRequest &request,
                                       int logId, const CallbackSlot &callback, const ProgressSlot &progress,
                                       int maxRedirects, NetworkRequestOrPostFunction getPostOrDeleteRequest);

        //! Call httpRequestImpl in correct thread
        void httpRequestImplInQAMThread(const QNetworkRequest &request,
                                        int logId, const CallbackSlot &callback, const ProgressSlot &progress,
                                        int maxRedirects, NetworkRequestOrPostFunction getPostOrDeleteRequest);

        //! Triggers a check of the network accessibility
        //! \remark this is a check that will double check that the watchdog will receive the correct QNetworkAccessManager::NetworkAccessibility
        void triggerNetworkAccessibilityCheck(int deferredMs);

        //! Write meta information into the application directory so other swift versions can display them
        void tagApplicationDataDirectory();

        //! Check if all required runtime files are in place
        //! \returns true if the check succeeded
        bool startupCheck() const;

        //! Loads the setup (bootstrap) and handles/displays warnings and error messages
        //! \returns true if loading succedded without errors
        bool loadSetupAndHandleErrors();

        //! Parses and handles the standard options such as help, version, parse error
        //! \note in some cases (error, version, help) application is terminated during this step
        //! \sa parsingHookIn
        //! \return true means to continue, false to stop
        bool parseCommandLineArguments();

        CInputManager *m_inputManager = nullptr; //!< Input devices and hotkeys
        QNetworkConfigurationManager *m_networkConfigManager = nullptr; //!< configuration
        QNetworkAccessManager *m_accessManager = nullptr; //!< single network access manager
        Db::CNetworkWatchdog *m_networkWatchDog = nullptr; //!< checking DB/internet access
        BlackMisc::CApplicationInfo m_applicationInfo; //!< Application if specified
        QScopedPointer<CCoreFacade> m_coreFacade; //!< core facade if any
        QScopedPointer<CSetupReader> m_setupReader; //!< setup reader
        QScopedPointer<CGitHubPackagesReader> m_gitHubPackagesReader; //!< github packages reader
        QScopedPointer<CWebDataServices> m_webDataServices; //!< web data services
        QScopedPointer<BlackMisc::CFileLogger> m_fileLogger; //!< file logger
        QPointer<CCookieManager> m_cookieManager; //!< single cookie manager for our access manager
        const QString m_applicationName; //!< application name
        QReadWriteLock m_accessManagerLock; //!< lock to make access manager access threadsafe
        CCoreFacadeConfig m_coreFacadeConfig; //!< Core facade config if any
        CWebReaderFlags::WebReader m_webReadersUsed; //!< Readers to be used
        Db::CDatabaseReaderConfigList m_dbReaderConfig; //!< Load or used caching?
        bool m_noNwAccessPoint = false; //!< no network access point?
        bool m_useContexts = false; //!< use contexts
        bool m_useWebData = false; //!< use web data
        bool m_devFlag = false; //!< dev. environment
        bool m_saveSettingsOnShutdown = true; //!< saving all settings on shutdown
        bool m_localSettingsLoaded = false; //!< local settings loaded?

        // -------------- crashpad -----------------
        BlackMisc::CSettingReadOnly<Application::TCrashDumpSettings> m_crashDumpSettings { this, &CApplication::onCrashDumpUploadEnabledChanged };

        //! Upload settings changed
        void onCrashDumpUploadEnabledChanged();
    };
} // namespace

//! Single instance of application object
extern BLACKCORE_EXPORT BlackCore::CApplication *sApp;

#endif // guard
