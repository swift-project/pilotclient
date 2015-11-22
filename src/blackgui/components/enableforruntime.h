/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ENABLEFORRUNTIME_H
#define BLACKGUI_ENABLEFORRUNTIME_H

#include "blackgui/blackguiexport.h"
#include "blackcore/contextruntime.h"
#include "blackmisc/audio/notificationsounds.h"
#include <QTimer>

namespace BlackCore
{
    class CRuntime;
    class CRuntimeConfig;
    class IContextApplication;
    class IContextAudio;
    class IContextNetwork;
    class IContextOwnAircraft;
    class IContextSettings;
    class IContextSimulator;
}

namespace BlackGui
{
    namespace Components
    {

        //! Component, which provides references to runtime objects
        //! \details Access to runtime allows to encapsualate many aspects of data access and makes
        //!          the component widely independent from a central data provideer
        //! \sa BlackCore::CRuntime
        class BLACKGUI_EXPORT CEnableForRuntime
        {
        public:
            //! Set runtime, usually set by runtime owner (must only be one, usually main window)
            void setRuntime(BlackCore::CRuntime *runtime, bool runtimeOwner = false);

            //! Set runtime for each CRuntimeBasedComponent
            static void setRuntimeForComponents(BlackCore::CRuntime *runtime, QWidget *parent);

            //! Log message category
            static const BlackMisc::CLogCategoryList &getLogCategories()
            {
                static const BlackMisc::CLogCategoryList cats { BlackMisc::CLogCategory::guiComponent() };
                return cats;
            }

        protected:
            //! Constructor
            //! \remarks Usually runtime will be provided later, not at initialization time.
            //!          If runtime is provided right now, make sure to call runtimeHasBeenSet afterwards
            CEnableForRuntime(BlackCore::CRuntime *runtime = nullptr, bool runtimeOwner = false) :
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

            //! Context for simulator
            const BlackCore::IContextSimulator *getIContextSimulator() const;

            //! Context for simulator
            BlackCore::IContextSimulator *getIContextSimulator();

            //! Owner?
            bool isRuntimeOwner() const { return this->m_runtimeOwner; }

            //! "Callback" when runtime is initialized, done this way as we do not have signals/slots here
            //! \remarks use this methods to hook up signal/slots with runtime
            virtual void runtimeHasBeenSet() {}

            //! \copydoc CRuntime::hasRemoteApplicationContext
            bool hasRemoteApplicationContext() const { return this->m_runtime->hasRemoteApplicationContext(); }

            //! Play a given notification sound
            void playNotifcationSound(BlackMisc::Audio::CNotificationSounds::Notification notification) const;

        private:
            BlackCore::CRuntime *m_runtime;
            bool m_runtimeOwner;
        };
    }
} // namespace

#endif // guard
