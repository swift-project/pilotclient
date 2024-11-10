// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/application.h"
#include "blackcore/context/contextapplicationimpl.h"
#include "blackcore/inputmanager.h"
#include "misc/dbusserver.h"
#include "misc/logcategories.h"
#include "misc/logmessage.h"
#include "misc/settingscache.h"
#include "misc/simplecommandparser.h"

#include <QFile>
#include <QFlags>
#include <QIODevice>
#include <QTextStream>
#include <QtGlobal>

using namespace swift::misc;

namespace BlackCore::Context
{
    CContextApplication::CContextApplication(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextApplication(mode, runtime), CIdentifiable(this)
    {}

    CContextApplication *CContextApplication::registerWithDBus(swift::misc::CDBusServer *server)
    {
        if (!server || getMode() != CCoreFacadeConfig::LocalInDBusServer) { return this; }
        server->addObject(IContextApplication::ObjectPath(), this);
        return this;
    }

    void CContextApplication::changeSettings(const CValueCachePacket &settings, const CIdentifier &origin)
    {
        // Intentionally don't check for round trip here
        emit this->settingsChanged(settings, origin);
    }

    swift::misc::CValueCachePacket CContextApplication::getAllSettings() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        return CSettingsCache::instance()->getAllValuesWithTimestamps();
    }

    QStringList CContextApplication::getUnsavedSettingsKeys() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        return CSettingsCache::instance()->getAllUnsavedKeys();
    }

    CSettingsDictionary CContextApplication::getUnsavedSettingsKeysDescribed() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        const QStringList keys = CSettingsCache::instance()->getAllUnsavedKeys();
        CSettingsDictionary result;
        for (const QString &key : keys) { result.insert(key, CSettingsCache::instance()->getHumanReadableName(key)); }
        return result;
    }

    void CContextApplication::synchronizeLocalSettings()
    {
        // no-op: proxy implements this method by calling getAllSettings
    }

    swift::misc::CStatusMessage CContextApplication::saveSettings(const QString &keyPrefix)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << keyPrefix; }
        return CSettingsCache::instance()->saveToStore(keyPrefix);
    }

    swift::misc::CStatusMessage CContextApplication::saveSettingsByKey(const QStringList &keys)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << keys.join(", "); }
        return CSettingsCache::instance()->saveToStore(keys);
    }

    swift::misc::CStatusMessage CContextApplication::loadSettings()
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
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

    CIdentifier CContextApplication::registerApplication(const CIdentifier &application)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << application; }

        if (!m_registeredApplications.contains(application))
        {
            m_registeredApplications.push_back(application);
            emit this->registrationChanged();
            emit this->hotkeyActionsRegistered(sApp->getInputManager()->allAvailableActions(), {});
        }

        this->cleanupRegisteredApplications();
        return application;
    }

    void CContextApplication::unregisterApplication(const CIdentifier &application)
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO << application; }
        int r = m_registeredApplications.remove(application);
        this->cleanupRegisteredApplications();
        if (r > 0) { emit registrationChanged(); }
    }

    void CContextApplication::cleanupRegisteredApplications()
    {
        // static const int outdatedMs = qRound(1.5 * PingIdentifiersMs);
        // m_registeredApplications.removeOlderThanNowMinusOffset(outdatedMs);
    }

    CIdentifierList CContextApplication::getRegisteredApplications() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        // static const int outdatedMs = qRound(1.5 * PingIdentifiersMs);
        // return m_registeredApplications.findAfterNowMinusOffset(outdatedMs);
        return m_registeredApplications;
    }

    CIdentifier CContextApplication::getApplicationIdentifier() const
    {
        if (isDebugEnabled()) { CLogMessage(this, CLogCategories::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->identifier();
    }
} // ns
