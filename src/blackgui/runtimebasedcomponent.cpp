#include "runtimebasedcomponent.h"
#include <QWidget>

namespace BlackGui
{
    const BlackCore::IContextApplication *CRuntimeBasedComponent::getIContextApplication() const
    {
        if (!this->m_runtime) return nullptr;
        return this->m_runtime->getIContextApplication();
    }

    BlackCore::IContextApplication *CRuntimeBasedComponent::getIContextApplication()
    {
        if (!this->m_runtime) return nullptr;
        return this->m_runtime->getIContextApplication();
    }

    BlackCore::IContextAudio *CRuntimeBasedComponent::getIContextAudio()
    {
        if (!this->m_runtime) return nullptr;
        return this->m_runtime->getIContextAudio();
    }

    const BlackCore::IContextAudio *CRuntimeBasedComponent::getIContextAudio() const
    {
        if (!this->m_runtime) return nullptr;
        return this->m_runtime->getIContextAudio();
    }

    void CRuntimeBasedComponent::setRuntimeForComponents(BlackCore::CRuntime *runtime, QWidget *parent)
    {
        if (!parent) return;
        QList<QWidget *> children = parent->findChildren<QWidget *>();
        foreach(QWidget * widget, children)
        {
            if (widget->objectName().isEmpty()) continue; // rule out unamed widgets
            CRuntimeBasedComponent *rbc = dynamic_cast<CRuntimeBasedComponent *>(widget);
            if (rbc) rbc->setRuntime(runtime, false);
        }
    }

    void CRuntimeBasedComponent::createRuntime(const BlackCore::CRuntimeConfig &config, QObject *parent)
    {
        this->m_runtime = new BlackCore::CRuntime(config, parent);
        this->m_runtimeOwner = true;
    }

    BlackCore::IContextNetwork *CRuntimeBasedComponent::getIContextNetwork()
    {
        if (!this->m_runtime) return nullptr;
        return this->m_runtime->getIContextNetwork();
    }

    const BlackCore::IContextNetwork *CRuntimeBasedComponent::getIContextNetwork() const
    {
        if (!this->m_runtime) return nullptr;
        return this->m_runtime->getIContextNetwork();
    }

    BlackCore::IContextSettings *CRuntimeBasedComponent::getIContextSettings()
    {
        if (!this->m_runtime) return nullptr;
        return this->m_runtime->getIContextSettings();
    }

    const BlackCore::IContextSettings *CRuntimeBasedComponent::getIContextSettings() const
    {
        if (!this->m_runtime) return nullptr;
        return this->m_runtime->getIContextSettings();
    }

    const BlackCore::IContextSimulator *CRuntimeBasedComponent::getIContextSimulator() const
    {
        if (!this->m_runtime) return nullptr;
        return this->m_runtime->getIContextSimulator();
    }

    BlackCore::IContextSimulator *CRuntimeBasedComponent::getIContextSimulator()
    {
        if (!this->m_runtime) return nullptr;
        return this->m_runtime->getIContextSimulator();
    }

    void CRuntimeBasedComponent::sendStatusMessage(const BlackMisc::CStatusMessage &statusMessage)
    {
        Q_ASSERT(this->getIContextApplication());
        this->getIContextApplication()->sendStatusMessage(statusMessage);
    }

    void CRuntimeBasedComponent::sendStatusMessages(const BlackMisc::CStatusMessageList &statusMessages)
    {
        Q_ASSERT(this->getIContextApplication());
        this->getIContextApplication()->sendStatusMessages(statusMessages);
    }
}
