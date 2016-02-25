/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "application.h"
#include "blackcore/corefacade.h"
#include "blackcore/setupreader.h"
#include "blackcore/webdataservices.h"
#include "blackcore/contextapplication.h"
#include "blackcore/registermetadata.h"
#include "blackcore/cookiemanager.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/project.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/verify.h"
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <stdlib.h>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;
using namespace BlackCore;

BlackCore::CApplication *sApp = nullptr; // set by constructor

namespace BlackCore
{
    CApplication::CApplication(const QString &applicationName) :
        m_applicationName(applicationName),
        m_coreFacadeConfig(CCoreFacadeConfig::allEmpty())
    {
        Q_ASSERT_X(!sApp, Q_FUNC_INFO, "already initialized");
        Q_ASSERT_X(QCoreApplication::instance(), Q_FUNC_INFO, "no application object");
        if (!sApp)
        {
            CApplication::initEnvironment();
            QCoreApplication::setApplicationName(applicationName);
            QCoreApplication::setApplicationVersion(CProject::version());
            this->setObjectName(applicationName);
            this->initParser();
            this->initLogging();

            // Translations
            QFile file(":blackmisc/translations/blackmisc_i18n_de.qm");
            CLogMessage(this).debug() << (file.exists() ? "Found translations in resources" : "No translations in resources");
            QTranslator translator;
            if (translator.load("blackmisc_i18n_de", ":blackmisc/translations/"))
            {
                CLogMessage(this).debug() << "Translator loaded";
            }
            QCoreApplication::instance()->installTranslator(&translator);

            // Global setup / bootstraping
            this->m_cookieManager.setParent(&this->m_accessManager);
            this->m_accessManager.setCookieJar(&this->m_cookieManager);

            // trigger loading of settings
            //! \todo maybe loaded twice, context initializing might trigger loading of settings a second time
            CStatusMessage m = CSettingsCache::instance()->loadFromStore();
            if (!m.isEmpty())
            {
                m.setCategories(getLogCategories());
                CLogMessage(this).preformatted(m);
            }

            // global setup
            sApp = this;
            this->m_setupReader.reset(new CSetupReader(this));
            connect(this->m_setupReader.data(), &CSetupReader::setupSynchronized, this, &CApplication::ps_setupSyncronized);
            this->m_parser.addOptions(this->m_setupReader->getCmdLineOptions());

            // notify when app goes down
            connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &CApplication::gracefulShutdown);
        }
    }

    CApplication::~CApplication()
    {
        this->gracefulShutdown();
    }

    QString CApplication::getApplicationNameAndVersion() const
    {
        return QCoreApplication::instance()->applicationName() + " " + CProject::version();
    }

    bool CApplication::start(bool waitForStart)
    {
        if (!this->m_parsed)
        {
            bool s = this->parse();
            if (!s) { return false; }
        }

        // parsing itself is done
        if (this->m_startSetupReader)
        {
            CStatusMessage m(this->requestReloadOfSetupAndVersion());
            if (m.isWarningOrAbove())
            {
                this->errorMessage(m.getMessage());
                return false;
            }
        }

        bool s = this->startHookIn();
        if (waitForStart)
        {
            s = this->waitForStart();
        }
        this->m_started = s;
        return s;
    }

    bool CApplication::waitForStart()
    {
        // process events return immediatley if nothing is to be processed
        const QTime dieTime = QTime::currentTime().addMSecs(5000);
        while (QTime::currentTime() < dieTime && !this->m_started && !this->m_startUpCompleted)
        {
            QCoreApplication::instance()->processEvents(QEventLoop::AllEvents, 250);
        }
        if (this->m_startUpCompleted)
        {
            CLogMessage(this).error("Waiting for startup timed out");
        }
        return this->m_started;
    }

    bool CApplication::isSetupSyncronized() const
    {
        if (this->m_shutdown || !this->m_setupReader) { return false; }
        return this->m_setupReader->isSetupSyncronized();
    }

    CStatusMessage CApplication::requestReloadOfSetupAndVersion()
    {
        if (!this->m_shutdown)
        {
            Q_ASSERT_X(this->m_setupReader, Q_FUNC_INFO, "Missing reader");
            Q_ASSERT_X(this->m_parsed, Q_FUNC_INFO, "Not yet parsed");
            return this->m_setupReader->asyncLoad();
        }
        else
        {
            return CStatusMessage(getLogCategories(), CStatusMessage::SeverityError, "No reader for setup/version");
        }
    }

    bool CApplication::hasWebDataServices() const
    {
        return this->m_webDataServices;
    }

    CWebDataServices *CApplication::getWebDataServices() const
    {
        Q_ASSERT_X(this->m_webDataServices, Q_FUNC_INFO, "Missing web data services");
        return this->m_webDataServices.data();
    }

    QNetworkReply *CApplication::getFromNetwork(const CUrl &url, const BlackMisc::CSlot<void (QNetworkReply *)> &callback)
    {
        if (this->m_shutdown) { return nullptr; }
        return getFromNetwork(url.toNetworkRequest(), callback);
    }

    QNetworkReply *CApplication::getFromNetwork(const QNetworkRequest &request, const BlackMisc::CSlot<void(QNetworkReply *)> &callback)
    {
        if (this->m_shutdown) { return nullptr; }
        QNetworkRequest r(request);
        CNetworkUtils::ignoreSslVerification(r);
        QWriteLocker locker(&m_accessManagerLock);
        QNetworkReply *reply = this->m_accessManager.get(r);
        if (callback)
        {
            connect(reply, &QNetworkReply::finished, callback.object(), [ = ] { callback(reply); });
        }
        return reply;
    }

    QNetworkReply *CApplication::postToNetwork(const QNetworkRequest &request, const QByteArray &data, const BlackMisc::CSlot<void (QNetworkReply *)> &callback)
    {
        if (this->m_shutdown) { return nullptr; }
        QNetworkRequest r(request);
        CNetworkUtils::ignoreSslVerification(r);
        QWriteLocker locker(&m_accessManagerLock);
        QNetworkReply *reply = this->m_accessManager.post(r, data);
        if (callback)
        {
            connect(reply, &QNetworkReply::finished, callback.object(), [ = ] { callback(reply); });
        }
        return reply;
    }

    QNetworkReply *CApplication::postToNetwork(const QNetworkRequest &request, QHttpMultiPart *multiPart, const BlackMisc::CSlot<void (QNetworkReply *)> &callback)
    {
        if (this->m_shutdown) { return nullptr; }
        QNetworkRequest r(request);
        CNetworkUtils::ignoreSslVerification(r);
        QWriteLocker locker(&m_accessManagerLock);
        QNetworkReply *reply = this->m_accessManager.post(r, multiPart);
        if (callback)
        {
            connect(reply, &QNetworkReply::finished, callback.object(), [ = ] { callback(reply); });
        }
        return reply;
    }

    void CApplication::deleteAllCookies()
    {
        this->m_cookieManager.deleteAllCookies();
    }

    int CApplication::exec()
    {
        Q_ASSERT_X(instance(), Q_FUNC_INFO, "missing application");
        return QCoreApplication::exec();
    }

    void CApplication::exit(int retcode)
    {
        if (instance())
        {
            instance()->gracefulShutdown();
        }
        // when the event loop is not running, this does nothing
        QCoreApplication::exit(retcode);
    }

    QStringList CApplication::arguments()
    {
        return QCoreApplication::arguments();
    }

    bool CApplication::useContexts(const CCoreFacadeConfig &coreConfig)
    {
        Q_ASSERT_X(this->m_parsed, Q_FUNC_INFO, "Call this after parsing");

        this->m_useContexts = true;
        this->m_coreFacadeConfig = coreConfig;
        return this->startCoreFacade(); // will do nothing if setup is not yet loaded
    }

    bool CApplication::useWebDataServices(const CWebReaderFlags::WebReader webReader, CWebReaderFlags::DbReaderHint hint)
    {
        Q_ASSERT_X(this->m_webDataServices.isNull(), Q_FUNC_INFO, "Services already started");
        this->m_webReader = webReader;
        this->m_dbReaderHint = hint;
        this->m_useWebData = true;
        return this->startWebDataServices();
    }

    bool CApplication::startCoreFacade()
    {
        if (!this->m_useContexts) { return true; } // we do not use context, so no need to startup
        if (!this->m_parsed) { return false; }
        if (!this->m_setupReader || !this->m_setupReader->isSetupSyncronized()) { return false; }

        Q_ASSERT_X(this->m_coreFacade.isNull(), Q_FUNC_INFO, "Cannot alter facade");
        Q_ASSERT_X(this->m_setupReader, Q_FUNC_INFO, "No facade without setup possible");
        Q_ASSERT_X(this->m_useWebData, Q_FUNC_INFO, "Need web data services");

        this->startWebDataServices();

        CLogMessage(this).info("Will start core facade now");
        this->m_coreFacade.reset(new CCoreFacade(this->m_coreFacadeConfig));
        emit this->coreFacadeStarted();
        return true;
    }

    bool CApplication::startWebDataServices()
    {
        if (!this->m_useWebData) { return true; }
        if (!this->m_parsed) { return false; }
        if (!this->m_setupReader || !this->m_setupReader->isSetupSyncronized()) { return false; }

        Q_ASSERT_X(this->m_setupReader, Q_FUNC_INFO, "No web data services without setup possible");
        if (!this->m_webDataServices)
        {
            CLogMessage(this).info("Will start web data services now");
            this->m_webDataServices.reset(
                new CWebDataServices(this->m_webReader, this->m_dbReaderHint)
            );
        }
        return true;
    }

    void CApplication::initLogging()
    {
        CLogHandler::instance()->install(); // make sure we have a log handler!

        // File logger
        static const QString logPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/org.swift-project/logs";
        this->m_fileLogger.reset(new CFileLogger(executable(), logPath));
        this->m_fileLogger->changeLogPattern(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityDebug));
    }

    void CApplication::initParser()
    {
        this->m_parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
        this->m_parser.setApplicationDescription(m_applicationName);
        this->m_cmdHelp = this->m_parser.addHelpOption();
        this->m_cmdVersion = this->m_parser.addVersionOption();
    }

    void CApplication::initEnvironment()
    {
        BlackMisc::registerMetadata();
        BlackCore::registerMetadata();
    }

    void CApplication::gracefulShutdown()
    {
        if (this->m_shutdown) { return; }
        this->m_shutdown = true;

        // save settings (but only when application was really alive)
        CStatusMessage m;
        if (this->m_parsed)
        {
            if (this->supportsContexts())
            {
                m = this->getIContextApplication()->saveSettings();
            }
            else
            {
                m = CSettingsCache::instance()->saveToStore();
            }
            CLogMessage(getLogCategories()).preformatted(m);
        }

        // from here on we really rip appart the application object
        // and it should no longer be used
        sApp = nullptr;
        disconnect(this);

        if (this->supportsContexts())
        {
            // clean up facade
            this->m_coreFacade->gracefulShutdown();
            this->m_coreFacade.reset();
        }

        if (this->m_webDataServices)
        {
            this->m_webDataServices->gracefulShutdown();
            this->m_webDataServices.reset();
        }

        if (this->m_setupReader)
        {
            this->m_setupReader->gracefulShutdown();
        }

        this->m_fileLogger->close();
    }

    void CApplication::ps_setupSyncronized(bool success)
    {
        if (success)
        {
            if (!this->m_started)
            {
                // follow up startups
                bool s = this->startWebDataServices();
                this->m_started = s && this->startCoreFacade();
            }
        }
        this->m_startUpCompleted = true;
    }

    void CApplication::severeStartupProblem(const CStatusMessage &message)
    {
        CLogMessage(this).preformatted(message);
        this->errorMessage(message.getMessage());
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

    const BlackMisc::CLogCategoryList &CApplication::getLogCategories()
    {
        static const CLogCategoryList l({ CLogCategory("swift.application"), CLogCategory("swift." + executable())});
        return l;
    }

    // ---------------------------------------------------------------------------------
    // Parsing
    // ---------------------------------------------------------------------------------

    bool CApplication::addParserOption(const QCommandLineOption &option)
    {
        return this->m_parser.addOption(option);
    }

    void CApplication::addDBusAddressOption()
    {
        this->m_cmdDBusAddress = QCommandLineOption({ "dbus", "dbus-address", "dbusaddress" },
                                 QCoreApplication::translate("application", "DBus address."),
                                 "dbusaddress");
        this->addParserOption(this->m_cmdDBusAddress);
    }

    QString CApplication::getCmdDBusAddressValue() const
    {
        if (this->isParserOptionSet(this->m_cmdDBusAddress))
        {
            const QString v(this->getParserOptionValue(m_cmdDBusAddress));
            const QString dBusAddress(CDBusServer:: normalizeAddress(v));
            return dBusAddress;
        }
        else
        {
            return "";
        }
    }

    bool CApplication::isParserOptionSet(const QString &option) const
    {
        return this->m_parser.isSet(option);
    }

    bool CApplication::isParserOptionSet(const QCommandLineOption &option) const
    {
        return this->m_parser.isSet(option);
    }

    QString CApplication::getParserOptionValue(const QString &option) const
    {
        return this->m_parser.value(option).trimmed();
    }

    QString CApplication::getParserOptionValue(const QCommandLineOption &option) const
    {
        return this->m_parser.value(option).trimmed();
    }

    bool CApplication::parse()
    {
        if (this->m_parsed) { return m_parsed; }

        // we call parse because we also want to display a GUI error message when applicable
        QStringList args(QCoreApplication::instance()->arguments());
        if (!this->m_parser.parse(args))
        {
            this->errorMessage(this->m_parser.errorText());
            return false;
        }

        // help/version
        if (this->m_parser.isSet(this->m_cmdHelp))
        {
            // Important parser help will already stop application
            this->helpMessage();
            return true;
        }
        if (this->m_parser.isSet(this->m_cmdVersion))
        {
            this->versionMessage();
            return true;
        }

        // Hookin, other parsing
        if (!this->parsingHookIn()) { return false; }

        // setup reader
        this->m_startSetupReader = this->m_setupReader->parseCmdLineArguments();
        this->m_parsed = true;
        return true;
    }

    void CApplication::errorMessage(const QString &errorMessage) const
    {
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(this->m_parser.helpText()), stderr);
    }

    void CApplication::helpMessage()
    {
        this->m_parser.showHelp(); // terminates
        Q_UNREACHABLE();
    }

    void CApplication::versionMessage() const
    {
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()), qPrintable(QCoreApplication::applicationVersion()));
    }

    // ---------------------------------------------------------------------------------
    // Contexts
    // ---------------------------------------------------------------------------------

    bool CApplication::supportsContexts() const
    {
        if (m_coreFacade.isNull()) { return false; }
        if (!this->m_coreFacade->getIContextApplication()) { return false; }
        return (!this->m_coreFacade->getIContextApplication()->isEmptyObject());
    }

    const IContextNetwork *CApplication::getIContextNetwork() const
    {
        if (!supportsContexts()) { return nullptr; }
        return this->m_coreFacade->getIContextNetwork();
    }

    const IContextAudio *CApplication::getIContextAudio() const
    {
        if (!supportsContexts()) { return nullptr; }
        return this->m_coreFacade->getIContextAudio();
    }

    const IContextApplication *CApplication::getIContextApplication() const
    {
        if (!supportsContexts()) { return nullptr; }
        return this->m_coreFacade->getIContextApplication();
    }

    const IContextOwnAircraft *CApplication::getIContextOwnAircraft() const
    {
        if (!supportsContexts()) { return nullptr; }
        return this->m_coreFacade->getIContextOwnAircraft();
    }

    const IContextSimulator *CApplication::getIContextSimulator() const
    {
        if (!supportsContexts()) { return nullptr; }
        return this->m_coreFacade->getIContextSimulator();
    }

    IContextNetwork *CApplication::getIContextNetwork()
    {
        if (!supportsContexts()) { return nullptr; }
        return this->m_coreFacade->getIContextNetwork();
    }

    IContextAudio *CApplication::getIContextAudio()
    {
        if (!supportsContexts()) { return nullptr; }
        return this->m_coreFacade->getIContextAudio();
    }

    IContextApplication *CApplication::getIContextApplication()
    {
        if (!supportsContexts()) { return nullptr; }
        return this->m_coreFacade->getIContextApplication();
    }

    IContextOwnAircraft *CApplication::getIContextOwnAircraft()
    {
        if (!supportsContexts()) { return nullptr; }
        return this->m_coreFacade->getIContextOwnAircraft();
    }

    IContextSimulator *CApplication::getIContextSimulator()
    {
        if (!supportsContexts()) { return nullptr; }
        return this->m_coreFacade->getIContextSimulator();
    }
} // ns
