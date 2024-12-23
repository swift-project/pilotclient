// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/audionotificationcomponent.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QPointer>
#include <QToolButton>
#include <QtGlobal>

#include "ui_audionotificationcomponent.h"

#include "core/context/contextaudio.h"
#include "gui/guiapplication.h"
#include "misc/audio/audiosettings.h"
#include "misc/audio/notificationsounds.h"
#include "misc/sequence.h"

using namespace swift::core;
using namespace swift::core::context;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::audio;
using namespace swift::misc::physical_quantities;

namespace swift::gui::components
{
    CAudioNotificationComponent::CAudioNotificationComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CAudioNotificationComponent)
    {
        ui->setupUi(this);

        // deferred init, because in a distributed swift system
        // it takes a moment until the settings are sychronized
        // this is leading to undesired "save settings" messages and played sounds
        QPointer<CAudioNotificationComponent> myself(this);
        QTimer::singleShot(2500, this, [=] {
            if (!myself || !sGui || sGui->isShuttingDown()) { return; }
            this->init();
        });
    }

    void CAudioNotificationComponent::init()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        this->reloadSettings();

        // checkboxes for notifications
        bool c = connect(ui->cb_SetupAudioPTTClickDown, &QCheckBox::toggled, this,
                         &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->cb_SetupAudioPTTClickUp, &QCheckBox::toggled, this,
                    &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->cb_SetupAudioNotificationTextMessageFrequency, &QCheckBox::toggled, this,
                    &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->cb_SetupAudioNotificationTextMessagePrivate, &QCheckBox::toggled, this,
                    &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->cb_SetupAudioNotificationTextMessageSupervisor, &QCheckBox::toggled, this,
                    &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->cb_SetupAudioNotificationTextMessageUnicom, &QCheckBox::toggled, this,
                    &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->cb_SetupAudioNotificationTextCallsignMentioned, &QCheckBox::toggled, this,
                    &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->cb_SetupAfvBlocked, &QCheckBox::toggled, this,
                    &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->cb_SetupAfvClicked, &QCheckBox::toggled, this,
                    &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->cb_SetupAudioNotificationLogin, &QCheckBox::toggled, this,
                    &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->cb_SetupAudioNotificationLogoff, &QCheckBox::toggled, this,
                    &CAudioNotificationComponent::onNotificationsToggled, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->pb_SoundReset, &QPushButton::released, this,
                    &CAudioNotificationComponent::resetNotificationSoundsDir, Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(ui->pb_SoundDir, &QPushButton::released, this,
                    &CAudioNotificationComponent::selectNotificationSoundsDir, Qt::QueuedConnection);
        Q_ASSERT(c);

        // volumes
        c = connect(ui->sb_NotificationValueVolume, qOverload<int>(&QSpinBox::valueChanged), this,
                    &CAudioNotificationComponent::onNotificationVolumeChanged);
        Q_ASSERT(c);
    }

    CAudioNotificationComponent::~CAudioNotificationComponent() {}

    void CAudioNotificationComponent::reloadSettings()
    {
        const CSettings as(m_audioSettings.getThreadLocal());

        ui->cb_SetupAudioPTTClickDown->setChecked(as.isNotificationFlagSet(CNotificationSounds::PTTClickKeyDown));
        ui->cb_SetupAudioPTTClickUp->setChecked(as.isNotificationFlagSet(CNotificationSounds::PTTClickKeyUp));

        ui->cb_SetupAudioNotificationTextMessageFrequency->setChecked(
            as.isNotificationFlagSet(CNotificationSounds::NotificationTextMessageFrequency));
        ui->cb_SetupAudioNotificationTextMessagePrivate->setChecked(
            as.isNotificationFlagSet(CNotificationSounds::NotificationTextMessagePrivate));
        ui->cb_SetupAudioNotificationTextMessageSupervisor->setChecked(
            as.isNotificationFlagSet(CNotificationSounds::NotificationTextMessageSupervisor));
        ui->cb_SetupAudioNotificationTextMessageUnicom->setChecked(
            as.isNotificationFlagSet(CNotificationSounds::NotificationTextMessageUnicom));
        ui->cb_SetupAudioNotificationTextCallsignMentioned->setChecked(
            as.isNotificationFlagSet(CNotificationSounds::NotificationTextCallsignMentioned));

        ui->cb_SetupAfvBlocked->setChecked(as.isNotificationFlagSet(CNotificationSounds::AFVBlocked));
        ui->cb_SetupAfvClicked->setChecked(as.isNotificationFlagSet(CNotificationSounds::AFVClicked));

        ui->cb_SetupAudioNotificationLogin->setChecked(
            as.isNotificationFlagSet(CNotificationSounds::NotificationLogin));
        ui->cb_SetupAudioNotificationLogoff->setChecked(
            as.isNotificationFlagSet(CNotificationSounds::NotificationLogoff));

        ui->le_SoundDir->setText(as.getNotificationSoundDirectory());
        ui->sb_NotificationValueVolume->setValue(as.getNotificationVolume());
    }

    void CAudioNotificationComponent::onNotificationVolumeChanged(int volume)
    {
        volume = qMax(25, qMin(100, volume));
        CSettings as(m_audioSettings.getThreadLocal());
        if (as.getNotificationVolume() == volume) { return; }
        as.setNotificationVolume(volume);
        m_audioSettings.set(as);
    }

    CNotificationSounds::NotificationFlag CAudioNotificationComponent::checkBoxToFlag(const QCheckBox *cb) const
    {
        if (!cb) { return CNotificationSounds::NoNotifications; }

        if (cb == ui->cb_SetupAudioPTTClickDown) { return CNotificationSounds::PTTClickKeyDown; }
        if (cb == ui->cb_SetupAudioPTTClickUp) { return CNotificationSounds::PTTClickKeyUp; }

        if (cb == ui->cb_SetupAudioNotificationTextCallsignMentioned)
        {
            return CNotificationSounds::NotificationTextCallsignMentioned;
        }
        if (cb == ui->cb_SetupAudioNotificationTextMessageFrequency)
        {
            return CNotificationSounds::NotificationTextMessageFrequency;
        }
        if (cb == ui->cb_SetupAudioNotificationTextMessagePrivate)
        {
            return CNotificationSounds::NotificationTextMessagePrivate;
        }
        if (cb == ui->cb_SetupAudioNotificationTextMessageSupervisor)
        {
            return CNotificationSounds::NotificationTextMessageSupervisor;
        }
        if (cb == ui->cb_SetupAudioNotificationTextMessageUnicom)
        {
            return CNotificationSounds::NotificationTextMessageUnicom;
        }

        if (cb == ui->cb_SetupAfvBlocked) { return CNotificationSounds::AFVBlocked; }
        if (cb == ui->cb_SetupAfvClicked) { return CNotificationSounds::AFVClicked; }

        if (cb == ui->cb_SetupAudioNotificationLogin) { return CNotificationSounds::NotificationLogin; }
        if (cb == ui->cb_SetupAudioNotificationLogoff) { return CNotificationSounds::NotificationLogoff; }

        return CNotificationSounds::NoNotifications;
    }

    void CAudioNotificationComponent::onNotificationsToggled(bool checked)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }
        CSettings as(m_audioSettings.getThreadLocal());

        as.setNotificationFlag(CNotificationSounds::PTTClickKeyDown, ui->cb_SetupAudioPTTClickDown->isChecked());
        as.setNotificationFlag(CNotificationSounds::PTTClickKeyUp, ui->cb_SetupAudioPTTClickUp->isChecked());

        as.setNotificationFlag(CNotificationSounds::NotificationTextMessageFrequency,
                               ui->cb_SetupAudioNotificationTextMessageFrequency->isChecked());
        as.setNotificationFlag(CNotificationSounds::NotificationTextMessagePrivate,
                               ui->cb_SetupAudioNotificationTextMessagePrivate->isChecked());
        as.setNotificationFlag(CNotificationSounds::NotificationTextMessageSupervisor,
                               ui->cb_SetupAudioNotificationTextMessageSupervisor->isChecked());
        as.setNotificationFlag(CNotificationSounds::NotificationTextMessageUnicom,
                               ui->cb_SetupAudioNotificationTextMessageUnicom->isChecked());
        as.setNotificationFlag(CNotificationSounds::NotificationTextCallsignMentioned,
                               ui->cb_SetupAudioNotificationTextCallsignMentioned->isChecked());

        as.setNotificationFlag(CNotificationSounds::AFVBlocked, ui->cb_SetupAfvBlocked->isChecked());
        as.setNotificationFlag(CNotificationSounds::AFVClicked, ui->cb_SetupAfvClicked->isChecked());

        as.setNotificationFlag(CNotificationSounds::NotificationLogin, ui->cb_SetupAudioNotificationLogin->isChecked());
        as.setNotificationFlag(CNotificationSounds::NotificationLogoff,
                               ui->cb_SetupAudioNotificationLogoff->isChecked());

        const CStatusMessage msg = m_audioSettings.set(as);
        CLogMessage(this).preformatted(msg);

        const QCheckBox *sender = qobject_cast<const QCheckBox *>(QObject::sender());
        if (checked && sGui && sGui->getCContextAudioBase() && sender)
        {
            const CNotificationSounds::NotificationFlag f = this->checkBoxToFlag(sender);
            sGui->getCContextAudioBase()->playNotification(f, false, as.getNotificationVolume());
        }
    }

    void CAudioNotificationComponent::selectNotificationSoundsDir()
    {
        CSettings s = m_audioSettings.get();
        const QString dir =
            QFileDialog::getExistingDirectory(this, QStringLiteral("Open directory"), s.getNotificationSoundDirectory(),
                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        const QDir d(dir);
        if (d.exists())
        {
            s.setNotificationSoundDirectory(dir);
            ui->le_SoundDir->setText(s.getNotificationSoundDirectory());
            const CStatusMessage m = m_audioSettings.setAndSave(s);
            CLogMessage::preformatted(m);
        }
    }

    void CAudioNotificationComponent::resetNotificationSoundsDir()
    {
        CSettings s = m_audioSettings.get();
        s.setNotificationSoundDirectory("");
        const CStatusMessage m = m_audioSettings.setAndSave(s);
        CLogMessage::preformatted(m);
        ui->le_SoundDir->clear();
    }

} // namespace swift::gui::components
