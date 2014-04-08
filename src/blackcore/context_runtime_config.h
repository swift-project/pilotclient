#ifndef BLACKCORE_CONTEXT_RUNTIME_CONFIG_H
#define BLACKCORE_CONTEXT_RUNTIME_CONFIG_H

namespace BlackCore
{
    //! Helper to correctly run a context
    class CRuntimeConfig
    {

    public:
        //! How to handle a given context
        enum ContextMode
        {
            NotUsed,
            Local,
            LocalInDbusServer,
            Remote
        };

    private:
        ContextMode m_settings;
        ContextMode m_audio;
        ContextMode m_network;
        ContextMode m_simulator;
        ContextMode m_application;

    public:

        //! Constructor
        CRuntimeConfig(ContextMode allTheSame):
            m_settings(allTheSame), m_audio(allTheSame), m_network(allTheSame), m_simulator(allTheSame), m_application(allTheSame)
        {}

        //! Constructor
        CRuntimeConfig(ContextMode settings, ContextMode audio, ContextMode network, ContextMode simulator, ContextMode application):
            m_settings(settings), m_audio(audio), m_network(network), m_simulator(simulator), m_application(application)
        {}

        //! settings mode
        ContextMode getModeSettings() const { return this->m_settings; }
        //! audio mode
        ContextMode getModeAudio() const { return this->m_audio; }
        //! network mode
        ContextMode getModeNetwork() const { return this->m_network; }
        //! simulator mode
        ContextMode getModeSimulator() const { return this->m_simulator; }
        //! application mode
        ContextMode getModeApplication() const { return this->m_application; }
        //! local settings?
        bool hasLocalSettings() const { return this->m_settings == Local || this->m_settings == LocalInDbusServer; }
        //! requires server (at least one in server)?
        bool requiresDBusSever() const;
        //!  requires DBus connection (at least one remote)?
        bool requiresDBusConnection() const;

        //! predefined for Core
        static const CRuntimeConfig &forCore();

        //! predefined, completely local (e.g. for unit tests)
        static const CRuntimeConfig &local();

        //! predefined, completely remote (e.g. for GUI with core)
        static const CRuntimeConfig &remote();
    };
}
#endif // guard
