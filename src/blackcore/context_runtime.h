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
        //! Tags to identify the context when logging a slot
        enum LogContext
        {
            LogDefault = 0,
            LogForApplication,
            LogForAudio,
            LogForNetwork,
            LogForOwnAircraft,
            LogForSettings,
            LogForSimulator
        };

        //! Constructor
        CRuntime(const CRuntimeConfig &config, QObject *parent = nullptr);

        //! Destructor
        virtual ~CRuntime() { this->gracefulShutdown(); }

        //! DBus server (if applicable)
        const CDBusServer *getDBusServer() const { return this->m_dbusServer; }

        //! DBus connection (if applicable)
        const QDBusConnection &getDBusConnection() const { return this->m_dbusConnection; }

        //! Enable / disable all logging
        //! \remarks thread safe
        void signalLog(bool enabled);

        //! Signal logging for application context
        //! \remarks thread safe
        bool signalLogForApplication(bool enabled);

        //! Signal logging for audio context
        //! \remarks thread safe
        bool signalLogForAudio(bool enabled);

        //! Signal logging for network context
        //! \remarks thread safe
        bool signalLogForNetwork(bool enabled);

        //! Signal logging for own aircraft context
        //! \remarks thread safe
        bool signalLogForOwnAircraft(bool enabled);

        //! Signal logging for settings context
        //! \remarks thread safe
        bool signalLogForSettings(bool enabled);

        //! Signal logging for simulator context
        //! \remarks thread safe
        bool signalLogForSimulator(bool enabled);

        //! Enable / disable all logging
        //! \remarks thread safe
        void slotLog(bool enabled);

        //! Slot logging for application context
        //! \remarks thread safe
        void slotLogForApplication(bool enabled)
        {
            QWriteLocker wl(&m_lock);
            this->m_slotLogApplication = enabled;
        }

        //! Slot logging for audio context
        //! \remarks thread safe
        void slotLogForAudio(bool enabled)
        {
            QWriteLocker wl(&m_lock);
            this->m_slotLogAudio = enabled;
        }

        //! Slot logging for network context
        //! \remarks thread safe
        void slotLogForNetwork(bool enabled)
        {
            QWriteLocker wl(&m_lock);
            this->m_slotLogNetwork = enabled;
        }

        //! Slot logging for own aircraft context
        //! \remarks thread safe
        void slotLogForOwnAircraft(bool enabled)
        {
            QWriteLocker wl(&m_lock);
            this->m_slotLogOwnAircraft = enabled;
        }

        //! Slot logging for settings context
        //! \remarks thread safe
        void slotLogForSettings(bool enabled)
        {
            QWriteLocker wl(&m_lock);
            this->m_slotLogSettings = enabled;
        }

        //! Slot logging for simulator context
        //! \remarks thread safe
        void slotLogForSimulator(bool enabled)
        {
            QWriteLocker wl(&m_lock);
            this->m_slotLogSimulator = enabled;
        }

        //! Slot logging for application context
        //! \remarks thread safe
        bool isSlotLogForApplicationEnabled() const
        {
            QReadLocker rl(&m_lock);
            return this->m_slotLogApplication;
        }

        //! Slot logging for audio context
        //! \remarks thread safe
        bool isSlotLogForAudioEnabled() const
        {
            QReadLocker rl(&m_lock);
            return this->m_slotLogAudio;
        }

        //! Slot logging for network context
        //! \remarks thread safe
        bool isSlotLogForNetworkEnabled() const
        {
            QReadLocker rl(&m_lock);
            return this->m_slotLogNetwork;
        }

        //! Slot log for own aircraft
        //! \remarks thread safe
        bool isSlotLogForOwnAircraftEnabled() const
        {
            QReadLocker rl(&m_lock);
            return this->m_slotLogOwnAircraft;
        }

        //! Slot logging for settings context
        //! \remarks thread safe
        bool isSlotLogForSettingsEnabled() const
        {
            QReadLocker rl(&m_lock);
            return this->m_slotLogSettings;
        }

        //! Slot logging for simulator context
        //! \remarks thread safe
        bool isSlotLogForSimulatorEnabled() const
        {
            QReadLocker rl(&m_lock);
            return this->m_slotLogSimulator;
        }

        //! Slot logging for specified context
        //! \remarks thread safe
        bool isSlotLogEnabledFor(LogContext context) const;

        //! Slot logging
        //! \deprecated Use a brace-initialized QStringList
        void logSlot(const char *func, const QString &param, const QString &p2, const QString &p3 = "", const QString &p4 = "") const;

        //! Slot logging
        void logSlot(const char *func, const QString &param = "") const;

        //! Slot logging
        void logSlot(const char *func, const QStringList &params) const;

        //! Slot logging for bool value
        void logSlot(const char *func, bool boolValue) const;

        //! Slot logging from a specific context
        void logSlot(LogContext ctx, const char *func, const QString &param = "") const;

        //! Slot logging from a specific context
        void logSlot(LogContext ctx, const char *func, const QStringList &params) const;

        //! Slot logging for bool value from a specific context
        void logSlot(LogContext ctx, const char *func, bool boolValue) const;

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
        const IContextApplication *getIContextApplication() const;

        //! Context for application
        IContextApplication *getIContextApplication();

        //! Context for own aircraft
        IContextOwnAircraft *getIContextOwnAircraft();

        //! Context for own aircraft
        const IContextOwnAircraft *getIContextOwnAircraft() const;

        //! Context for settings
        IContextSettings *getIContextSettings();

        //! Context for settings
        const IContextSettings *getIContextSettings() const;

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
        bool m_signalLogApplication;
        bool m_signalLogAudio;
        bool m_signalLogNetwork;
        bool m_signalLogOwnAircraft;
        bool m_signalLogSettings;
        bool m_signalLogSimulator;
        bool m_slotLogApplication;
        bool m_slotLogAudio;
        bool m_slotLogNetwork;
        bool m_slotLogOwnAircraft;
        bool m_slotLogSettings;
        bool m_slotLogSimulator;
        QDBusConnection m_dbusConnection;
        IContextApplication *m_contextApplication;
        IContextAudio *m_contextAudio;
        IContextNetwork *m_contextNetwork;
        IContextOwnAircraft *m_contextOwnAircraft;
        IContextSettings *m_contextSettings;
        IContextSimulator *m_contextSimulator;
        QMultiMap<QString, QMetaObject::Connection> m_logSignalConnections;
        mutable QReadWriteLock m_lock;

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
