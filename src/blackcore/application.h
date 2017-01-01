/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_APPLICATION_H
#define BLACKCORE_APPLICATION_H

#include <QByteArray>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QList>
#include <QNetworkAccessManager>
#include <QObject>
#include <QReadWriteLock>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <atomic>
#include <functional>

#include "blackcore/blackcoreexport.h"
#include "blackcore/cookiemanager.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/data/updateinfo.h"
#include "blackcore/db/databasereaderconfig.h"
#include "blackcore/application/applicationsettings.h"
#include "blackcore/webreaderflags.h"
#include "blackmisc/applicationinfolist.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/slot.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"

#if defined(Q_CC_MSVC) || defined(Q_OS_OSX) // Crashpad only supported on MSVC and MacOS/X
#define BLACK_USE_CRASHPAD
#endif

class QHttpMultiPart;
class QNetworkReply;
class QNetworkRequest;

namespace BlackMisc
{
    class CFileLogger;
    class CLogCategoryList;
}

namespace crashpad
{
    class CrashpadClient;
    class CrashReportDatabase;
}

namespace BlackCore
{
    class CCoreFacade;
    class CSetupReader;
    class CWebDataServices;
    namespace Context
    {
        class IContextApplication;
        class IContextAudio;
        class IContextNetwork;
        class IContextOwnAircraft;
        class IContextSimulator;
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
    class BLACKCORE_EXPORT CApplication : public QObject
    {
        Q_OBJECT

    public:
        //! Similar to \sa QCoreApplication::instance() returns the single instance
        static CApplication *instance();

        //! Own log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Constructor
        CApplication(const QString &applicationName = executable(), BlackMisc::CApplicationInfo::Application application = BlackMisc::CApplicationInfo::Unknown, bool init = true);

        //! Destructor
        virtual ~CApplication();

        //! Application information
        BlackMisc::CApplicationInfo getApplicationInfo() const;

        //! Information about all running apps (including this one only if exec() has already been called)
        static BlackMisc::CApplicationInfoList getRunningApplications();

        //! True if this swift application is already running (including different versions)
        bool isAlreadyRunning() const;

        //! Application name and version
        const QString &getApplicationNameAndVersion() const;

        //! Version, name beta and dev info
        const QString &getApplicationNameVersionBetaDev() const;

        //! swift application running
        BlackMisc::CApplicationInfo::Application getSwiftApplication() const;

        //! Unit test?
        bool isUnitTest() const;

        //! Global setup
        //! \threadsafe
        BlackCore::Data::CGlobalSetup getGlobalSetup() const;

        //! Update info
        //! \threadsafe
        BlackCore::Data::CUpdateInfo getUpdateInfo() const;

        //! Delete all cookies from cookier manager
        void deleteAllCookies();

        //! Setup already synchronized
        bool isSetupAvailable() const;

        //! Reload setup and version
        BlackMisc::CStatusMessageList requestReloadOfSetupAndVersion();

        //! Web data services available?
        bool hasWebDataServices() const;

        //! Get the web data services
        CWebDataServices *getWebDataServices() const;

        //! Currently running in application thread?
        bool isApplicationThread() const;

        //! String with beta, dev. and version
        const QString &versionStringDevBetaInfo() const;

        //! swift info string
        const QString &swiftVersionString() const;

        //! swift info string
        const char *swiftVersionChar();

        //! Running in dev.environment?
        bool isRunningInDeveloperEnvironment() const { return this->m_devEnv; }

        //! Signal startup automatically or individually
        void setSignalStartupAutomatically(bool enabled);

        //! Info string
        QString getEnvironmentInfoString(const QString &separator) const;

        //! To string
        QString getInfoString(const QString &separator) const;

        //! Unsaved settings
        bool hasUnsavedSettings() const;

        //! Automatically and always save settings
        void setSettingsAutoSave(bool autoSave);

        //! All unsaved settings
        QStringList getAllUnsavedSettings() const;

        //! Save all settings
        BlackMisc::CStatusMessage saveSettingsByKey(const QStringList &keys);

        //! Directory for temporary files
        QString getTemporaryDirectory() const;

        //! Run event loop
        static int exec();

        //! Exit application, perform graceful shutdown and exit
        static void exit(int retcode = 0);

        //! Similar to QCoreApplication::arguments
        static QStringList arguments();

        //! Process all events for some time
        //! \remark unlike QCoreApplication::processEvents this will spend at least the given time in the function, using QThread::msleep
        static void processEventsFor(int milliseconds);

        //! Clear the caches
        static QStringList clearCaches();

        // ----------------------- parsing ----------------------------------------

        //! \name parsing of command line options
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

        //! Private resource dir for developer's own resource files
        QString getCmdSwiftPrivateSharedDir() const;

        //! Delegates to QCommandLineParser::isSet
        bool isParserOptionSet(const QString &option) const;

        //! Delegates to QCommandLineParser::isSet
        bool isParserOptionSet(const QCommandLineOption &option) const;

        //! Delegates to QCommandLineParser::value
        QString getParserValue(const QString &option) const;

        //! Delegates to QCommandLineParser::value
        QString getParserValue(const QCommandLineOption &option) const;

        //! Parses and handles the standard options such as help, version, parse error
        //! \note in some cases (error, version, help) application is terminated during this step
        //! \sa parsingHookIn
        //! \return true means to continue, false to stop
        bool parse();
        //! @}

        //! Display error message
        virtual void cmdLineErrorMessage(const QString &cmdLineErrorMessage) const;

        //! Display error message
        virtual void cmdLineErrorMessage(const BlackMisc::CStatusMessageList &msgs) const;

        // ----------------------- contexts ----------------------------------------

        //! \name Context / core facade related
        //! @{

        //! Supports contexts
        //! \remark checks the real availability of the contexts, so it can happen that we want to use contexts, and they are not yet initialized (false here)
        //! \sa m_useContexts we use or we will use contexts
        bool supportsContexts() const;

        //! Init the contexts part and start core facade
        //! \sa coreFacadeStarted
        BlackMisc::CStatusMessageList useContexts(const CCoreFacadeConfig &coreConfig);

        //! Init web data services and start them
        //! \sa webDataServicesStarted
        BlackMisc::CStatusMessageList useWebDataServices(const CWebReaderFlags::WebReader webReader, const BlackCore::Db::CDatabaseReaderConfigList &dbReaderConfig);

        //! Get the facade
        CCoreFacade *getCoreFacade() { return m_coreFacade.data(); }

        //! Get the facade
        const CCoreFacade *getCoreFacade() const { return m_coreFacade.data(); }
        //! @}

        //! \name Direct access to contexts if a CCoreFacade has been initialized
        //! @{
        const Context::IContextNetwork *getIContextNetwork() const;
        const Context::IContextAudio *getIContextAudio() const;
        const Context::IContextApplication *getIContextApplication() const;
        const Context::IContextOwnAircraft *getIContextOwnAircraft() const;
        const Context::IContextSimulator *getIContextSimulator() const;
        Context::IContextNetwork *getIContextNetwork();
        Context::IContextAudio *getIContextAudio();
        Context::IContextApplication *getIContextApplication();
        Context::IContextOwnAircraft *getIContextOwnAircraft();
        Context::IContextSimulator *getIContextSimulator();
        //! @}

        // ----------------------- direct access to some setup data ---------------------------------

        //! Consolidated version of METAR URLs, either from CGlobalSetup or CVatsimSetup
        //! \threadsafe
        BlackMisc::Network::CUrlList getVatsimMetarUrls() const;

        //! Consolidated version of data file URLs, either from CGlobalSetup or CVatsimSetup
        //! \threadsafe
        BlackMisc::Network::CUrlList getVatsimDataFileUrls() const;

    public slots:
        //! Graceful shutdown
        virtual void gracefulShutdown();

        //! Start services, if not yet parsed call CApplication::parse
        virtual bool start();

        //! Wait for setup data by calling the event loop and waiting until everything is ready
        BlackMisc::CStatusMessageList waitForSetup();

        //! Request to get network reply
        //! \threadsafe
        QNetworkReply *getFromNetwork(const BlackMisc::Network::CUrl &url,
                                      const BlackMisc::CSlot<void(QNetworkReply *)> &callback);

        //! Request to get network reply
        //! \threadsafe
        QNetworkReply *getFromNetwork(const QNetworkRequest &request,
                                      const BlackMisc::CSlot<void(QNetworkReply *)> &callback);

        //! Post to network
        //! \threadsafe
        QNetworkReply *postToNetwork(const QNetworkRequest &request, const QByteArray &data,
                                     const BlackMisc::CSlot<void(QNetworkReply *)> &callback);

        //! Post to network
        //! \note This method takes ownership over \c multiPart.
        //! \threadsafe
        QNetworkReply *postToNetwork(const QNetworkRequest &request, QHttpMultiPart *multiPart,
                                     const BlackMisc::CSlot<void(QNetworkReply *)> &callback);

        //! Request to get network repy using HTTP's HEADER method
        //! \threadsafe
        QNetworkReply *headerFromNetwork(const BlackMisc::Network::CUrl &url,
                                         const BlackMisc::CSlot<void(QNetworkReply *)> &callback);

        //! Request to get network repy using HTTP's HEADER method
        //! \threadsafe
        QNetworkReply *headerFromNetwork(const QNetworkRequest &request,
                                         const BlackMisc::CSlot<void(QNetworkReply *)> &callback);

    signals:
        //! Setup available (cache, web load, ..) or failed to load setup
        void setupHandlingCompleted(bool success);

        //! Update info available (cache, web load)
        void updateInfoAvailable(bool success);

        //! Startup has been completed
        //! \remark needs to be triggered by application when it think it is done
        void startUpCompleted(bool success);

        //! Facade started
        void coreFacadeStarted();

        //! Web data services started
        void webDataServicesStarted(bool success);

    protected slots:
        //! Setup read/synchronized
        void ps_setupHandlingCompleted(bool available);

        //! Startup completed
        virtual void ps_startupCompleted();

    protected:
        //! Init class, allows to init from BlackGui::CGuiApplication as well (pseudo virtual)
        void init(bool withMetadata);

        //! Display help message
        virtual void cmdLineHelpMessage();

        //! Display version message
        virtual void cmdLineVersionMessage() const;

        //! Can be used to parse specialized arguments
        virtual bool parsingHookIn() { return true; }

        //! Can be used to start special services
        virtual BlackMisc::CStatusMessageList startHookIn() { return BlackMisc::CStatusMessageList(); }

        //! Flag set or explicitly set to true
        bool isSetOrTrue(const QCommandLineOption &option) const;

        //! Severe issue during startup, most likely it does not make sense to continue
        //! \note call this here if the parsing stage is over and reaction to a runtime issue
        //!       is needed
        void severeStartupProblem(const BlackMisc::CStatusMessage &message);

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
        QCommandLineParser m_parser;                       //!< cmd parser
        QCommandLineOption m_cmdHelp {"help"};             //!< help option
        QCommandLineOption m_cmdVersion {"version"};       //!< version option
        QCommandLineOption m_cmdDBusAddress {"empty"};     //!< DBus address
        QCommandLineOption m_cmdDevelopment {"dev"};       //!< Dev. flag
        QCommandLineOption m_cmdSharedDir {"shared"};      //!< Shared directory
        QCommandLineOption m_cmdClearCache {"clearcache"}; //!< Clear cache
        bool               m_parsed  = false;              //!< Parsing accomplished?
        bool               m_started = false;              //!< started with success?
        bool               m_startSetupReader = false;     //!< start the setup reader

    private:
        //! init logging system
        void initLogging();

        //! Init parser
        void initParser();

        //! Dev.environment
        bool initIsRunningInDeveloperEnvironment() const;

        //! Async. start when setup is loaded
        BlackMisc::CStatusMessageList asyncWebAndContextStart();

        //! Implementation for getFromNetwork(), postToNetwork() and headerFromNetwork()
        QNetworkReply *httpRequestImpl(const QNetworkRequest &request,
                                       const BlackMisc::CSlot<void(QNetworkReply *)> &callback,
                                       std::function<QNetworkReply *(QNetworkAccessManager &, const QNetworkRequest &)> method);

        QScopedPointer<CCoreFacade>              m_coreFacade;             //!< core facade if any
        QScopedPointer<CSetupReader>             m_setupReader;            //!< setup reader
        QScopedPointer<CWebDataServices>         m_webDataServices;        //!< web data services
        QScopedPointer<BlackMisc::CFileLogger>   m_fileLogger;             //!< file logger
        QNetworkAccessManager                    m_accessManager { this }; //!< single network access manager
        CCookieManager                           m_cookieManager;          //!< single cookie manager for our access manager
        QString                                  m_applicationName;        //!< application name
        BlackMisc::CApplicationInfo::Application m_application = BlackMisc::CApplicationInfo::Unknown; //!< Application if specified
        QReadWriteLock                           m_accessManagerLock;      //!< lock to make access manager access threadsafe
        CCoreFacadeConfig                        m_coreFacadeConfig;       //!< Core facade config if any
        CWebReaderFlags::WebReader               m_webReadersUsed;         //!< Readers to be used
        BlackCore::Db::CDatabaseReaderConfigList m_dbReaderConfig;         //!< Load or used caching?
        std::atomic<bool>                        m_shutdown { false };     //!< is being shutdown?
        bool                                     m_useContexts = false;    //!< use contexts
        bool                                     m_useWebData = false;     //!< use web data
        bool                                     m_signalStartup = true;   //!< signal startup automatically
        bool                                     m_devEnv = false;         //!< dev. environment
        bool                                     m_unitTest = false;       //!< is UNIT test
        bool                                     m_autoSaveSettings = true;//!< automatically saving all settings

        // -------------- crashpad -----------------
        BlackMisc::CStatusMessageList initCrashHandler();
        void crashDumpUploadEnabledChanged();

        #ifdef BLACK_USE_CRASHPAD
        std::unique_ptr<crashpad::CrashpadClient> m_crashpadClient;
        std::unique_ptr<crashpad::CrashReportDatabase> m_crashReportDatabase;
        BlackMisc::CSettingReadOnly<BlackCore::Application::TCrashDumpUploadEnabled> m_crashDumpUploadEnabled { this, &CApplication::crashDumpUploadEnabledChanged };
        #endif
    };
} // namespace

//! Single instance of application object
extern BLACKCORE_EXPORT BlackCore::CApplication *sApp;

#endif // guard
