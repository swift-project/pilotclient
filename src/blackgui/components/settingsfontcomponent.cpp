/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/guiapplication.h"
#include "blackmisc/logmessage.h"
#include "settingsfontcomponent.h"
#include "ui_settingsfontcomponent.h"

#include <QColorDialog>
#include <QPushButton>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CSettingsFontComponent::CSettingsFontComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsFontComponent)
        {
            ui->setupUi(this);

            // Font
            const QFont font = this->font();
            m_cancelFont = font;
            m_cancelColor = QColor(sGui->getStyleSheetUtility().fontColor());
            this->setMode(CSettingsFontComponent::DirectUpdate);
            this->initUiValues();

            connect(ui->tb_SettingsGuiFontColor, &QToolButton::clicked, this, &CSettingsFontComponent::fontColorDialog);
            connect(ui->pb_Ok, &QPushButton::clicked, this, &CSettingsFontComponent::changeFont);
            connect(ui->pb_CancelOrReset, &QToolButton::pressed, this, &CSettingsFontComponent::resetFont);
        }

        CSettingsFontComponent::~CSettingsFontComponent()
        { }

        void CSettingsFontComponent::setMode(CSettingsFontComponent::Mode m)
        {
            m_mode = m;
            if (m == CSettingsFontComponent::DirectUpdate)
            {
                ui->pb_CancelOrReset->setText("reset");
            }
            else
            {
                ui->pb_CancelOrReset->setText("cancel");
            }
        }

        void CSettingsFontComponent::setCurrentFont(const QFont &font)
        {
            m_cancelFont = font;
            this->resetFont();
        }

        void CSettingsFontComponent::changeFont()
        {
            const QString fontSize = ui->cb_SettingsGuiFontSize->currentText().append("pt");
            const QString fontFamily = ui->cb_SettingsGuiFont->currentFont().family();
            const QString fontStyleCombined = ui->cb_SettingsGuiFontStyle->currentText();
            QString fontColor = this->m_selectedColor.name();
            if (!this->m_selectedColor.isValid() || this->m_selectedColor.name().isEmpty())
            {
                fontColor = sGui->getStyleSheetUtility().fontColor();
            }
            ui->le_SettingsGuiFontColor->setText(fontColor);
            m_qss = CStyleSheetUtility::asStylesheet(fontFamily, fontSize, CStyleSheetUtility::fontStyle(fontStyleCombined), CStyleSheetUtility::fontWeight(fontStyleCombined), fontColor);
            if (m_mode == CSettingsFontComponent::DirectUpdate)
            {
                const bool ok = sGui->updateFont(m_qss);
                if (ok)
                {
                    CLogMessage(this).info("Updated font style");
                }
                else
                {
                    CLogMessage(this).warning("Updating style failed");
                }
            }
            emit this->accept();
        }

        void CSettingsFontComponent::fontColorDialog()
        {
            const QColor c = QColorDialog::getColor(this->m_selectedColor, this, "Font color");
            if (c == this->m_selectedColor) return;
            this->m_selectedColor = c;
            ui->le_SettingsGuiFontColor->setText(this->m_selectedColor.name());
        }

        void CSettingsFontComponent::initUiValues()
        {
            ui->cb_SettingsGuiFontStyle->setCurrentText(CStyleSheetUtility::fontAsCombinedWeightStyle(m_cancelFont));
            ui->cb_SettingsGuiFont->setCurrentFont(m_cancelFont);
            ui->cb_SettingsGuiFontSize->setCurrentText(QString::number(m_cancelFont.pointSize()));
            ui->le_SettingsGuiFontColor->setText(this->m_cancelColor.name());
            m_selectedColor = m_cancelColor;
            m_qss.clear();
        }

        void CSettingsFontComponent::resetFont()
        {
            this->initUiValues();
            if (m_mode == CSettingsFontComponent::DirectUpdate)
            {
                sGui->resetFont();
            }
            emit this->reject();
        }
    } // ns
} // ns
