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
using namespace BlackMisc::Input;

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
        this->m_settingsAudio.initDefaultValues();
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
        jsonObject.insert(IContextSettings::PathAudioSettings(), this->m_settingsAudio.toJson());
        QJsonDocument doc(jsonObject);
        return doc;
    }

    /*
     * Emit all changed signals
     */
    void CContextSettings::emitCompletelyChanged()
    {
        emit this->changedSettings(IContextSettings::SettingsNetwork);
        emit this->changedSettings(IContextSettings::SettingsAudio);
        emit this->changedSettings(IContextSettings::SettingsSimulator);
    }

    /*
     * Audio settings
     */
    CSettingsAudio CContextSettings::getAudioSettings() const
    {
        return this->m_settingsAudio;
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
        }

        // next level
        QString nextLevelPath = CSettingUtilities::removeLeadingPath(path);
        if (path.startsWith(IContextSettings::PathAudioSettings()))
        {
            bool changed = false;
            msgs.push_back(this->m_settingsAudio.value(nextLevelPath, command, value, changed));
            if (changed)
            {
                msgs.push_back(this->write());
                emit this->changedSettings(static_cast<uint>(SettingsAudio));
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
