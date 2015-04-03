/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_settings_impl.h"
#include "context_runtime.h"

#include "blackmisc/settingutilities.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/logmessage.h"
#include <QFile>
#include <QJsonDocument>

using namespace BlackMisc;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Network;
using namespace BlackMisc::Hardware;
using namespace BlackMisc::Simulation::Settings;

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
            return CLogMessage(this).error("Cannot init directory: %1") << this->getSettingsDirectory();
        }
        bool ok = false;
        QFile jsonFile(this->getSettingsFileName());
        QJsonObject jsonObject;

        if (jsonFile.open(QFile::ReadOnly))
        {
            QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
            jsonObject = doc.object();
            ok = true;
        }
        jsonFile.close();

        // init network
        if (jsonObject.contains(IContextSettings::PathNetworkSettings()))
        {
            this->m_settingsNetwork.convertFromJson(
                jsonObject.value(IContextSettings::PathNetworkSettings()).toObject()
            );
        }
        else
        {
            this->m_settingsNetwork.initDefaultValues();
        }

        // init audio
        if (jsonObject.contains(IContextSettings::PathAudioSettings()))
        {
            this->m_settingsAudio.convertFromJson(
                jsonObject.value(IContextSettings::PathAudioSettings()).toObject()
            );
        }
        else
        {
            this->m_settingsAudio.initDefaultValues();
        }

        // init simulator
        if (jsonObject.contains(IContextSettings::PathSimulatorSettings()))
        {
            this->m_settingsSimulator.convertFromJson(
                jsonObject.value(IContextSettings::PathSimulatorSettings()).toObject()
            );
        }
        else
        {
            this->m_settingsSimulator.initDefaultValues();
        }

        // init own members
        if (jsonObject.contains(IContextSettings::PathHotkeys()))
        {
            this->m_hotkeys.convertFromJson(
                jsonObject.value(IContextSettings::PathHotkeys()).toObject()
            );
        }
        this->m_hotkeys.initAsHotkeyList(false); // update missing parts

        if (ok)
        {
            return CStatusMessage(CStatusMessage::SeverityInfo, QString("Read settings: %1").arg(this->getSettingsFileName()));
        }
        else
        {
            return CStatusMessage(CStatusMessage::SeverityError, QString("Problem reading settings: %1").arg(this->getSettingsFileName()));
        }
    }

    /*
     * Write settings
     */
    CStatusMessage CContextSettings::write() const
    {
        if (!CSettingUtilities::initSettingsDirectory())
        {
            return CStatusMessage(CStatusMessage::SeverityError, QString("Cannot init directory: %1").arg(this->getSettingsDirectory()));
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
            return CStatusMessage(CStatusMessage::SeverityInfo, QString("Written settings: %1").arg(this->getSettingsFileName()));
        }
        else
        {
            return CStatusMessage(CStatusMessage::SeverityError, QString("Problem writing settings: %1").arg(this->getSettingsFileName()));
        }
    }

    /*
     * Reset settings
     */
    CStatusMessage CContextSettings::reset(bool write)
    {
        this->m_hotkeys.initAsHotkeyList(true);
        this->m_settingsNetwork.initDefaultValues();
        this->m_settingsAudio.initDefaultValues();
        this->m_settingsSimulator.initDefaultValues();
        this->emitCompletelyChanged();
        if (write)
        {
            return this->write();
        }
        else
        {
            return CStatusMessage(CStatusMessage::SeverityInfo, "Reset settings data, not written");
        }
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
        QJsonObject jsonObject;
        jsonObject.insert(IContextSettings::PathNetworkSettings(), this->m_settingsNetwork.toJson());
        jsonObject.insert(IContextSettings::PathAudioSettings(), this->m_settingsAudio.toJson());
        jsonObject.insert(IContextSettings::PathSimulatorSettings(), this->m_settingsSimulator.toJson());
        jsonObject.insert(IContextSettings::PathHotkeys(), this->m_hotkeys.toJson());
        QJsonDocument doc(jsonObject);
        return doc;
    }

    /*
     * Emit all changed signals
     */
    void CContextSettings::emitCompletelyChanged()
    {
        emit this->changedSettings(IContextSettings::SettingsHotKeys);
        emit this->changedSettings(IContextSettings::SettingsNetwork);
        emit this->changedSettings(IContextSettings::SettingsAudio);
        emit this->changedSettings(IContextSettings::SettingsSimulator);
    }

    /*
     * Hotkeys
     */
    CSettingKeyboardHotkeyList CContextSettings::getHotkeys() const
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
     * Audio settings
     */
    CSettingsAudio CContextSettings::getAudioSettings() const
    {
        return this->m_settingsAudio;
    }

    /*
     * Simulator settings
     */
    CSettingsSimulator CContextSettings::getSimulatorSettings() const
    {
        return this->m_settingsSimulator;
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
                    auto hotkeys = value.value<BlackMisc::Settings::CSettingKeyboardHotkeyList>();
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
            msgs.push_back(this->m_settingsNetwork.value(nextLevelPath, command, value, changed));
            if (changed)
            {
                msgs.push_back(this->write());
                emit this->changedSettings(static_cast<uint>(SettingsNetwork));
            }
        }
        else if (path.startsWith(IContextSettings::PathAudioSettings()))
        {
            msgs.push_back(this->m_settingsAudio.value(nextLevelPath, command, value, changed));
            if (changed)
            {
                msgs.push_back(this->write());
                emit this->changedSettings(static_cast<uint>(SettingsAudio));
            }
        }
        else if (path.startsWith(IContextSettings::PathSimulatorSettings()))
        {
            msgs.push_back(this->m_settingsSimulator.value(nextLevelPath, command, value, changed));
            if (changed)
            {
                msgs.push_back(this->write());
                emit this->changedSettings(static_cast<uint>(SettingsSimulator));
            }
        }
        else
        {
            msgs.push_back(
                CStatusMessage({CLogCategory::validation()}, CStatusMessage::SeverityError, QString("Wrong path: %1").arg(this->getSettingsFileName()))
            );
        }
        return msgs;
    }

} // namespace
