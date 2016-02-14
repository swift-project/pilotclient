/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingscomponent.h"
#include "ui_settingscomponent.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/guiapplication.h"
#include "blackcore/contextnetwork.h"
#include "blackcore/contextaudio.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/input/keyboardkeylist.h"
#include "blackmisc/logmessage.h"
#include <QColorDialog>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Input;

namespace BlackGui
{
    namespace Components
    {
        CSettingsComponent::CSettingsComponent(QWidget *parent) :
            QTabWidget(parent),
            ui(new Ui::CSettingsComponent)
        {
            ui->setupUi(this);
            this->tabBar()->setExpanding(false);

            // Opacity, intervals
            this->connect(this->ui->hs_SettingsGuiOpacity, &QSlider::valueChanged, this, &CSettingsComponent::changedWindowsOpacity);
            this->connect(this->ui->hs_SettingsGuiAircraftRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAircraftUpdateInterval);
            this->connect(this->ui->hs_SettingsGuiAtcRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAtcStationsUpdateInterval);
            this->connect(this->ui->hs_SettingsGuiUserRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedUsersUpdateInterval);

            // Font
            const QFont font = this->font();
            this->ui->cb_SettingsGuiFontStyle->setCurrentText(CStyleSheetUtility::fontAsCombinedWeightStyle(font));
            this->ui->cb_SettingsGuiFont->setCurrentFont(font);
            this->ui->cb_SettingsGuiFontSize->setCurrentText(QString::number(font.pointSize()));
            this->m_fontColor = QColor(CStyleSheetUtility::instance().fontColor());
            this->ui->le_SettingsGuiFontColor->setText(this->m_fontColor.name());
            bool connected = this->connect(this->ui->cb_SettingsGuiFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            connected = this->connect(this->ui->cb_SettingsGuiFontSize, SIGNAL(currentIndexChanged(QString)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            connected = this->connect(this->ui->cb_SettingsGuiFontStyle, SIGNAL(currentIndexChanged(QString)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            this->connect(this->ui->tb_SettingsGuiFontColor, &QToolButton::clicked, this, &CSettingsComponent::ps_fontColorDialog);
            Q_UNUSED(connected);
        }

        CSettingsComponent::~CSettingsComponent()
        { }

        bool CSettingsComponent::playNotificationSounds() const
        {
            return ui->comp_AudioSetup->playNotificationSounds();
        }

        void CSettingsComponent::setGuiOpacity(double value)
        {
            this->ui->hs_SettingsGuiOpacity->setValue(value);
        }

        int CSettingsComponent::getAtcUpdateIntervalSeconds() const
        {
            return this->ui->hs_SettingsGuiAtcRefreshTime->value();
        }

        int CSettingsComponent::getAircraftUpdateIntervalSeconds() const
        {
            return this->ui->hs_SettingsGuiAircraftRefreshTime->value();
        }

        int CSettingsComponent::getUsersUpdateIntervalSeconds() const
        {
            return this->ui->hs_SettingsGuiUserRefreshTime->value();
        }

        /*
         * Set tab
         */
        void CSettingsComponent::setSettingsTab(CSettingsComponent::SettingTab tab)
        {
            this->setCurrentIndex(static_cast<int>(tab));
        }

        void CSettingsComponent::ps_fontChanged()
        {
            QString fontSize = this->ui->cb_SettingsGuiFontSize->currentText().append("pt");
            QString fontFamily = this->ui->cb_SettingsGuiFont->currentFont().family();
            QString fontStyleCombined = this->ui->cb_SettingsGuiFontStyle->currentText();
            QString fontColor = this->m_fontColor.name();
            if (!this->m_fontColor.isValid() || this->m_fontColor.name().isEmpty())
            {
                fontColor = CStyleSheetUtility::instance().fontColor();
            }
            this->ui->le_SettingsGuiFontColor->setText(fontColor);
            bool ok = CStyleSheetUtility::instance().updateFonts(fontFamily, fontSize, CStyleSheetUtility::fontStyle(fontStyleCombined), CStyleSheetUtility::fontWeight(fontStyleCombined), fontColor);
            if (ok)
            {
                CLogMessage(this).info("Updated font style");
            }
            else
            {
                CLogMessage(this).info("Updating style failed");
            }
        }

        void CSettingsComponent::ps_fontColorDialog()
        {
            QColor c =  QColorDialog::getColor(this->m_fontColor, this, "Font color");
            if (c == this->m_fontColor) return;
            this->m_fontColor = c;
            this->ui->le_SettingsGuiFontColor->setText(this->m_fontColor.name());
            this->ps_fontChanged();
        }
    }
} // namespace
