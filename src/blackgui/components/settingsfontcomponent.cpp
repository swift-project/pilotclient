/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
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
            this->setMode(CSettingsFontComponent::DirectUpdate);

            this->initValues(); // most likely default font at ctor call time
            connect(ui->tb_SettingsGuiFontColor,    &QToolButton::clicked, this, &CSettingsFontComponent::fontColorDialog);
            connect(ui->tb_SettingsGuiNoFontColor,  &QToolButton::clicked, this, &CSettingsFontComponent::noColor);
            connect(ui->pb_Ok,                      &QPushButton::clicked, this, &CSettingsFontComponent::changeFont);
            connect(ui->pb_CancelOrReset,           &QToolButton::clicked, this, &CSettingsFontComponent::resetFont);

            // only after the complete startup style sheet font overrides are available
            connect(sGui, &CGuiApplication::startUpCompleted, this, &CSettingsFontComponent::initValues);
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

        void CSettingsFontComponent::setFont(const QFont &font)
        {
            m_cancelFont = font;
            this->resetFont();
        }

        QFont CSettingsFontComponent::getFont() const
        {
            return ui->cb_SettingsGuiFont->font();
        }

        QStringList CSettingsFontComponent::getFamilySizeStyle() const
        {
            const QString fontSize = ui->cb_SettingsGuiFontSize->currentText().append("pt");
            const QString fontFamily = ui->cb_SettingsGuiFont->currentFont().family();
            const QString fontStyleCombined = ui->cb_SettingsGuiFontStyle->currentText();
            return QStringList({ fontFamily, fontSize, fontStyleCombined });
        }

        void CSettingsFontComponent::setWithColorSelection(bool withColor)
        {
            ui->le_SettingsGuiFontColor->setVisible(withColor);
            ui->tb_SettingsGuiFontColor->setVisible(withColor);
        }

        void CSettingsFontComponent::changeFont()
        {
            const QString fontSize = ui->cb_SettingsGuiFontSize->currentText().append("pt");
            const QString fontFamily = ui->cb_SettingsGuiFont->currentFont().family();
            const QString fontStyleCombined = ui->cb_SettingsGuiFontStyle->currentText();

            const QStringList familySizeStyle = this->getFamilySizeStyle();
            QString fontColor = m_selectedColor.name();
            if (!m_selectedColor.isValid() || m_selectedColor.name().isEmpty())
            {
                // fontColor = sGui->getStyleSheetUtility().fontColor();
                fontColor.clear();
            }
            ui->le_SettingsGuiFontColor->setText(fontColor);
            m_qss = CStyleSheetUtility::asStylesheet(fontFamily, fontSize, CStyleSheetUtility::fontStyle(fontStyleCombined), CStyleSheetUtility::fontWeight(fontStyleCombined), fontColor);
            if (m_mode == CSettingsFontComponent::DirectUpdate)
            {
                const bool ok = sGui->updateFont(m_qss);
                if (ok)
                {
                    CLogMessage(this).info(u"Updated font style");
                }
                else
                {
                    CLogMessage(this).warning(u"Updating style failed");
                }
            }
            emit this->accept();
        }

        void CSettingsFontComponent::fontColorDialog()
        {
            const QColor c = QColorDialog::getColor(m_selectedColor, this, "Font color");
            if (c == m_selectedColor) return;
            m_selectedColor = c;
            ui->le_SettingsGuiFontColor->setText(m_selectedColor.name());
        }

        void CSettingsFontComponent::noColor()
        {
            m_selectedColor = QColor(); // invalid color
            ui->le_SettingsGuiFontColor->clear();
        }

        void CSettingsFontComponent::initValues()
        {
            // Font
            m_cancelFont = this->font();
            m_cancelColor = QColor(sGui->getStyleSheetUtility().fontColor());
            this->initUiValues(m_cancelFont, m_cancelColor);
        }

        void CSettingsFontComponent::initUiValues(const QFont &font, const QColor &color)
        {
            ui->cb_SettingsGuiFontStyle->setCurrentText(CStyleSheetUtility::fontAsCombinedWeightStyle(font));
            ui->cb_SettingsGuiFont->setCurrentFont(font);
            ui->cb_SettingsGuiFontSize->setCurrentText(QString::number(font.pointSize()));
            ui->le_SettingsGuiFontColor->setText(color.name());
            m_selectedColor = color;
            m_qss.clear();
        }

        void CSettingsFontComponent::resetFont()
        {
            if (!sGui || sGui->isShuttingDown()) { return; }
            this->initUiValues(m_cancelFont, m_cancelColor);
            if (m_mode == CSettingsFontComponent::DirectUpdate)
            {
                sGui->resetFont();
            }
            emit this->reject();
        }
    } // ns
} // ns
