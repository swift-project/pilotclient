#include "blackcore/coreruntime.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/nwserver.h"

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
}
