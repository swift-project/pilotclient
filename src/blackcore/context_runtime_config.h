#ifndef BLACKCORE_CONTEXT_RUNTIME_CONFIG_H
#define BLACKCORE_CONTEXT_RUNTIME_CONFIG_H

#include <QString>

namespace BlackCore
{
    //! Helper to correctly run a context
    class CRuntimeConfig
    {

    public:
        //! How to handle a given context
        enum ContextMode
        {
            NotUsed,           //!< during shutdown or not used at all
            Local,             //!< in same process
            LocalInDbusServer, //!< in same process, also registered in DBus, will be accessed by proxy objects too
            Remote             //!< proxy object
        };

    private:
        ContextMode m_application;
        ContextMode m_audio;
        ContextMode m_network;
        ContextMode m_ownAircraft;
        ContextMode m_settings;
        ContextMode m_simulator;
        QString m_dbusAddress; //!< for boot strapping

    public:
        //! Constructor
        CRuntimeConfig(ContextMode allTheSame = NotUsed, const QString &dbusBootstrapAddress = ""):
            m_application(allTheSame), m_audio(allTheSame), m_network(allTheSame), m_ownAircraft(allTheSame), m_settings(allTheSame), m_simulator(allTheSame),
            m_dbusAddress(dbusBootstrapAddress)
        {}

        //! Constructor
        CRuntimeConfig(ContextMode application, ContextMode audio, ContextMode network, ContextMode ownAircraft, ContextMode settings, ContextMode simulator, const QString &dbusBootstrapAddress = ""):
            m_application(application), m_audio(audio), m_network(network), m_ownAircraft(ownAircraft) , m_settings(settings), m_simulator(simulator),
            m_dbusAddress(dbusBootstrapAddress)
        {}

        //! application mode
        ContextMode getModeApplication() const { return this->m_application; }
        //! audio mode
        ContextMode getModeAudio() const { return this->m_audio; }
        //! network mode
        ContextMode getModeNetwork() const { return this->m_network; }
        //! own aircraft
        ContextMode getModeOwnAircraft() const { return this->m_ownAircraft; }
        //! settings mode
        ContextMode getModeSettings() const { return this->m_settings; }
        //! simulator mode
        ContextMode getModeSimulator() const { return this->m_simulator; }
        //! local settings?
        bool hasLocalSettings() const { return this->m_settings == Local || this->m_settings == LocalInDbusServer; }
        //! requires server (at least one in server)?
        bool requiresDBusSever() const;
        //! requires DBus connection (at least one remote)?
        bool requiresDBusConnection() const;
        //! DBus address
        QString getDBusAddress() const { return this->m_dbusAddress; }
        //! DBus address?
        bool hasDBusAddress() const { return !this->m_dbusAddress.isEmpty(); }
        //! predefined for Core
        static const CRuntimeConfig &forCoreAllLocalInDBus(const QString &dbusBootstrapAddress = "");
        //! predefined for Core
        static const CRuntimeConfig &forCoreAllLocalInDBusNoAudio(const QString &dbusBootstrapAddress = "");
        //! predefined, completely local (e.g. for unit tests)
        static const CRuntimeConfig &local(const QString &dbusBootstrapAddress = "");
        //! predefined, completely remote (e.g. for GUI with core)
        static const CRuntimeConfig &remote(const QString &dbusBootstrapAddress = "");
        //! predefined, remote with local audio (e.g. Aduio in GUI)
        static const CRuntimeConfig &remoteLocalAudio(const QString &dbusBootstrapAddress = "");
    };
}
#endif // guard
