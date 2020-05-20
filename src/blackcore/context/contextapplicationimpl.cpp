/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/application.h"
#include "blackcore/context/contextapplicationimpl.h"
#include "blackcore/inputmanager.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/simplecommandparser.h"

#include <QFile>
#include <QFlags>
#include <QIODevice>
#include <QTextStream>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackCore
{
    namespace Context
    {
        CContextApplication::CContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
            IContextApplication(mode, runtime), CIdentifiable(this)
        { }

        CContextApplication *CContextApplication::registerWithDBus(BlackMisc::CDBusServer *server)
        {
            if (!server || m_mode != CCoreFacadeConfig::LocalInDBusServer) { return this; }
            server->addObject(IContextApplication::ObjectPath(), this);
            return this;
        }

        void CContextApplication::changeSettings(const CValueCachePacket &settings, const CIdentifier &origin)
        {
            // Intentionally don't check for round trip here
            emit this->settingsChanged(settings, origin);
        }

        BlackMisc::CValueCachePacket CContextApplication::getAllSettings() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return CSettingsCache::instance()->getAllValuesWithTimestamps();
        }

        QStringList CContextApplication::getUnsavedSettingsKeys() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return CSettingsCache::instance()->getAllUnsavedKeys();
        }

        CSettingsDictionary CContextApplication::getUnsavedSettingsKeysDescribed() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            const QStringList keys = CSettingsCache::instance()->getAllUnsavedKeys();
            CSettingsDictionary result;
            for (const QString &key : keys) { result.insert(key, CSettingsCache::instance()->getHumanReadableName(key)); }
            return result;
        }

        void CContextApplication::synchronizeLocalSettings()
        {
            // no-op: proxy implements this method by calling getAllSettings
        }

        BlackMisc::CStatusMessage CContextApplication::saveSettings(const QString &keyPrefix)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << keyPrefix; }
            return CSettingsCache::instance()->saveToStore(keyPrefix);
        }

        BlackMisc::CStatusMessage CContextApplication::saveSettingsByKey(const QStringList &keys)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << keys.join(", "); }
            return CSettingsCache::instance()->saveToStore(keys);
        }

        BlackMisc::CStatusMessage CContextApplication::loadSettings()
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return CSettingsCache::instance()->loadFromStore();
        }

        void CContextApplication::registerHotkeyActions(const QStringList &actions, const CIdentifier &origin)
        {
            // Intentionally don't check for round trip here
            emit this->hotkeyActionsRegistered(actions, origin);
        }

        void CContextApplication::callHotkeyActionRemotely(const QString &action, bool argument, const CIdentifier &origin)
        {
            if (origin.hasApplicationProcessId())
            {
                // If it originated from this process, then we are going to emit a signal
                emit this->remoteHotkeyAction(action, argument, origin);
            }
            else
            {
                // action came from a different process but on the same machine. Ignore
                if (origin.isFromLocalMachine()) { return; }

                // Different process and different machine. Process it.
                // However, it should not emit a remote action itself.
                sApp->getInputManager()->callFunctionsBy(action, argument, false);
            }
        }

        bool CContextApplication::writeToFile(const QString &fileName, const QString &content)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << fileName << content.left(25); }
            if (fileName.isEmpty()) { return false; }
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                out << content;
                return true;
            }
            return false;
        }

        CIdentifier CContextApplication::registerApplication(const CIdentifier &application)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << application; }

            CIdentifier identifier(application);
            identifier.setCurrentUtcTime();

            if (!m_registeredApplications.contains(identifier))
            {
                m_registeredApplications.push_back(identifier);
                emit this->registrationChanged();
                emit this->hotkeyActionsRegistered(sApp->getInputManager()->allAvailableActions(), {});
            }
            else
            {
                m_registeredApplications.replace(application, identifier);
            }

            this->cleanupRegisteredApplications();
            return application;
        }

        void CContextApplication::unregisterApplication(const CIdentifier &application)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << application; }
            int r = m_registeredApplications.remove(application);
            this->cleanupRegisteredApplications();
            if (r > 0) { emit registrationChanged(); }
        }

        void CContextApplication::cleanupRegisteredApplications()
        {
            static const int outdatedMs = qRound(1.5 * PingIdentifiersMs);
            m_registeredApplications.removeOlderThanNowMinusOffset(outdatedMs);
        }

        CIdentifierList CContextApplication::getRegisteredApplications() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            static const int outdatedMs = qRound(1.5 * PingIdentifiersMs);
            return m_registeredApplications.findAfterNowMinusOffset(outdatedMs);
        }

        CIdentifier CContextApplication::getApplicationIdentifier() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return this->identifier();
        }

        QString CContextApplication::readFromFile(const QString &fileName) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << fileName; }
            QFile file(fileName);
            QString content;
            if (fileName.isEmpty()) return content;
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&file);
                in >> content;
                file.close();
            }
            return content;
        }

        bool CContextApplication::removeFile(const QString &fileName)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << fileName; }
            if (fileName.isEmpty()) { return false; }
            return QFile::remove(fileName);
        }

        bool CContextApplication::existsFile(const QString &fileName) const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << fileName; }
            if (fileName.isEmpty()) return false;
            return QFile::exists(fileName);
        }

        QString CContextApplication::dotCommandsHtmlHelp() const
        {
            return CSimpleCommandParser::commandsHtmlHelp();
        }
    } // ns
} // ns
