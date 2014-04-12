#ifndef BLACKCORE_CONTEXT_RUNTIME_H
#define BLACKCORE_CONTEXT_RUNTIME_H

#include "blackcore/context_runtime_config.h"
#include <QDBusConnection>
#include <QObject>
#include <QMultiMap>

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

    public:
        //! Constructor
        CRuntime(const CRuntimeConfig &config, QObject *parent = nullptr);

        //! Destructor
        virtual ~CRuntime() { this->gracefulShutdown(); }

        //! DBus server (if applicable)
        const CDBusServer *getDBusServer() const { return this->m_dbusServer; }

        //! DBus connection (if applicable)
        const QDBusConnection &getDBusConnection() const { return this->m_dbusConnection; }

        //! Enable / disable all logging
        void signalLog(bool enabled);

        //! Signal logging for application context
        bool signalLogForApplication(bool enabled);

        //! Signal logging for audio context
        bool signalLogForAudio(bool enabled);

        //! Signal logging for network context
        bool signalLogForNetwork(bool enabled);

        //! Signal logging for settings context
        bool signalLogForSettings(bool enabled);

        //! Signal logging for simulator context
        bool signalLogForSimulator(bool enabled);

        //! Enable / disable all logging
        void slotLog(bool enabled);

        //! Signal logging for application context
        void slotLogForApplication(bool enabled) { this->m_slotLogApplication = enabled; }

        //! Signal logging for audio context
        void slotLogForAudio(bool enabled) { this->m_slotLogAudio = enabled; }

        //! Signal logging for network context
        void slotLogForNetwork(bool enabled)  { this->m_slotLogNetwork = enabled; }

        //! Signal logging for settings context
        void slotLogForSettings(bool enabled)  { this->m_slotLogSettings = enabled; }

        //! Signal logging for simulator context
        void slotLogForSimulator(bool enabled)  { this->m_slotLogSimulator = enabled; }

        //! Signal logging for application context
        bool isSlotLogForApplicationEnabled() const { return this->m_slotLogApplication; }

        //! Signal logging for audio context
        bool isSlotLogForAudioEnabled() const { return this->m_slotLogAudio; }

        //! Signal logging for network context
        bool isSlotLogForNetworkEnabled() const  { return this->m_slotLogNetwork; }

        //! Signal logging for settings context
        bool isSlotLogForSettingsEnabled() const { return this->m_slotLogSettings; }

        //! Signal logging for simulator context
        bool isSlotLogForSimulatorEnabled() const { return this->m_slotLogSimulator; }

        //! Slot logging
        //! \todo to be replace if initializer lists becomes available
        void logSlot(const char *func, const QString &p1 = "", const QString &p2 = "", const QString &p3 = "", const QString &p4 = "") const;

        //! Slot logging for bool value
        void logSlot(const char *func, bool boolValue) const;

        //! shutdown
        void gracefulShutdown();

        // ------- Context as interface, normal way to access a context

        //! Context for network
        IContextNetwork *getIContextNetwork();

        //! Context for network
        const IContextNetwork *getIContextNetwork() const;

        //! Context for audio
        IContextAudio *getIContextAudio();

        //! Context for audio
        const IContextAudio *getIContextAudio() const;

        //! Context for settings
        IContextSettings *getIContextSettings();

        //! Context for settings
        const IContextSettings *getIContextSettings() const;

        //! Context for application
        const IContextApplication *getIContextApplication() const;

        //! Context for application
        IContextApplication *getIContextApplication();

        //! Context for simulator
        const IContextSimulator *getIContextSimulator() const;

        //! Context for simulator
        IContextSimulator *getIContextSimulator();

        // ------- Context as implementing (local) class

        //! Context for application
        //! \remarks only applicable for local object
        CContextApplication *getCContextApplication();

        //! Context for application
        //! \remarks only applicable for local object
        CContextApplication *getCContextApplication() const;

        //! Context for audio
        //! \remarks only applicable for local object
        CContextAudio *getCContextAudio();

        //! Context for audio
        //! \remarks only applicable for local object
        CContextAudio *getCContextAudio() const;

        //! Context for simulator
        //! \remarks only applicable for local object
        CContextSimulator *getCContextSimulator();

        //! Context for simulator
        //! \remarks only applicable for local object
        CContextSimulator *getCContextSimulator() const;

        //! Init
        void init(const CRuntimeConfig &config);

    private:
        bool m_init; /*!< flag */
        CDBusServer *m_dbusServer;
        bool m_initDBusConnection;
        bool m_signalLogApplication;
        bool m_signalLogAudio;
        bool m_signalLogNetwork;
        bool m_signalLogSettings;
        bool m_signalLogSimulator;
        bool m_slotLogApplication;
        bool m_slotLogAudio;
        bool m_slotLogNetwork;
        bool m_slotLogSettings;
        bool m_slotLogSimulator;
        QDBusConnection m_dbusConnection;
        IContextApplication *m_contextApplication;
        IContextAudio *m_contextAudio;
        IContextNetwork *m_contextNetwork;
        IContextSettings *m_contextSettings;
        IContextSimulator *m_contextSimulator;
        QMultiMap<QString, QMetaObject::Connection> m_logSignalConnections;

        //! initialization of DBus connection (where applicable)
        void initDBusConnection(const QString &address);

        //! initialization of DBus connection (where applicable)
        void initDBusServer(const QString &dBusAddress);

        //! post init tasks, mainly connecting context signal slots
        void initPostSetup();

        //! log
        void logSignal(QObject *sender, const QStringList &values);

        //! disconnect log signals (connected via connection to log them)
        void disconnectLogSignals(const QString &name);
    };
}
#endif // guard
