/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/context.h"
#include "blackmisc/logcategorylist.h"

using namespace BlackMisc;

namespace BlackCore
{
    namespace Context
    {
        const CLogCategoryList &CContext::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats { BlackMisc::CLogCategory::context() };
            return cats;
        }

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

        IContextSimulator *CContext::getIContextSimulator()
        {
            return this->getRuntime()->getIContextSimulator();
        }

        void CContext::setDebugEnabled(bool debug)
        {
            if (this->m_debugEnabled == debug) { return; }
            emit this->changedLogOrDebugSettings();
        }

        bool CContext::isDebugEnabled() const
        {
            return this->m_debugEnabled;
        }

        void CContext::relayBaseClassSignals(const QString &serviceName, QDBusConnection &connection, const QString &objectPath, const QString &interfaceName)
        {
            bool s = connection.connect(serviceName, objectPath, interfaceName,
                                        "changedLogOrDebugSettings", this, SIGNAL(changedLogOrDebugSettings()));
            Q_ASSERT(s);
        }

        const IContextSimulator *CContext::getIContextSimulator() const
        {
            return this->getRuntime()->getIContextSimulator();
        }

        const CStatusMessage &CContext::statusMessageEmptyContext()
        {
            static const CStatusMessage m(static_cast<CContext *>(nullptr), CStatusMessage::SeverityWarning, u"empty context");
            return m;
        }
    } // ns
} // ns
