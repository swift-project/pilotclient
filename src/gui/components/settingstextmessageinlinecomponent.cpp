// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/logmessage.h"
#include "misc/statusmessage.h"
#include "settingstextmessageinlinecomponent.h"
#include "ui_settingstextmessageinlinecomponent.h"

#include <QPushButton>
#include <QCheckBox>
#include <QTimer>
#include <QPointer>

using namespace swift::gui::settings;
using namespace swift::misc;

namespace swift::gui::components
{
    CSettingsTextMessageInlineComponent::CSettingsTextMessageInlineComponent(QWidget *parent) : QFrame(parent),
                                                                                                ui(new Ui::CSettingsTextMessageInlineComponent)
    {
        ui->setupUi(this);
        connect(ui->cb_All, &QCheckBox::released, this, &CSettingsTextMessageInlineComponent::changeSettings);
        connect(ui->cb_Frequency, &QCheckBox::released, this, &CSettingsTextMessageInlineComponent::changeSettings);
        connect(ui->cb_Private, &QCheckBox::released, this, &CSettingsTextMessageInlineComponent::changeSettings);
        connect(ui->cb_Supervisor, &QCheckBox::released, this, &CSettingsTextMessageInlineComponent::changeSettings);
        connect(ui->cb_Focus, &QCheckBox::released, this, &CSettingsTextMessageInlineComponent::changeSettings);
        connect(ui->pb_Disable, &QPushButton::released, this, &CSettingsTextMessageInlineComponent::disableAllOverlayMessages);
        connect(ui->pb_Reset, &QPushButton::released, this, &CSettingsTextMessageInlineComponent::resetOverlayMessages);

        this->settingsChanged();
    }

    CSettingsTextMessageInlineComponent::~CSettingsTextMessageInlineComponent()
    {}

    void CSettingsTextMessageInlineComponent::settingsChanged()
    {
        const CTextMessageSettings s(m_settings.get());
        ui->cb_All->setChecked(s.getPopupAllMessages());
        ui->cb_Supervisor->setChecked(s.getPopupSupervisorMessages());
        ui->cb_Frequency->setChecked(s.getPopupFrequencyMessages());
        ui->cb_Private->setChecked(s.getPopupPrivateMessages());
        ui->cb_Selcal->setChecked(s.getPopupSelcalMessages());
        ui->cb_Focus->setChecked(s.focusOverlayWindow());
    }

    void CSettingsTextMessageInlineComponent::changeSettings()
    {
        CTextMessageSettings s(m_settings.get());
        s.setPopupAllMessages(ui->cb_All->isChecked());
        s.setPopupFrequencyMessages(ui->cb_Frequency->isChecked());
        s.setPopupPrivateMessages(ui->cb_Private->isChecked());
        s.setSupervisorMessages(ui->cb_Supervisor->isChecked());
        s.setPopupSelcalMessages(ui->cb_Selcal->isChecked());
        s.setFocusOverlayWindows(ui->cb_Focus->isChecked());
        const CStatusMessage m = m_settings.setAndSave(s);
        CLogMessage::preformatted(m);
    }

    void CSettingsTextMessageInlineComponent::disableAllOverlayMessages()
    {
        CTextMessageSettings s(m_settings.get());
        s.disableAllPopups();
        const CStatusMessage m = m_settings.setAndSave(s);
        CLogMessage::preformatted(m);

        QPointer<CSettingsTextMessageInlineComponent> myself(this);
        QTimer::singleShot(500, this, [=] {
            if (myself) { myself->settingsChanged(); }
        });
    }

    void CSettingsTextMessageInlineComponent::resetOverlayMessages()
    {
        CTextMessageSettings s;
        const CStatusMessage m = m_settings.setAndSave(s);
        CLogMessage::preformatted(m);

        QPointer<CSettingsTextMessageInlineComponent> myself(this);
        QTimer::singleShot(500, this, [=] {
            if (myself) { myself->settingsChanged(); }
        });
    }
} // ns
