/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "context.h"
#include <QDebug>

using namespace BlackMisc;

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

    const CStatusMessage &CContext::statusMessageEmptyContext()
    {
        static const CStatusMessage m(getLogCategories(), CStatusMessage::SeverityWarning, "empty context");
        return m;
    }
} // namespace
