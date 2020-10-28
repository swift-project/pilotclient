/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/context.h"
#include "blackcore/application.h"
#include "blackmisc/logcategories.h"

using namespace BlackMisc;

namespace BlackCore
{
    namespace Context
    {
        const QStringList &IContext::getLogCategories()
        {
            static const QStringList cats { BlackMisc::CLogCategories::context() };
            return cats;
        }

        IContext::IContext(CCoreFacadeConfig::ContextMode mode, QObject *parent) :
            QObject(parent), m_mode(mode), m_contextId(QDateTime::currentMSecsSinceEpoch())
        {
            if (sApp && !sApp->isShuttingDown())
            {
                QObject::connect(sApp, &CApplication::aboutToShutdown, this, &IContext::onAboutToShutdown);
            }
        }

        IContextNetwork *IContext::getIContextNetwork()
        {
            return this->getRuntime()->getIContextNetwork();
        }

        const IContextNetwork *IContext::getIContextNetwork() const
        {
            return this->getRuntime()->getIContextNetwork();
        }

        IContextAudio *IContext::getIContextAudio()
        {
            return this->getRuntime()->getIContextAudio();
        }

        const IContextAudio *IContext::getIContextAudio() const
        {
            return this->getRuntime()->getIContextAudio();
        }

        IContextApplication *IContext::getIContextApplication()
        {
            return this->getRuntime()->getIContextApplication();
        }

        const IContextApplication *IContext::getIContextApplication() const
        {
            return this->getRuntime()->getIContextApplication();
        }

        IContextOwnAircraft *IContext::getIContextOwnAircraft()
        {
            return this->getRuntime()->getIContextOwnAircraft();
        }

        const IContextOwnAircraft *IContext::getIContextOwnAircraft() const
        {
            return this->getRuntime()->getIContextOwnAircraft();
        }

        IContextSimulator *IContext::getIContextSimulator()
        {
            return this->getRuntime()->getIContextSimulator();
        }

        void IContext::setDebugEnabled(bool debug)
        {
            if (m_debugEnabled == debug) { return; }
            emit this->changedLogOrDebugSettings();
        }

        bool IContext::isDebugEnabled() const
        {
            return m_debugEnabled;
        }

        void IContext::relayBaseClassSignals(const QString &serviceName, QDBusConnection &connection, const QString &objectPath, const QString &interfaceName)
        {
            bool s = connection.connect(serviceName, objectPath, interfaceName,
                                        "changedLogOrDebugSettings", this, SIGNAL(changedLogOrDebugSettings()));
            Q_ASSERT(s);
        }

        const IContextSimulator *IContext::getIContextSimulator() const
        {
            return this->getRuntime()->getIContextSimulator();
        }

        const CStatusMessage &IContext::statusMessageEmptyContext()
        {
            static const CStatusMessage m(static_cast<IContext *>(nullptr), CStatusMessage::SeverityWarning, u"empty context");
            return m;
        }
    } // ns
} // ns
