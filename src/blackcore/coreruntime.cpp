#include "blackcore/coreruntime.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/nwserver.h"
#include "blackcore/context_application_impl.h"
#include "blackcore/context_network_impl.h"
#include "blackcore/context_settings_impl.h"
#include "blackcore/context_audio_impl.h"
#include "blackcore/context_simulator_impl.h"

namespace BlackCore
{
/*
 * Constructor
 */
CCoreRuntime::CCoreRuntime(bool withDbus, QObject *parent) :
    QObject(parent), m_init(false), m_dbusServer(nullptr),
    m_contextNetwork(nullptr), m_contextAudio(nullptr),
    m_contextSettings(nullptr), m_contextApplication(nullptr),
    m_contextSimulator(nullptr)
{
    this->init(withDbus);
}

/*
 * Init runtime
 */
void CCoreRuntime::init(bool withDbus)
{
    if (m_init) return;
    BlackMisc::registerMetadata();
    BlackMisc::initResources();

    // TODO: read settings
    if (withDbus) {
        QString dBusAddress = "session";
        this->m_dbusServer = new CDBusServer(dBusAddress, this);
    }

    // contexts
    this->m_contextSettings = new CContextSettings(this);
    if (withDbus) this->m_contextSettings->registerWithDBus(this->m_dbusServer);

    this->m_contextNetwork = new CContextNetwork(this);
    if (withDbus) this->m_contextNetwork->registerWithDBus(this->m_dbusServer);

    this->m_contextApplication = new CContextApplication(this);
    if (withDbus) this->m_contextApplication->registerWithDBus(this->m_dbusServer);

    this->m_contextAudio = new CContextAudio(this);
    if (withDbus) this->m_contextAudio->registerWithDBus(this->m_dbusServer);

    this->m_contextSimulator = new CContextSimulator(this);
    if (withDbus) this->m_contextSimulator->registerWithDBus(this->m_dbusServer);

    // flag
    m_init = true;
}

IContextNetwork *CCoreRuntime::getIContextNetwork()
{
    return this->m_contextNetwork;
}

const IContextNetwork *CCoreRuntime::getIContextNetwork() const
{
    return this->m_contextNetwork;
}

IContextAudio *CCoreRuntime::getIContextAudio()
{
    return this->m_contextAudio;
}

const IContextAudio *CCoreRuntime::getIContextAudio() const
{
    return this->m_contextAudio;
}

IContextSettings *CCoreRuntime::getIContextSettings()
{
    return this->m_contextSettings;
}

const IContextSettings *CCoreRuntime::getIContextSettings() const
{
    return this->m_contextSettings;
}

const IContextApplication *CCoreRuntime::getIContextApplication() const
{
    return this->m_contextApplication;
}

IContextApplication *CCoreRuntime::getIContextApplication()
{
    return this->m_contextApplication;
}

const IContextSimulator *CCoreRuntime::getIContextSimulator() const
{
    return this->m_contextSimulator;
}

IContextSimulator *CCoreRuntime::getIContextSimulator()
{
    return this->m_contextSimulator;
}
}
