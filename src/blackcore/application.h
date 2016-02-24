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

#include "corefacadeconfig.h"
#include "cookiemanager.h"
#include "webdataservices.h"
#include "blackmisc/network/url.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/filelogger.h"
#include "blackmisc/slot.h"
#include "blackcoreexport.h"
#include <QObject>
#include <QScopedPointer>
#include <QNetworkAccessManager>
#include <QCommandLineParser>
#include <atomic>

namespace BlackCore
{
    class CCoreFacade;
    class CSetupReader;

    class IContextApplication;
    class IContextAudio;
    class IContextNetwork;
    class IContextOwnAircraft;
    class IContextSimulator;

    /*!
     * Our runtime. Normally one instance is to be initialized at the beginning of main, and thereafter
     * it can be used everywhere via QApplication::instance
     *
     * - A swift standard cmd line parser is part of the application.
     *   Hence cmd arguments can be optained any time / everywhere when required.
     *   Also some standard swift cmd arguments do not need to be re-implemented for each swift application.
     * - The core facade (aka core runtime) is now part of the application. It can be started via cmd line arguments.
     * - Settings are loaded
     * - Setup is loaded (load the so called bootsrap file) to find servers and other resources
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
        CApplication(const QString &applicationName = executable());

        //! Destructor
        virtual ~CApplication();

        //! Application name and version
        QString getApplicationNameAndVersion() const;

        //! Start services, if not yet parsed call CApplication::parse
        virtual bool start(bool waitForStart = true);

        //! Wait for stert by calling the event loop and waiting until everything is ready
        bool waitForStart();

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
        //! \threadsafe
        QNetworkReply *postToNetwork(const QNetworkRequest &request, QHttpMultiPart *multiPart,
                                     const BlackMisc::CSlot<void(QNetworkReply *)> &callback);

        //! Delete all cookies from cookier manager
        void deleteAllCookies();

        //! Setup already syncronized
        bool isSetupSyncronized() const;

        //! Reload setup and version
        BlackMisc::CStatusMessage requestReloadOfSetupAndVersion();

        //! Web data services available?
        bool hasWebDataServices() const;

        //! Get the web data services
        CWebDataServices *getWebDataServices() const;

        //! Run event loop
        static int exec();

        //! Exit application, perform graceful shutdown and exit
        static void exit(int retcode = 0);

        //! Similar to QCoreApplication::arguments
        static QStringList arguments();

        // ----------------------- parsing ----------------------------------------

        //! \name parsing of command line options
        //! @{

        //! \copydoc QCommandLineParser::addOption(const QCommandLineOption &)
        bool addParserOption(const QCommandLineOption &option);

        //! CMD line argument for DBus address
        void addDBusAddressOption();

        //! DBus address from CMD line, otherwise ""
        QString getCmdDBusAddressValue() const;

        //! Delegates to QCommandLineParser::isSet
        bool isParserOptionSet(const QString &option) const;

        //! Delegates to QCommandLineParser::isSet
        bool isParserOptionSet(const QCommandLineOption &option) const;

        //! Delegates to QCommandLineParser::value
        QString getParserOptionValue(const QString &option) const;

        //! Delegates to QCommandLineParser::value
        QString getParserOptionValue(const QCommandLineOption &option) const;

        //! Display parser error message
        virtual void errorMessage(const QString &errorMessage) const;

        //! Parses and handles the standard options such as help, version, parse error
        //! \note in some cases (error, version, help) application is terminated during this step
        //! \sa parsingHookIn
        bool parse();
        //! @}

        // ----------------------- contexts ----------------------------------------

        //! \name Context / core facade related
        //! @{

        //! Supports contexts
        bool supportsContexts() const;

        //! Init the contexts part and start core facade
        //! \sa coreFacadeStarted
        bool useContexts(const CCoreFacadeConfig &coreConfig);

        //! Init web data services and start them
        //! \sa webDataServicesStarted
        bool useWebDataServices(const CWebReaderFlags::WebReader webReader, CWebReaderFlags::DbReaderHint hint);

        //! Get the facade
        CCoreFacade *getCoreFacade() { return m_coreFacade.data(); }

        //! Get the facade
        const CCoreFacade *getCoreFacade() const { return m_coreFacade.data(); }
        //! @}

        //! \name Direct access to contexts if a CCoreFacade has been initialized
        //! @{
        const IContextNetwork *getIContextNetwork() const;
        const IContextAudio *getIContextAudio() const;
        const IContextApplication *getIContextApplication() const;
        const IContextOwnAircraft *getIContextOwnAircraft() const;
        const IContextSimulator *getIContextSimulator() const;
        IContextNetwork *getIContextNetwork();
        IContextAudio *getIContextAudio();
        IContextApplication *getIContextApplication();
        IContextOwnAircraft *getIContextOwnAircraft();
        IContextSimulator *getIContextSimulator();
        //! @}

    public slots:
        //! Graceful shutdown
        virtual void gracefulShutdown();

    signals:
        //! Facade started
        void coreFacadeStarted();

        //! Web data services started
        void webDataServicesStarted();

    protected slots:
        //! Setup read/syncronized
        void ps_setupSyncronized(bool success);

    protected:
        //! Constructor
        CApplication(const QString &applicationName, QCoreApplication *app);

        //! Display help message
        virtual void helpMessage();

        //! Display version message
        virtual void versionMessage() const;

        //! Can be used to parse specialized arguments
        virtual bool parsingHookIn() { return true; }

        //! Can be used to start special services
        virtual bool startHookIn() { return true; }

        //! Severe issue during startup, most likely it does not make sense to continue
        //! \note call this here if the parsing stage is over and reaction to a runtime issue
        //!       is needed
        void severeStartupProblem(const BlackMisc::CStatusMessage &message);

        //! Start the core facade
        //! \note does nothing when setup is not yet loaded
        bool startCoreFacade();

        //! Start the web data services
        //! \note does nothing when setup is not yet loaded
        bool startWebDataServices();

        //! executable name
        static const QString &executable();

        // cmd parsing
        QCommandLineParser m_parser;                    //!< cmd parser
        QCommandLineOption m_cmdHelp {"help"};          //!< help option
        QCommandLineOption m_cmdVersion {"version"};    //!< version option
        QCommandLineOption m_cmdDBusAddress {"empty"};  //!< DBus address
        bool               m_parsed = false;            //!< Parsing accomplished?
        bool               m_started = false;           //!< started with success?
        bool               m_startUpCompleted = false;  //!< startup phase completed? Can mean startup failed
        bool               m_startSetupReader = false;  //!< start the setup reader

    private:
        //! init logging system
        void initLogging();

        //! Init parser
        void initParser();

        //! static init part
        static void initEnvironment();

        QScopedPointer<CCoreFacade>            m_coreFacade;             //!< core facade if any
        QScopedPointer<CSetupReader>           m_setupReader;            //!< setup reader
        QScopedPointer<CWebDataServices>       m_webDataServices;        //!< web data services
        QScopedPointer<BlackMisc::CFileLogger> m_fileLogger;             //!< file logger
        QNetworkAccessManager                  m_accessManager { this }; //!< single network access manager
        CCookieManager                         m_cookieManager;          //!< single cookie manager for our access manager
        QString                                m_applicationName;        //!< application name
        QReadWriteLock                         m_accessManagerLock;      //!< lock to make accessmanager access threadsafe
        CCoreFacadeConfig                      m_coreFacadeConfig;       //!< Core facade config if any
        CWebReaderFlags::WebReader             m_webReader;              //!< Readers used
        CWebReaderFlags::DbReaderHint          m_dbReaderHint;           //!< Load or used caching?
        std::atomic<bool>                      m_shutdown { false };     //!< is being shutdown?
        bool                                   m_useContexts = false;    //!< use contexts
        bool                                   m_useWebData = false;     //!< use web data
    };
} // namespace

//! Single instance of application object
extern BLACKCORE_EXPORT BlackCore::CApplication *sApp;

#endif // guard
