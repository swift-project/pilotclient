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

    IContextOwnAircraft *CContext::getIContextOwnAircraft()
    {
        return this->getRuntime()->getIContextOwnAircraft();
    }

    const IContextOwnAircraft *CContext::getIContextOwnAircraft() const
    {
        return this->getRuntime()->getIContextOwnAircraft();
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

    const IContextSimulator *CContext::getIContextSimulator() const
    {
        return this->getRuntime()->getIContextSimulator();
    }
}
