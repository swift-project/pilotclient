#include "blackcore/coreruntime.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/nwserver.h"
#include "blackcore/context_application.h"
#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_voice.h"

namespace BlackCore
{
    /*
     * Constructor
     */
    CCoreRuntime::CCoreRuntime(bool withDbus, QObject *parent) :
        QObject(parent), m_init(false), m_dbusServer(nullptr), m_contextNetwork(nullptr), m_settings(nullptr)
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
        if (withDbus)
        {
            QString dBusAddress = "session";
            this->m_dbusServer = new CDBusServer(dBusAddress, this);
        }

        // contexts
        this->m_settings = new CContextSettings(this);
        if (withDbus) this->m_settings->registerWithDBus(this->m_dbusServer);

        this->m_contextNetwork = new CContextNetwork(this);
        if (withDbus) this->m_contextNetwork->registerWithDBus(this->m_dbusServer); // complete object after init

        // flag
        m_init = true;
    }

    IContextNetwork *CCoreRuntime::getIContextNetwork() { return this->m_contextNetwork; }

    const IContextNetwork *CCoreRuntime::getIContextNetwork() const { return this->m_contextNetwork; }

    IContextVoice *CCoreRuntime::getIContextVoice() { return this->m_contextVoice; }

    const IContextVoice *CCoreRuntime::getIContextVoice() const { return this->m_contextVoice; }

    IContextSettings *CCoreRuntime::getIContextSettings() { return this->m_contextSettings; }

    const IContextSettings *CCoreRuntime::getIContextSettings() const { return this->m_contextSettings; }

    const IContextApplication *CCoreRuntime::getIContextApplication() const { return this->m_contextApplication; }

    IContextApplication *CCoreRuntime::getIContextApplication() { return this->m_contextApplication; }
}
