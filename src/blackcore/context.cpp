#include "context.h"
#include <QDebug>

namespace BlackCore
{

    IContextNetwork *CContext::getIContextNetwork()
    {
        return this->getRuntime()->getIContextNetwork();
    }

    const IContextNetwork *CContext::getIContextNetwork() const
    {
        return this->getRuntime()->getIContextNetwork();
    }

    IContextAudio *CContext::getIContextAudio()
    {
        return this->getRuntime()->getIContextAudio();
    }

    const IContextAudio *CContext::getIContextAudio() const
    {
        return this->getRuntime()->getIContextAudio();
    }

    IContextApplication *CContext::getIContextApplication()
    {
        return this->getRuntime()->getIContextApplication();
    }

    const IContextApplication *CContext::getIContextApplication() const
    {
        return this->getRuntime()->getIContextApplication();
    }

    IContextSettings *CContext::getIContextSettings()
    {
        return this->getRuntime()->getIContextSettings();
    }

    const IContextSettings *CContext::getIContextSettings() const
    {
        return this->getRuntime()->getIContextSettings();
    }

    IContextSimulator *CContext::getIContextSimulator()
    {
        return this->getRuntime()->getIContextSimulator();
    }

    void CContext::reEmitSignalFromProxy(const QString &signalName)
    {
        if (signalName.isEmpty()) return;
        if (this->usingLocalObjects())
        {
            // resent in implementation
            QString sn = signalName;
            if (!sn.endsWith("()")) sn.append("()");
            const QMetaObject *metaObject = this->metaObject();
            int signalId = metaObject->indexOfSignal(sn.toUtf8().constData());
            Q_ASSERT(signalId >= 0);
            void *a[] = { 0 };
            QMetaObject::activate(this, signalId, a);
        }
        else
        {
            Q_ASSERT_X(false, "signalFromProxy", "Proxy needs to override method");
        }
    }

    const IContextSimulator *CContext::getIContextSimulator() const
    {
        return this->getRuntime()->getIContextSimulator();
    }
}
