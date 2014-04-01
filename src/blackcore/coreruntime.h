#ifndef BLACKCORE_CORERUNTIME_H
#define BLACKCORE_CORERUNTIME_H

#include <QObject>

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

    //! \brief The CCoreRuntime class
    class CCoreRuntime : public QObject
    {
        Q_OBJECT

    private:
        bool m_init; /*!< flag */
        CDBusServer *m_dbusServer;
        CContextNetwork *m_contextNetwork;
        CContextAudio *m_contextAudio;
        CContextSettings *m_contextSettings;
        CContextApplication *m_contextApplication;
        CContextSimulator *m_contextSimulator;

        //! \brief Init
        void init(bool withDbus);

    public:
        //! \brief Constructor
        CCoreRuntime(bool withDbus = true, QObject *parent = nullptr);

        //! \brief Destructor
        virtual ~CCoreRuntime() {}

        //! \brief DBus server
        const CDBusServer *getDBusServer() const
        {
            return this->m_dbusServer;
        }

        //! \brief Context for network
        IContextNetwork *getIContextNetwork();

        //! \brief Context for network
        const IContextNetwork *getIContextNetwork() const;

        //! \brief Context for network
        IContextAudio *getIContextAudio();

        //! \brief Context for network
        const IContextAudio *getIContextAudio() const;

        //! \brief Settings
        IContextSettings *getIContextSettings();

        //! \brief Settings
        const IContextSettings *getIContextSettings() const;

        //! \brief Context for application
        const IContextApplication *getIContextApplication() const;

        //! \brief Application
        IContextApplication *getIContextApplication();

        //! \brief Context for simulator
        const IContextSimulator *getIContextSimulator() const;

        //! \brief Simulator
        IContextSimulator *getIContextSimulator();

    };
}
#endif // guard
