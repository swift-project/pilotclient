/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_settings_impl.h"
#include "context_runtime.h"

#include "blackmisc/settingutilities.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QFile>
#include <QJsonDocument>

using namespace BlackMisc;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Network;
using namespace BlackMisc::Hardware;

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextSettings::CContextSettings(CRuntimeConfig::ContextMode mode, CRuntime *parent) : IContextSettings(mode, parent) {}

    /*
     * Read settings
     */
    CStatusMessage CContextSettings::read()
    {
        if (!CSettingUtilities::initSettingsDirectory())
        {
            return CStatusMessage(CStatusMessage::TypeCore, CStatusMessage::SeverityError,
                                  QString("Cannot init directory: %1").arg(this->getSettingsDirectory()));
        }
        bool ok = false;
        QFile jsonFile(this->getSettingsFileName());
        QJsonObject obj;

        if (jsonFile.open(QFile::ReadOnly))
        {
            QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
            obj = doc.object();
            ok = true;
        }
        jsonFile.close();

        // init network
        if (obj.contains(IContextSettings::PathNetworkSettings()))
        {
            this->m_settingsNetwork.fromJson(
                obj.value(IContextSettings::PathNetworkSettings()).toObject()
            );
        }
        else
        {
            this->m_settingsNetwork.initDefaultValues();
        }

        // init own members
        if (obj.contains(IContextSettings::PathHotkeys()))
        {
            this->m_hotkeys.fromJson(
                obj.value(IContextSettings::PathHotkeys()).toObject()
            );
        }
        this->m_hotkeys.initAsHotkeyList(false); // update missing parts

        if (ok)
        {
            return CStatusMessage(CStatusMessage::TypeCore, CStatusMessage::SeverityInfo,
                                  QString("Read settings: %1").arg(this->getSettingsFileName()));
        }
        else
        {
            return CStatusMessage(CStatusMessage::TypeCore, CStatusMessage::SeverityError,
                                  QString("Problem reading settings: %1").arg(this->getSettingsFileName()));
        }
    }

    /*
     * Write settings
     */
    CStatusMessage CContextSettings::write() const
    {
        if (!CSettingUtilities::initSettingsDirectory())
        {
            return CStatusMessage(CStatusMessage::TypeCore, CStatusMessage::SeverityError,
                                  QString("Cannot init directory: %1").arg(this->getSettingsDirectory()));
        }
        QFile jsonFile(this->getSettingsFileName());
        bool ok = false;
        if (jsonFile.open(QFile::WriteOnly))
        {
            QJsonDocument doc = this->toJsonDocument();
            ok = jsonFile.write(doc.toJson(QJsonDocument::Indented)) >= 0;
            jsonFile.close();
        }
        if (ok)
        {
            return CStatusMessage(CStatusMessage::TypeSettings, CStatusMessage::SeverityInfo,
                                  QString("Written settings: %1").arg(this->getSettingsFileName()));
        }
        else
        {
            return CStatusMessage(CStatusMessage::TypeSettings, CStatusMessage::SeverityError,
                                  QString("Problem writing settings: %1").arg(this->getSettingsFileName()));
        }
    }

    /*
     * Reset settings
     */
    CStatusMessage CContextSettings::reset(bool write)
    {
        this->m_hotkeys.initAsHotkeyList(true);
        this->m_settingsNetwork.initDefaultValues();
        this->emitCompletelyChanged();
        if (write)
            return this->write();
        else
            return CStatusMessage(CStatusMessage::TypeSettings, CStatusMessage::SeverityInfo,
                                  QString("Reset settings data, not written"));

    }

    QString CContextSettings::getSettingsAsJsonString() const
    {
        QJsonDocument doc = this->toJsonDocument();
        return QString(doc.toJson(QJsonDocument::Indented));
    }

    /*
     * JSON document
     */
    QJsonDocument CContextSettings::toJsonDocument() const
    {
        QJsonObject obj;
        obj.insert(IContextSettings::PathNetworkSettings(), this->m_settingsNetwork.toJson());
        obj.insert(IContextSettings::PathHotkeys(), this->m_hotkeys.toJson());
        QJsonDocument doc(obj);
        return doc;
    }

    /*
     * Emit all changed signals
     */
    void CContextSettings::emitCompletelyChanged()
    {
        emit this->changedSettings(IContextSettings::SettingsHotKeys);
        emit this->changedSettings(IContextSettings::SettingsNetwork);
    }

    /*
     * Hotkeys
     */
    CKeyboardKeyList CContextSettings::getHotkeys() const
    {
        return this->m_hotkeys;
    }

    /*
     * Network settings
     */
    CSettingsNetwork CContextSettings::getNetworkSettings() const
    {
        return this->m_settingsNetwork;
    }

    /*
     * Pass value
     */
    BlackMisc::CStatusMessageList CContextSettings::value(const QString &path, const QString &command, const BlackMisc::CVariant &value)
    {
        Q_ASSERT(path.length() > 3);
        Q_ASSERT(path.indexOf('/') >= 0);

        BlackMisc::CStatusMessageList msgs;
        if (path.contains(IContextSettings::PathRoot()))
        {
            if (path.contains(IContextSettings::PathHotkeys()))
            {
                if (command == CSettingUtilities::CmdUpdate())
                {
                    BlackMisc::Hardware::CKeyboardKeyList hotkeys = value.value<BlackMisc::Hardware::CKeyboardKeyList>();
                    this->m_hotkeys = hotkeys;
                    msgs.push_back(this->write()); // write settings
                    emit this->changedSettings(static_cast<uint>(SettingsHotKeys));
                    return msgs;
                }
            }
        }

        // next level
        QString nextLevelPath = CSettingUtilities::removeLeadingPath(path);
        bool changed = false;
        if (path.startsWith(IContextSettings::PathNetworkSettings()))
        {
            msgs = this->m_settingsNetwork.value(nextLevelPath, command, value, changed);
            if (changed)
            {
                msgs.push_back(this->write());
                emit this->changedSettings(static_cast<uint>(SettingsNetwork));
            }
        }
        else
        {
            // wrong path
            msgs  = CSettingUtilities::wrongPathMessages(path);
        }
        return msgs;
    }

} // namespace
