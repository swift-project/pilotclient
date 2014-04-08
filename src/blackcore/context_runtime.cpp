#include "blackcore/context_runtime.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/nwserver.h"

#include "blackcore/context_application_impl.h"
#include "blackcore/context_network_impl.h"
#include "blackcore/context_settings_impl.h"
#include "blackcore/context_audio_impl.h"
#include "blackcore/context_simulator_impl.h"

#include "blackcore/context_application_proxy.h"
#include "blackcore/context_network_proxy.h"
#include "blackcore/context_settings_proxy.h"
#include "blackcore/context_audio_proxy.h"
#include "blackcore/context_simulator_proxy.h"


namespace BlackCore
{
    /*
     * Constructor
     */
    CRuntime::CRuntime(const CRuntimeConfig &config, QObject *parent) :
        QObject(parent), m_init(false), m_dbusServer(nullptr), m_initDBusConnection(false), m_dbusConnection(QDBusConnection("default")),
        m_contextNetwork(nullptr), m_contextAudio(nullptr),
        m_contextSettings(nullptr), m_contextApplication(nullptr),
        m_contextSimulator(nullptr)
    {
        this->init(config);
    }

    /*
     * Init runtime
     */
    void CRuntime::init(const CRuntimeConfig &config)
    {
        if (m_init) return;
        BlackMisc::registerMetadata();
        BlackMisc::initResources();

        // upfront reading of settings, as DBus server alread relies on
        // settings
        CContextSettings *settings = nullptr;
        QString dbusServerAddress;
        if (config.hasLocalSettings())
        {
            settings = new CContextSettings(config.getModeSettings(), this);
            if (settings) settings->read();
            dbusServerAddress = settings->getNetworkSettings().getDBusServerAddress();
        }

        // contexts
        switch (config.getModeSettings())
        {
        case CRuntimeConfig::Local:
            this->m_contextSettings = settings;
            break;
        case CRuntimeConfig::LocalInDbusServer:
            this->initDBusServer(dbusServerAddress);
            this->m_contextSettings = settings->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->initDBusConnection();
            this->m_contextSettings = new BlackCore::CContextSettingsProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeSettings(), this);
            break;
        default:
            break;
        }

        switch (config.getModeApplication())
        {
        case CRuntimeConfig::Local:
            this->m_contextApplication = new CContextApplication(config.getModeApplication(), this);
            break;
        case CRuntimeConfig::LocalInDbusServer:
            this->initDBusServer(dbusServerAddress);
            this->m_contextApplication = (new CContextApplication(config.getModeApplication(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->initDBusConnection();
            this->m_contextApplication = new BlackCore::CContextApplicationProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeApplication(), this);
            break;
        default:
            break;
        }

        switch (config.getModeAudio())
        {
        case CRuntimeConfig::Local:
            this->m_contextAudio = new CContextAudio(config.getModeAudio(), this);
            break;
        case CRuntimeConfig::LocalInDbusServer:
            this->initDBusServer(dbusServerAddress);
            this->m_contextAudio = (new CContextAudio(config.getModeAudio(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->initDBusConnection();
            this->m_contextAudio = new BlackCore::CContextAudioProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeAudio(), this);
            break;
        default:
            break;
        }

        switch (config.getModeNetwork())
        {
        case CRuntimeConfig::Local:
            this->m_contextNetwork = new CContextNetwork(config.getModeNetwork(), this);
            break;
        case CRuntimeConfig::LocalInDbusServer:
            this->initDBusServer(dbusServerAddress);
            this->m_contextNetwork = (new CContextNetwork(config.getModeNetwork(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->initDBusConnection();
            this->m_contextNetwork = new BlackCore::CContextNetworkProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeNetwork(), this);
            break;
        default:
            break;
        }

        switch (config.getModeSimulator())
        {
        case CRuntimeConfig::Local:
            this->m_contextSimulator = new CContextSimulator(config.getModeSimulator(), this);
            break;
        case CRuntimeConfig::LocalInDbusServer:
            this->initDBusServer(dbusServerAddress);
            this->m_contextSimulator = (new CContextSimulator(config.getModeSimulator(), this))->registerWithDBus(this->m_dbusServer);
            break;
        case CRuntimeConfig::Remote:
            this->initDBusConnection();
            this->m_contextSimulator = new BlackCore::CContextSimulatorProxy(BlackCore::CDBusServer::ServiceName, this->m_dbusConnection, config.getModeSimulator(), this);
            break;
        default:
            break;
        }

        // post inits, wiring things among context (e.g. signal slots)
        this->initPostSetup();

        // flag
        m_init = true;
    }

    void CRuntime::initDBusServer(const QString &dBusAddress)
    {
        if (this->m_dbusServer) return;
        Q_ASSERT(!dBusAddress.isEmpty());
        this->m_dbusServer = new CDBusServer(dBusAddress, this);
    }

    void CRuntime::initPostSetup()
    {
        if (this->m_contextSettings && this->m_contextAudio && this->m_contextSettings->usingLocalObjects())
        {
            // only, when both contexts exists and only if settings originate locally
            connect(this->m_contextSettings, &IContextSettings::changedSettings,
                    this->getCContextAudio(), &CContextAudio::settingsChanged);
        }

        if (this->m_contextSimulator && this->m_contextSimulator->usingLocalObjects() && this->m_contextNetwork)
        {
            if (this->getCContextSimulator()->m_simulator)
            {
                connect(this->m_contextNetwork, SIGNAL(aircraftSituationUpdate(BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituation)),
                        this->getCContextSimulator()->m_simulator, SLOT(addAircraftSituation(BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituation)));
            }
        }
    }

    void CRuntime::initDBusConnection()
    {
        if (this->m_initDBusConnection) return;
        this->m_dbusConnection = QDBusConnection::sessionBus();
    }

    IContextNetwork *CRuntime::getIContextNetwork()
    {
        return this->m_contextNetwork;
    }

    const IContextNetwork *CRuntime::getIContextNetwork() const
    {
        return this->m_contextNetwork;
    }

    IContextAudio *CRuntime::getIContextAudio()
    {
        return this->m_contextAudio;
    }

    const IContextAudio *CRuntime::getIContextAudio() const
    {
        return this->m_contextAudio;
    }

    IContextSettings *CRuntime::getIContextSettings()
    {
        return this->m_contextSettings;
    }

    const IContextSettings *CRuntime::getIContextSettings() const
    {
        return this->m_contextSettings;
    }

    const IContextApplication *CRuntime::getIContextApplication() const
    {
        return this->m_contextApplication;
    }

    IContextApplication *CRuntime::getIContextApplication()
    {
        return this->m_contextApplication;
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
        return static_cast<CContextAudio *>(this->m_contextAudio);
    }

    CContextAudio *CRuntime::getCContextAudio() const
    {
        return static_cast<CContextAudio *>(this->m_contextAudio);
    }

    CContextApplication *CRuntime::getCContextApplication()
    {
        return static_cast<CContextApplication *>(this->m_contextApplication);
    }

    CContextApplication *CRuntime::getCContextApplication() const
    {
        return static_cast<CContextApplication *>(this->m_contextApplication);
    }

    CContextSimulator *CRuntime::getCContextSimulator()
    {
        return static_cast<CContextSimulator *>(this->m_contextSimulator);
    }

    CContextSimulator *CRuntime::getCContextSimulator() const
    {
        return static_cast<CContextSimulator *>(this->m_contextSimulator);
    }

    bool CRuntimeConfig::requiresDBusSever() const
    {
        return (this->m_application == LocalInDbusServer ||
                this->m_audio == LocalInDbusServer ||
                this->m_network == LocalInDbusServer ||
                this->m_settings == LocalInDbusServer ||
                this->m_simulator == LocalInDbusServer);
    }

    bool CRuntimeConfig::requiresDBusConnection() const
    {
        return (this->m_application == Remote ||
                this->m_audio == Remote ||
                this->m_network == Remote ||
                this->m_settings == Remote ||
                this->m_simulator == Remote);
    }

    const CRuntimeConfig &CRuntimeConfig::forCore()
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::LocalInDbusServer));
        return cfg;
    }

    const CRuntimeConfig &CRuntimeConfig::local()
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::Local));
        return cfg;
    }

    const CRuntimeConfig &CRuntimeConfig::remote()
    {
        static CRuntimeConfig cfg = CRuntimeConfig(CRuntimeConfig(CRuntimeConfig::Remote));
        return cfg;
    }
}
