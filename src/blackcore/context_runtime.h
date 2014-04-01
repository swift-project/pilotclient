#ifndef BLACKCORE_CONTEXT_RUNTIME_H
#define BLACKCORE_CONTEXT_RUNTIME_H

#include <QDBusConnection>
#include <QObject>
#include "blackcore/context_runtime_config.h"

namespace BlackCore
{
    // forward declaration, see review
    // https://dev.vatsim-germany.org/boards/22/topics/1350?r=1359#message-1359
    class CDBusServer;
    class CContextNetwork;
    class CContextAudio;
    class CContextSettings;
    class CContextApplication;
    class CContextSimulator;
    class IContextNetwork;
    class IContextAudio;
    class IContextSettings;
    class IContextApplication;
    class IContextSimulator;

    //! The Context runtime class
    class CRuntime : public QObject
    {
        Q_OBJECT

    private:
        bool m_init; /*!< flag */
        CDBusServer *m_dbusServer;
        bool m_initDBusConnection;
        QDBusConnection m_dbusConnection;
        IContextNetwork *m_contextNetwork;
        IContextAudio *m_contextAudio;
        IContextSettings *m_contextSettings;
        IContextApplication *m_contextApplication;
        IContextSimulator *m_contextSimulator;

        //! Init
        void init(const CRuntimeConfig &config);

        //! initialization of DBus connection (where applicable)
        void initDBusConnection();

        //! initialization of DBus connection (where applicable)
        void initDBusServer(const QString &dBusAddress);

        //! post init tasks, mainly connecting context signal slots
        void initPostSetup();

    public:
        //! Constructor
        CRuntime(const CRuntimeConfig &config, QObject *parent = nullptr);

        //! Destructor
        virtual ~CRuntime() {}

        //! DBus server
        const CDBusServer *getDBusServer() const
        {
            return this->m_dbusServer;
        }

        //! Context for network
        IContextNetwork *getIContextNetwork();

        //! Context for network
        const IContextNetwork *getIContextNetwork() const;

        //! Context for network
        IContextAudio *getIContextAudio();

        //! Context for network
        const IContextAudio *getIContextAudio() const;

        //! Settings
        IContextSettings *getIContextSettings();

        //! Settings
        const IContextSettings *getIContextSettings() const;

        //! Context for application
        const IContextApplication *getIContextApplication() const;

        //! Application
        IContextApplication *getIContextApplication();

        //! Context for simulator
        const IContextSimulator *getIContextSimulator() const;

        //! Simulator
        IContextSimulator *getIContextSimulator();

    private:
        CContextAudio *getCContextAudio();
        CContextAudio *getCContextAudio() const;
    };
}
#endif // guard
