#ifndef BLACKGUI_RUNTIMEBASEDCOMPONENT_H
#define BLACKGUI_RUNTIMEBASEDCOMPONENT_H

#include "blackcore/context_runtime.h"
#include "blackcore/context_all_interfaces.h"

namespace BlackGui
{
    /*!
     * \brief Component, which provides reference to runtime object
     * \details Access to runtime allows to encapsualate many aspects of data access and makes
     *          the component widely independent from a central data provideer
     * \sa BlackCore::CRuntime
     */
    class CRuntimeBasedComponent
    {
    public:
        //! Set runtime, usually set by runtime owner (must only be one, usually main window)
        void setRuntime(BlackCore::CRuntime *runtime, bool runtimeOwner = false) { this->m_runtime = runtime; this->m_runtimeOwner = runtimeOwner; }

    protected:
        //! Constructor
        //! \remarks usually runtime will be provide later, not at initialization time
        CRuntimeBasedComponent(BlackCore::CRuntime *runtime = nullptr, bool runtimeOwner = false) :
            m_runtime(runtime), m_runtimeOwner(runtimeOwner)
        {}

        //! Runtime const
        const BlackCore::CRuntime *getRuntime() const { return this->m_runtime;}

        //! Runtime non const
        BlackCore::CRuntime *getRuntime() { return this->m_runtime;}

        //! Create a runtime (becomes owner). Only create one runtime.
        void createRuntime(const BlackCore::CRuntimeConfig &config, QObject *parent);

        //! Context for network
        BlackCore::IContextNetwork *getIContextNetwork();

        //! Context for network
        const BlackCore::IContextNetwork *getIContextNetwork() const;

        //! Context for audio
        BlackCore::IContextAudio *getIContextAudio();

        //! Context for audio
        const BlackCore::IContextAudio *getIContextAudio() const;

        //! Context for settings
        BlackCore::IContextSettings *getIContextSettings();

        //! Context for settings
        const BlackCore::IContextSettings *getIContextSettings() const;

        //! Context for application
        const BlackCore::IContextApplication *getIContextApplication() const;

        //! Context for application
        BlackCore::IContextApplication *getIContextApplication();

        //! Context for simulator
        const BlackCore::IContextSimulator *getIContextSimulator() const;

        //! Context for simulator
        BlackCore::IContextSimulator *getIContextSimulator();

        //! Owner?
        bool isRuntimeOwner() const { return this->m_runtimeOwner; }

    private:
        BlackCore::CRuntime *m_runtime;
        bool m_runtimeOwner;

    };

} // namespace

#endif // guard
