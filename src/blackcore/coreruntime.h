#ifndef BLACKCORE_CORERUNTIME_H
#define BLACKCORE_CORERUNTIME_H

#include <QObject>

namespace BlackCore
{
    class CDBusServer;
    class CContextNetwork;
    class CContextVoice;
    class CContextSettings;
    class CContextApplication;
    class IContextNetwork;
    class IContextVoice;
    class IContextSettings;
    class IContextApplication;

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
        CContextVoice *m_contextVoice;
        CContextSettings *m_contextSettings;
        CContextApplication *m_contextApplication;

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
        IContextNetwork *getIContextNetwork();

        /*!
         * \brief Context for network
         * \return
         */
        const IContextNetwork *getIContextNetwork() const;

        /*!
         * \brief Context for network
         * \return
         */
        IContextVoice *getIContextVoice();

        /*!
         * \brief Context for network
         * \return
         */
        const IContextVoice *getIContextVoice() const;


        /*!
         * \brief Settings
         * \return
         */
        IContextSettings *getIContextSettings();

        /*!
         * \brief Settings
         * \return
         */
        const IContextSettings *getIContextSettings() const;

        /*!
         * \brief Context for application
         * \return
         */
        const IContextApplication *getIContextApplication() const;

        /*!
         * \brief Application
         * \return
         */
        IContextApplication *getIContextApplication();

    };
}
#endif // guard
