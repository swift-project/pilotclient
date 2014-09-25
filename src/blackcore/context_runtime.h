#ifndef BLACKCORE_CONTEXT_RUNTIME_H
#define BLACKCORE_CONTEXT_RUNTIME_H

#include "blackcore/context_runtime_config.h"
#include "blackmisc/statusmessagelist.h"
#include <QDBusConnection>
#include <QObject>
#include <QMultiMap>
#include <QReadWriteLock>

using namespace BlackMisc;

namespace BlackCore
{
    // forward declaration, see review
    // https://dev.vatsim-germany.org/boards/22/topics/1350?r=1359#message-1359
    class CDBusServer;
    class CContextApplication;
    class CContextAudio;
    class CContextNetwork;
    class CContextOwnAircraft;
    class CContextSettings;
    class CContextSimulator;
    class IContextApplication;
    class IContextAudio;
    class IContextNetwork;
    class IContextOwnAircraft;
    class IContextSettings;
    class IContextSimulator;

    //! The Context runtime class
    class CRuntime : public QObject
    {
        Q_OBJECT

    public:
        //! Log message category
        static QString getMessageCategory() { return "swift.runtime"; }

        //! Constructor
        CRuntime(const CRuntimeConfig &config, QObject *parent = nullptr);

        //! Destructor
        virtual ~CRuntime() { this->gracefulShutdown(); }

        //! DBus server (if applicable)
        const CDBusServer *getDBusServer() const { return this->m_dbusServer; }

        //! DBus connection (if applicable)
        const QDBusConnection &getDBusConnection() const { return this->m_dbusConnection; }

        //! Clean up (will be connected to signal QCoreApplication::aboutToQuit)
        void gracefulShutdown();

        //! \copydoc IContextApplication::sendStatusMessage
        //! \remarks fail safe
        void sendStatusMessage(const BlackMisc::CStatusMessage &message);

        //! \copydoc IContextApplication::sendStatusMessage
        //! \remarks fail safe
        void sendStatusMessages(const BlackMisc::CStatusMessageList &messages);

        // ------- Context as interface, normal way to access a context

        //! Context for network
        IContextNetwork *getIContextNetwork();

        //! Context for network
        const IContextNetwork *getIContextNetwork() const;

        //! Context for audio
        IContextAudio *getIContextAudio();

        //! Context for audio
        const IContextAudio *getIContextAudio() const;

        //! Context for application
        IContextApplication *getIContextApplication();

        //! Context for application
        const IContextApplication *getIContextApplication() const;

        //! Context for own aircraft
        IContextOwnAircraft *getIContextOwnAircraft();

        //! Context for own aircraft
        const IContextOwnAircraft *getIContextOwnAircraft() const;

        //! Context for settings
        IContextSettings *getIContextSettings();

        //! Context for settings
        const IContextSettings *getIContextSettings() const;

        //! Context for simulator
        IContextSimulator *getIContextSimulator();

        //! Context for simulator
        const IContextSimulator *getIContextSimulator() const;

        // ------- Context as implementing (local) class

        //! Context for application
        //! \remarks only applicable for local object
        CContextApplication *getCContextApplication();

        //! Context for application
        //! \remarks only applicable for local object
        const CContextApplication *getCContextApplication() const;

        //! Context for audio
        //! \remarks only applicable for local object
        CContextAudio *getCContextAudio();

        //! Context for audio
        //! \remarks only applicable for local object
        const CContextAudio *getCContextAudio() const;

        //! Context for own aircraft
        //! \remarks only applicable for local object
        CContextOwnAircraft *getCContextOwnAircraft();

        //! Context for own aircraft
        //! \remarks only applicable for local object
        const CContextOwnAircraft *getCContextOwnAircraft() const;

        //! Context for network
        //! \remarks only applicable for local object
        CContextNetwork *getCContextNetwork();

        //! Context for network
        //! \remarks only applicable for local object
        const CContextNetwork *getCContextNetwork() const;

        //! Context for simulator
        //! \remarks only applicable for local object
        CContextSimulator *getCContextSimulator();

        //! Context for simulator
        //! \remarks only applicable for local object
        const CContextSimulator *getCContextSimulator() const;

        //! Init
        void init(const CRuntimeConfig &config);

        //! Remote application context, indicates distributed environment
        bool hasRemoteApplicationContext() const;

        //! Is application context available?
        bool canPingApplicationContext() const;

    private:
        bool m_init; /*!< flag */
        CDBusServer *m_dbusServer;
        bool m_initDBusConnection;
        QDBusConnection m_dbusConnection;
        IContextApplication *m_contextApplication;
        IContextAudio *m_contextAudio;
        IContextNetwork *m_contextNetwork;
        IContextOwnAircraft *m_contextOwnAircraft;
        IContextSettings *m_contextSettings;
        IContextSimulator *m_contextSimulator;
        mutable QReadWriteLock m_lock;

        //! initialization of DBus connection (where applicable)
        void initDBusConnection(const QString &address);

        //! initialization of DBus connection (where applicable)
        void initDBusServer(const QString &dBusAddress);

        //! post init tasks, mainly connecting context signal slots
        void initPostSetup();
    };
}
#endif // guard
