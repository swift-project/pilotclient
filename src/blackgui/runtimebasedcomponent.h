#ifndef BLACKGUI_RUNTIMEBASEDCOMPONENT_H
#define BLACKGUI_RUNTIMEBASEDCOMPONENT_H

#include "blackcore/context_runtime.h"
#include "blackcore/context_all_interfaces.h"
#include <QTimer>

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
        void setRuntime(BlackCore::CRuntime *runtime, bool runtimeOwner = false);

        //! Set runtime for each CRuntimeBasedComponent
        static void setRuntimeForComponents(BlackCore::CRuntime *runtime, QWidget *parent);

    protected:
        //! Constructor
        //! \remarks Usually runtime will be provided later, not at initialization time.
        //!          If runtime is provided right now, make sure to call runtimeHasBeenSet afterwards
        CRuntimeBasedComponent(BlackCore::CRuntime *runtime = nullptr, bool runtimeOwner = false) :
            m_runtime(runtime), m_runtimeOwner(runtimeOwner)
        {}

        //! Runtime const
        const BlackCore::CRuntime *getRuntime() const { return this->m_runtime;}

        //! Runtime non const
        BlackCore::CRuntime *getRuntime() { return this->m_runtime;}

        //! Create a runtime (becomes owner). Only create one runtime.
        void createRuntime(const BlackCore::CRuntimeConfig &config, QObject *parent);

        //! Context for application
        const BlackCore::IContextApplication *getIContextApplication() const;

        //! Context for application
        BlackCore::IContextApplication *getIContextApplication();

        //! Context for audio
        BlackCore::IContextAudio *getIContextAudio();

        //! Context for audio
        const BlackCore::IContextAudio *getIContextAudio() const;

        //! Context for network
        BlackCore::IContextNetwork *getIContextNetwork();

        //! Context for network
        const BlackCore::IContextNetwork *getIContextNetwork() const;

        //! Context for own aircraft
        const BlackCore::IContextOwnAircraft *getIContextOwnAircraft() const;

        //! Context for own aircraft
        BlackCore::IContextOwnAircraft *getIContextOwnAircraft();

        //! Context for settings
        BlackCore::IContextSettings *getIContextSettings();

        //! Context for settings
        const BlackCore::IContextSettings *getIContextSettings() const;

        //! Context for simulator
        const BlackCore::IContextSimulator *getIContextSimulator() const;

        //! Context for simulator
        BlackCore::IContextSimulator *getIContextSimulator();

        //! Send status message (via application context)
        void sendStatusMessage(const BlackMisc::CStatusMessage &statusMessage);

        //! Send status message (via application context)
        void sendStatusMessages(const BlackMisc::CStatusMessageList &statusMessages);

        //! Owner?
        bool isRuntimeOwner() const { return this->m_runtimeOwner; }

        //! "Callback" when runtime is initialized, done this way as we do not have signals/slots here
        //! \remarks use this methods to hook up signal/slots with runtime
        virtual void runtimeHasBeenSet() {}

    private:
        BlackCore::CRuntime *m_runtime;
        bool m_runtimeOwner;
    };

} // namespace

#endif // guard
