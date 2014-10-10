/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "enableforruntime.h"
#include "blackcore/context_all_interfaces.h"
#include <QWidget>

namespace BlackGui
{
    namespace Components
    {
        void CEnableForRuntime::setRuntime(BlackCore::CRuntime *runtime, bool runtimeOwner)
        {
            Q_ASSERT(runtime);
            this->m_runtime = runtime;
            this->m_runtimeOwner = runtimeOwner;
            this->runtimeHasBeenSet();
        }

        void CEnableForRuntime::setRuntimeForComponents(BlackCore::CRuntime *runtime, QWidget *parent)
        {
            if (!parent) return;

            // tested for runtime, not too slow, only some ms
            QList<QWidget *> children = parent->findChildren<QWidget *>();
            foreach(QWidget * widget, children)
            {
                if (widget->objectName().isEmpty()) continue; // rule out unamed widgets
                CEnableForRuntime *rbc = dynamic_cast<CEnableForRuntime *>(widget);
                if (rbc) rbc->setRuntime(runtime, false);
            }
        }

        void CEnableForRuntime::createRuntime(const BlackCore::CRuntimeConfig &config, QObject *parent)
        {
            this->m_runtime = new BlackCore::CRuntime(config, parent);
            this->m_runtimeOwner = true;
        }

        const BlackCore::IContextApplication *CEnableForRuntime::getIContextApplication() const
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextApplication();
        }

        BlackCore::IContextApplication *CEnableForRuntime::getIContextApplication()
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextApplication();
        }

        BlackCore::IContextAudio *CEnableForRuntime::getIContextAudio()
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextAudio();
        }

        const BlackCore::IContextAudio *CEnableForRuntime::getIContextAudio() const
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextAudio();
        }

        BlackCore::IContextNetwork *CEnableForRuntime::getIContextNetwork()
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextNetwork();
        }

        const BlackCore::IContextNetwork *CEnableForRuntime::getIContextNetwork() const
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextNetwork();
        }

        BlackCore::IContextOwnAircraft *CEnableForRuntime::getIContextOwnAircraft()
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextOwnAircraft();
        }

        const BlackCore::IContextOwnAircraft *CEnableForRuntime::getIContextOwnAircraft() const
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextOwnAircraft();
        }

        BlackCore::IContextSettings *CEnableForRuntime::getIContextSettings()
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextSettings();
        }

        const BlackCore::IContextSettings *CEnableForRuntime::getIContextSettings() const
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextSettings();
        }

        const BlackCore::IContextSimulator *CEnableForRuntime::getIContextSimulator() const
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextSimulator();
        }

        BlackCore::IContextSimulator *CEnableForRuntime::getIContextSimulator()
        {
            if (!this->m_runtime) return nullptr;
            return this->m_runtime->getIContextSimulator();
        }

        void CEnableForRuntime::playNotifcationSound(BlackSound::CNotificationSounds::Notification notification) const
        {
            if (!this->getIContextAudio()) return;
            this->getIContextAudio()->playNotification(static_cast<uint>(notification), true);
        }
    }
}
