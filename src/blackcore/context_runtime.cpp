#include "blackcore/context_runtime.h"
#include "blackcore/context_all_impl.h"
#include "blackcore/context_all_proxies.h"
#include "blackcore/blackcorefreefunctions.h"

#include "blacksim/blacksimfreefunctions.h"

#include "blackmisc/nwserver.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/blackmiscfreefunctions.h"

#include <QDebug>

namespace BlackCore
{
    /*
     * Constructor
     */
    CRuntime::CRuntime(const CRuntimeConfig &config, QObject *parent) :
        QObject(parent), m_init(false), m_dbusServer(nullptr), m_initDBusConnection(false),
        m_signalLogApplication(false), m_signalLogAudio(false), m_signalLogNetwork(false), m_signalLogOwnAircraft(false), m_signalLogSettings(false), m_signalLogSimulator(false),
        m_slotLogApplication(false), m_slotLogAudio(false), m_slotLogNetwork(false), m_slotLogOwnAircraft(false), m_slotLogSettings(false), m_slotLogSimulator(false),
        m_dbusConnection(QDBusConnection("default")),
        m_contextApplication(nullptr), m_contextAudio(nullptr), m_contextNetwork(nullptr), m_contextSettings(nullptr), m_contextSimulator(nullptr)
    {
        this->init(config);
    }

    /*
     * Signal logging
     */
    void CRuntime::signalLog(bool enabled)
    {
        this->signalLogForApplication(enabled);
        this->signalLogForAudio(enabled);
        this->signalLogForNetwork(enabled);
        this->signalLogForOwnAircraft(enabled);
        this->signalLogForSettings(enabled);
        this->signalLogForSimulator(enabled);
    }

    /*
     * Signal logging
     */
    void CRuntime::slotLog(bool enabled)
    {
        this->slotLogForApplication(enabled);
        this->slotLogForAudio(enabled);
        this->slotLogForNetwork(enabled);
        this->slotLogForOwnAircraft(enabled);
        this->slotLogForSettings(enabled);
        this->slotLogForSimulator(enabled);
    }

    /*
     * Enable signal logging
     */
    bool CRuntime::signalLogForApplication(bool enabled)
    {
        if (enabled == this->m_signalLogApplication) return enabled;
        if (!this->getIContextApplication())
        {
            this->m_signalLogApplication = false;
            return false;
        }
        this->m_signalLogApplication = enabled;
        if (enabled)
        {
            // connect signal / slots when enabled
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextApplication(), &IContextApplication::componentChanged,
            [this](uint component, uint action) { QStringList l; l << "componentChanged" << QString::number(component) << QString::number(action); this->logSignal(this->getIContextApplication(), l);});
            this->m_logSignalConnections.insert("application", con);
            con = QObject::connect(this->getIContextApplication(), &IContextApplication::statusMessage,
            [this](const BlackMisc::CStatusMessage & msg) { QStringList l; l << "statusMessage" << msg.toQString() ; this->logSignal(this->getIContextApplication(), l);});
            this->m_logSignalConnections.insert("application", con);
            con = QObject::connect(this->getIContextApplication(), &IContextApplication::statusMessages,
            [this](const BlackMisc::CStatusMessageList & msgs) { QStringList l; l << "statusMessages" << msgs.toQString(); this->logSignal(this->getIContextApplication(), l);});
            this->m_logSignalConnections.insert("application", con);
        }
        else
        {
            this->disconnectLogSignals("application");
        }
        return enabled;
    }

    bool CRuntime::signalLogForAudio(bool enabled)
    {
        if (enabled == this->m_signalLogAudio) return enabled;
        if (!this->getIContextNetwork())
        {
            this->m_signalLogAudio = false;
            return false;
        }
        this->m_signalLogAudio = enabled;
        if (enabled)
        {
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextAudio(), &IContextAudio::audioTestCompleted,
            [this]() { QStringList l; l << "audioTestCompleted"; this->logSignal(this->getIContextAudio(), l);});
            this->m_logSignalConnections.insert("audio", con);
        }
        else
        {
            this->disconnectLogSignals("audio");
        }
        return enabled;
    }

    /*
     * Enable signal logging
     */
    bool CRuntime::signalLogForNetwork(bool enabled)
    {
        if (enabled == this->m_signalLogNetwork) return enabled;
        if (!this->getIContextNetwork())
        {
            this->m_signalLogNetwork = false;
            return false;
        }
        this->m_signalLogNetwork = enabled;
        if (enabled)
        {
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextNetwork(), &IContextNetwork::changedAircraftsInRange,
            [this]() { QStringList l; l << "changedAircraftsInRange"; this->logSignal(this->getIContextNetwork(), l);});
            this->m_logSignalConnections.insert("network", con);
            con = QObject::connect(this->getIContextNetwork(), &IContextNetwork::changedAtcStationsBooked,
            [this]() { QStringList l; l << "changedAtcStationsBooked"; this->logSignal(this->getIContextNetwork(), l);});
            this->m_logSignalConnections.insert("network", con);
            con = QObject::connect(this->getIContextNetwork(), &IContextNetwork::changedAtcStationsOnline,
            [this]() { QStringList l; l << "changedAtcStationsOnline"; this->logSignal(this->getIContextNetwork(), l);});
            this->m_logSignalConnections.insert("network", con);
            con = QObject::connect(this->getIContextNetwork(), &IContextNetwork::changedAircraftSituation,
            [this](const BlackMisc::Aviation::CCallsign & callsign, const BlackMisc::Aviation::CAircraftSituation & situation) { QStringList l; l << "changedAircraftSituation" << callsign.toQString() << situation.toQString(); this->logSignal(this->getIContextNetwork(), l);});
            this->m_logSignalConnections.insert("network", con);
            con = QObject::connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged,
            [this](uint from, uint to) { QStringList l; l << "connectionStatusChanged" << QString::number(from) << QString::number(to); this->logSignal(this->getIContextNetwork(), l);});
            this->m_logSignalConnections.insert("network", con);
        }
        else
        {
            this->disconnectLogSignals("network");
        }
        return enabled;
    }

    /*
     * Enable signal logging
     */
    bool CRuntime::signalLogForOwnAircraft(bool enabled)
    {
        if (enabled == this->m_signalLogOwnAircraft) return enabled;
        if (!this->getIContextOwnAircraft())
        {
            this->m_signalLogOwnAircraft = false;
            return false;
        }
        this->m_signalLogOwnAircraft = enabled;
        if (enabled)
        {
            // connect signal / slots when enabled
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftSituation,
            [this](const BlackMisc::Aviation::CAircraft & aircraft, const QString & originator) { QStringList l; l << "changedAircraftSituation" << aircraft.toQString() << originator; this->logSignal(this->getIContextOwnAircraft(), l);});
            this->m_logSignalConnections.insert("ownaircraft", con);

            con = QObject::connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit,
            [this](const BlackMisc::Aviation::CAircraft & aircraft, const QString & originator) { QStringList l; l << "changedAircraftCockpit" << aircraft.toQString() << originator; this->logSignal(this->getIContextOwnAircraft(), l);});
            this->m_logSignalConnections.insert("ownaircraft", con);

            con = QObject::connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftPosition,
            [this](const BlackMisc::Aviation::CAircraft & aircraft, const QString & originator) { QStringList l; l << "changedAircraftPosition" << aircraft.toQString() << originator; this->logSignal(this->getIContextOwnAircraft(), l);});
            this->m_logSignalConnections.insert("ownaircraft", con);
        }
        else
        {
            this->disconnectLogSignals("ownaircraft");
        }
        return enabled;
    }


    /*
     * Enable signal logging
     */
    bool CRuntime::signalLogForSettings(bool enabled)
    {
        if (enabled == this->m_signalLogSettings) return enabled;
        if (!this->getIContextSettings())
        {
            this->m_signalLogSettings = false;
            return false;
        }
        this->m_signalLogSettings = enabled;
        if (enabled)
        {
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextSettings(), &IContextSettings::changedSettings,
            [this]() { QStringList l; l << "changedSettings"; this->logSignal(this->getIContextSettings(), l);});
            this->m_logSignalConnections.insert("settings", con);
        }
        else
        {
            this->disconnectLogSignals("settings");
        }
        return enabled;
    }

    /*
     * Enable signal logging
     */
    bool CRuntime::signalLogForSimulator(bool enabled)
    {
        if (enabled == this->m_signalLogSimulator) return enabled;
        if (!this->getIContextSimulator())
        {
            this->m_signalLogSimulator = false;
            return false;
        }
        this->m_signalLogSimulator = enabled;
        if (enabled)
        {
            QMetaObject::Connection con;
            con = QObject::connect(this->getIContextSimulator(), &IContextSimulator::connectionChanged,
            [this]() { QStringList l; l << "connectionChanged"; this->logSignal(this->getIContextSimulator(), l);});
            this->m_logSignalConnections.insert("simulator", con);
        }
        else
        {
            this->disconnectLogSignals("simulator");
        }
        return enabled;
    }

    void CRuntime::logSignal(QObject *sender, const QStringList &values)
    {
        QString s = (sender) ? sender->metaObject()->className() : "";
        qDebug() << "signal" << s << values;
    }

    void CRuntime::logSlot(const char *func, const QString &p1, const QString &p2, const QString &p3, const QString &p4) const
    {
        if (p1.isEmpty()) { qDebug() << func; return; }
        if (p2.isEmpty()) { qDebug() << func << p1; return; }
        if (p3.isEmpty()) { qDebug() << func << p1 << p2; return; }
        if (p4.isEmpty()) { qDebug() << func << p1 << p2 << p3; return; }
        qDebug() << func << p1 << p2 << p3 << p4;
    }

    void CRuntime::logSlot(const char *func, const QString &param) const
    {
        qDebug() << func << param;
    }

    void CRuntime::logSlot(const char *func, const QStringList &params) const
    {
        qDebug() << func << params;
    }

    void CRuntime::logSlot(const char *func, bool boolValue) const
    {
        qDebug() << func << boolValue;
    }

    void CRuntime::logSlot(CRuntime::LogContext context, const char *func, const QString &param) const
    {
        if (this->isSlotLogEnabledFor(context)) qDebug() << func << param;
    }

    void CRuntime::logSlot(CRuntime::LogContext context, const char *func, const QStringList &params) const
    {
        if (this->isSlotLogEnabledFor(context)) qDebug() << func << params;
    }

    void CRuntime::logSlot(CRuntime::LogContext context, const char *func, bool boolValue) const
    {
        if (this->isSlotLogEnabledFor(context)) qDebug() << func << boolValue;
    }

    bool CRuntime::isSlotLogEnabledFor(CRuntime::LogContext context) const
    {
        switch (context)
        {
        default: return true;
        case LogForApplication: return this->m_slotLogApplication;
        case LogForAudio: return this->m_slotLogAudio;
        case LogForNetwork: return this->m_slotLogNetwork;
        case LogForOwnAircraft: return this->m_slotLogOwnAircraft;
        case LogForSettings: return this->m_slotLogSettings;
        case LogForSimulator: return this->m_slotLogSimulator;
        }
    }

    /*
     * Init runtime
     */
    void CRuntime::init(const CRuntimeConfig &config)
    {
        if (m_init) return;
        BlackMisc::registerMetadata();
        BlackMisc::initResources();
        BlackSim::registerMetadata();
        BlackCore::registerMetadata();

        // upfront reading of settings, as DBus server already relies on settings
        CContextSettings *settings = nullptr;
        QString dbusAddress;
        QMap<QString, int> times;
        QTime time;
        time.start();

        if (config.hasDBusAddress()) dbusAddress = config.getDBusAddress(); // bootstrap / explicit
        if (config.hasLocalSettings())
        {
            settings = new CContextSettings(config.getModeSettings(), this);
            if (settings) settings->read();
            if (dbusAddress.isEmpty()) dbusAddress = settings->getNetworkSettings().getDBusServerAddress();
        }

        // DBus
        if (config.requiresDBusSever()) this->initDBusServer(dbusAddress);
        if (config.requiresDBusConnection())
        {
            this->initDBusConnection(dbusAddress);
            if (!this->m_dbusConnection.isConnected())
            {
                QString notConnected("DBus connection failed");
                QString e = this->m_dbusConnection.lastError().message();
                if (!e.isEmpty()) notConnected.append(" ").append(e);
                Q_ASSERT_X(false, "CRuntime::init", notConnected.toUtf8().constData());
                qCritical() << notConnected;
            }
        }
        times.insert("DBus", time.restart());

        // contexts
        switch (config.getModeSettings())
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            Q_ASSERT(settings);
            this->m_contextSettings = settings->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->m_contextSettings = new BlackCore::CContextSettingsProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeSettings(), this);
            break;
        default:
            qFatal("Always initialize a settings context");
        }
        times.insert("Settings", time.restart());

        switch (config.getModeApplication())
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            this->m_contextApplication = (new CContextApplication(config.getModeApplication(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->m_contextApplication = new BlackCore::CContextApplicationProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeApplication(), this);
            break;
        default:
            qFatal("Always initialize an application context");
        }
        times.insert("Application", time.restart());

        switch (config.getModeOwnAircraft())
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            this->m_contextOwnAircraft = (new CContextOwnAircraft(config.getModeApplication(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->m_contextOwnAircraft = new BlackCore::CContextOwnAircraftProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeOwnAircraft(), this);
            break;
        default:
            qFatal("Always initialize own aircraft context");
        }
        times.insert("Own aircraft", time.restart());

        switch (config.getModeAudio())
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            this->m_contextAudio = (new CContextAudio(config.getModeAudio(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->m_contextAudio = new BlackCore::CContextAudioProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeAudio(), this);
            break;
        default:
            break; // audio not mandatory
        }
        times.insert("Audio", time.restart());

        switch (config.getModeNetwork())
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            this->m_contextNetwork = (new CContextNetwork(config.getModeNetwork(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->m_contextNetwork = new BlackCore::CContextNetworkProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeNetwork(), this);
            break;
        default:
            break; // network not mandatory
        }
        times.insert("Network", time.restart());

        switch (config.getModeSimulator())
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            this->m_contextSimulator = (new CContextSimulator(config.getModeSimulator(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->m_contextSimulator = new BlackCore::CContextSimulatorProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeSimulator(), this);
            break;
        default:
            break; // network not mandatory
        }
        times.insert("Simulator", time.restart());

        // checks --------------
        // 1. own aircraft and simulator should reside in same location
        Q_ASSERT(!this->m_contextSimulator || (this->m_contextOwnAircraft->usingLocalObjects() == this->m_contextSimulator->usingLocalObjects()));

        // 2. own aircraft and network should reside in same location
        Q_ASSERT(!this->m_contextNetwork || (this->m_contextOwnAircraft->usingLocalObjects() == this->m_contextNetwork->usingLocalObjects()));

        // post inits, wiring things among context (e.g. signal slots)
        this->initPostSetup();
        qDebug() << "Init times:" << times;

        // flag
        m_init = true;
    }

    bool CRuntime::hasRemoteApplicationContext() const
    {
        Q_ASSERT(this->m_contextApplication);
        return !this->m_contextApplication->usingLocalObjects();
    }

    bool CRuntime::canPingApplicationContext() const
    {
        Q_ASSERT(this->m_contextApplication);
        if (this->m_contextApplication->usingLocalObjects()) return true;
        qint64 token = QDateTime::currentMSecsSinceEpoch();
        return (token == this->m_contextApplication->ping(token));
    }

    void CRuntime::initDBusServer(const QString &dBusAddress)
    {
        if (this->m_dbusServer) return;
        Q_ASSERT(!dBusAddress.isEmpty());
        this->m_dbusServer = new CDBusServer(dBusAddress, this);
    }

    void CRuntime::initPostSetup()
    {
        bool c;
        Q_UNUSED(c); // for release version
        if (this->m_contextSettings && this->m_contextAudio && this->m_contextSettings->usingLocalObjects())
        {
            // only, when both contexts exists and only if settings originate locally
            c = connect(this->m_contextSettings, &IContextSettings::changedSettings,
                        this->getCContextAudio(), &CContextAudio::settingsChanged);
            Q_ASSERT(c);
        }

        if (this->m_contextSimulator && this->m_contextSimulator->usingLocalObjects() && this->getCContextSimulator()->m_simulator)
        {
            // only connect if simulator running locally, no round trips
            if (this->m_contextNetwork && this->m_contextNetwork->usingLocalObjects())
            {
                c = connect(this->m_contextNetwork, &IContextNetwork::changedAircraftSituation,
                            this->getCContextSimulator(), &CContextSimulator::addAircraftSituation);
                Q_ASSERT(c);

                c = connect(this->m_contextNetwork, &IContextNetwork::textMessagesReceived,
                            this->getCContextSimulator(), &CContextSimulator::textMessagesReceived);
                Q_ASSERT(c);
            }

            // only if own aircraft running locally
            if (this->m_contextOwnAircraft && this->m_contextOwnAircraft->usingLocalObjects())
            {
                c = connect(this->m_contextOwnAircraft, &IContextOwnAircraft::changedAircraftCockpit,
                            this->getCContextSimulator(), &CContextSimulator::updateCockpitFromContext);
                Q_ASSERT(c);
            }

            // only if own aircraft running locally
            if (this->m_contextApplication && this->m_contextApplication->usingLocalObjects())
            {
                c = connect(this->m_contextApplication, &IContextApplication::statusMessage,
                            this->getCContextSimulator(), &CContextSimulator::statusMessageReceived);
                Q_ASSERT(c);

                c = connect(this->m_contextApplication, &IContextApplication::statusMessages,
                            this->getCContextSimulator(), &CContextSimulator::statusMessagesReceived);
                Q_ASSERT(c);
            }
        }

        if (this->m_contextNetwork && this->m_contextOwnAircraft && this->m_contextNetwork->usingLocalObjects() && this->m_contextOwnAircraft->usingLocalObjects())
        {
            // only where network and(!) own aircraft run locally
            c = this->connect(this->m_contextNetwork, &IContextNetwork::changedAtcStationOnlineConnectionStatus,
                              this->getCContextOwnAircraft(),  &CContextOwnAircraft::changedAtcStationOnlineConnectionStatus);
            Q_ASSERT(c);

            // inject updated own aircraft to network
            c = this->connect(this->m_contextOwnAircraft, &IContextOwnAircraft::changedAircraft,
                              this->getCContextNetwork(),  &CContextNetwork::psChangedOwnAircraft);
            Q_ASSERT(c);
        }
    }

    void CRuntime::gracefulShutdown()
    {
        if (!this->m_init) return;
        this->m_init = false;

        // disable all logging and all signals towards runtime
        this->signalLog(false);
        disconnect(this);

        // unregister all from DBus
        if (this->m_dbusServer) this->m_dbusServer->unregisterAllObjects();

        // handle contexts
        if (this->getIContextApplication())
        {
            disconnect(this->getIContextApplication());
            this->getIContextApplication()->setOutputRedirectionLevel(IContextApplication::RedirectNone);
            this->getIContextApplication()->setStreamingForRedirectedOutputLevel(IContextApplication::RedirectNone);
            IContextApplication::resetOutputRedirection();
            this->getIContextApplication()->deleteLater();
        }

        if (this->getIContextOwnAircraft())
        {
            disconnect(this->getIContextOwnAircraft());
            this->getIContextOwnAircraft()->deleteLater();
        }

        if (this->getIContextSimulator())
        {
            // TODO: disconnect from simulator
            disconnect(this->getIContextSimulator());
            this->getIContextSimulator()->deleteLater();
        }

        // log off from network, if connected
        if (this->getIContextNetwork())
        {
            disconnect(this->getIContextNetwork());
            this->getIContextNetwork()->disconnectFromNetwork();
            this->getIContextNetwork()->deleteLater();
        }

        if (this->getIContextAudio())
        {
            disconnect(this->getIContextAudio());
            this->getIContextAudio()->deleteLater();
        }

        if (this->getIContextSettings())
        {
            disconnect(this->getIContextSettings());
            this->getIContextSettings()->deleteLater();
        }

        // mark contexts as invalid
        // objects are already scheduled for deletion
        this->m_contextApplication = nullptr;
        this->m_contextAudio = nullptr;
        this->m_contextNetwork = nullptr;
        this->m_contextOwnAircraft = nullptr;
        this->m_contextSettings = nullptr;
        this->m_contextSimulator = nullptr;
    }

    void CRuntime::disconnectLogSignals(const QString &name)
    {
        if (!this->m_logSignalConnections.contains(name)) return;
        QMultiMap<QString, QMetaObject::Connection>::const_iterator i = this->m_logSignalConnections.constFind(name);
        while (i != this->m_logSignalConnections.end() && i.key() == name)
        {
            disconnect(i.value());
            ++i;
        }
        this->m_logSignalConnections.remove(name);
    }

    void CRuntime::initDBusConnection(const QString &address)
    {
        if (this->m_initDBusConnection) return;
        if (address.isEmpty() || address == CDBusServer::sessionDBusServer())
            this->m_dbusConnection = QDBusConnection::sessionBus();
        else if (address == CDBusServer::systemDBusServer())
            this->m_dbusConnection = QDBusConnection::sessionBus();
        else
            this->m_dbusConnection = QDBusConnection::connectToPeer(address, "BlackBoxRuntime");
    }

    const IContextApplication *CRuntime::getIContextApplication() const
    {
        return this->m_contextApplication;
    }

    IContextApplication *CRuntime::getIContextApplication()
    {
        return this->m_contextApplication;
    }

    IContextAudio *CRuntime::getIContextAudio()
    {
        return this->m_contextAudio;
    }

    const IContextAudio *CRuntime::getIContextAudio() const
    {
        return this->m_contextAudio;
    }

    IContextNetwork *CRuntime::getIContextNetwork()
    {
        return this->m_contextNetwork;
    }

    const IContextNetwork *CRuntime::getIContextNetwork() const
    {
        return this->m_contextNetwork;
    }

    IContextOwnAircraft *CRuntime::getIContextOwnAircraft()
    {
        return this->m_contextOwnAircraft;
    }

    const IContextOwnAircraft *CRuntime::getIContextOwnAircraft() const
    {
        return this->m_contextOwnAircraft;
    }

    IContextSettings *CRuntime::getIContextSettings()
    {
        return this->m_contextSettings;
    }

    const IContextSettings *CRuntime::getIContextSettings() const
    {
        return this->m_contextSettings;
    }

    const IContextSimulator *CRuntime::getIContextSimulator() const
    {
        return this->m_contextSimulator;
    }

    IContextSimulator *CRuntime::getIContextSimulator()
    {
        return this->m_contextSimulator;
    }

    CContextAudio *CRuntime::getCContextAudio()
    {
        Q_ASSERT_X(!this->m_contextAudio || this->m_contextAudio->usingLocalObjects(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextAudio *>(this->m_contextAudio);
    }

    const CContextAudio *CRuntime::getCContextAudio() const
    {
        Q_ASSERT_X(!this->m_contextAudio || this->m_contextAudio->usingLocalObjects(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextAudio *>(this->m_contextAudio);
    }

    CContextApplication *CRuntime::getCContextApplication()
    {
        Q_ASSERT_X(!this->m_contextApplication || this->m_contextApplication->usingLocalObjects(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextApplication *>(this->m_contextApplication);
    }

    const CContextApplication *CRuntime::getCContextApplication() const
    {
        Q_ASSERT_X(!this->m_contextApplication || this->m_contextApplication->usingLocalObjects(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextApplication *>(this->m_contextApplication);
    }

    CContextNetwork *CRuntime::getCContextNetwork()
    {
        Q_ASSERT_X(!this->m_contextApplication || this->m_contextNetwork->usingLocalObjects(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextNetwork *>(this->m_contextNetwork);
    }

    const CContextNetwork *CRuntime::getCContextNetwork() const
    {
        Q_ASSERT_X(!this->m_contextApplication || this->m_contextNetwork->usingLocalObjects(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextNetwork *>(this->m_contextNetwork);
    }



    CContextOwnAircraft *CRuntime::getCContextOwnAircraft()
    {
        Q_ASSERT_X(!this->m_contextOwnAircraft || this->m_contextOwnAircraft->usingLocalObjects(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextOwnAircraft *>(this->m_contextOwnAircraft);
    }

    const CContextOwnAircraft *CRuntime::getCContextOwnAircraft() const
    {
        Q_ASSERT_X(!this->m_contextOwnAircraft || this->m_contextOwnAircraft->usingLocalObjects(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextOwnAircraft *>(this->m_contextOwnAircraft);
    }

    CContextSimulator *CRuntime::getCContextSimulator()
    {
        Q_ASSERT_X(!this->m_contextSimulator || this->m_contextSimulator->usingLocalObjects(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextSimulator *>(this->m_contextSimulator);
    }

    const CContextSimulator *CRuntime::getCContextSimulator() const
    {
        Q_ASSERT_X(!this->m_contextSimulator || this->m_contextSimulator->usingLocalObjects(), "CCoreRuntime", "Cannot downcast to local object");
        return static_cast<CContextSimulator *>(this->m_contextSimulator);
    }

    bool CRuntimeConfig::requiresDBusSever() const
    {
        return (this->m_application == LocalInDbusServer ||
                this->m_audio == LocalInDbusServer ||
                this->m_network == LocalInDbusServer ||
                this->m_ownAircraft == LocalInDbusServer ||
                this->m_settings == LocalInDbusServer ||
                this->m_simulator == LocalInDbusServer);
    }

    bool CRuntimeConfig::requiresDBusConnection() const
    {
        return (this->m_application == Remote ||
                this->m_audio == Remote ||
                this->m_network == Remote ||
                this->m_ownAircraft == Remote ||
                this->m_settings == Remote ||
                this->m_simulator == Remote);
    }

    const CRuntimeConfig &CRuntimeConfig::forCoreAllLocalInDBus(const QString &dbusBootstrapAddress)
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::LocalInDbusServer, dbusBootstrapAddress));
        return cfg;
    }

    const CRuntimeConfig &CRuntimeConfig::forCoreAllLocalInDBusNoAudio(const QString &dbusBootstrapAddress)
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::LocalInDbusServer, dbusBootstrapAddress));
        cfg.m_audio = CRuntimeConfig::NotUsed;
        return cfg;
    }

    const CRuntimeConfig &CRuntimeConfig::local(const QString &dbusBootstrapAddress)
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::Local, dbusBootstrapAddress));
        return cfg;
    }

    const CRuntimeConfig &CRuntimeConfig::remote(const QString &dbusBootstrapAddress)
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::Remote, dbusBootstrapAddress));
        return cfg;
    }

    const CRuntimeConfig &CRuntimeConfig::remoteLocalAudio(const QString &dbusBootstrapAddress)
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::Remote, dbusBootstrapAddress));
        cfg.m_audio = CRuntimeConfig::Local;
        return cfg;
    }
}
