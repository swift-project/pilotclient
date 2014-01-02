#ifndef BLACKCORE_CORERUNTIME_H
#define BLACKCORE_CORERUNTIME_H

#include <QObject>
#include "dbus_server.h"
#include "context_network.h"
#include "context_settings.h"

namespace BlackCore
{
    /*!
     * \brief The CCoreRuntime class
     */
    class CCoreRuntime : public QObject
    {
        Q_OBJECT

    private:
        bool m_init; /*!< flag */
        CDBusServer *m_dbusServer;
        CContextNetwork *m_contextNetwork;
        CContextSettings *m_settings;

        /*!
         * \brief Init
         * \param withDbus
         */
        void init(bool withDbus);

    public:
        /*!
         * \brief Constructor
         * \param withDbus
         * \param parent
         */
        CCoreRuntime(bool withDbus = true, QObject *parent = nullptr);

        /*!
         * \brief Destructor
         */
        virtual ~CCoreRuntime() {}

        /*!
         * \brief DBus server
         * \return
         */
        const CDBusServer *getDBusServer() const
        {
            return this->m_dbusServer;
        }

        /*!
         * \brief Context for network
         * \return
         */
        IContextNetwork *getIContextNetwork()
        {
            return this->m_contextNetwork;
        }

        /*!
         * \brief Context for network
         * \return
         */
        const IContextNetwork *getIContextNetwork() const
        {
            return this->m_contextNetwork;
        }

        /*!
         * \brief Settings
         * \return
         */
        IContextSettings *getIContextSettings()
        {
            return this->m_settings;
        }

        /*!
         * \brief Settings
         * \return
         */
        const IContextSettings *getIContextSettings() const
        {
            return this->m_settings;
        }

    };
}
#endif // guard
