// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingsfontcomponent.h"

#include <QColorDialog>
#include <QPushButton>
#include <QStringBuilder>

#include "ui_settingsfontcomponent.h"

#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "misc/logmessage.h"

using namespace swift::misc;

namespace swift::gui::components
{
    CSettingsFontComponent::CSettingsFontComponent(QWidget *parent) : QFrame(parent),
                                                                      ui(new Ui::CSettingsFontComponent)
    {
        ui->setupUi(this);
        this->setMode(CSettingsFontComponent::DirectUpdate);

        // due to the problems with overriding a color (e.g T571) we use "no color" as default
        this->initValues();
        connect(ui->tb_SettingsGuiFontColor, &QToolButton::clicked, this, &CSettingsFontComponent::fontColorDialog);
        connect(ui->tb_SettingsGuiNoFontColor, &QToolButton::clicked, this, &CSettingsFontComponent::noColor);
        connect(ui->pb_Ok, &QPushButton::clicked, this, &CSettingsFontComponent::changeFont, Qt::QueuedConnection);
        connect(ui->pb_CancelOrReset, &QToolButton::clicked, this, &CSettingsFontComponent::resetFontAndReject, Qt::QueuedConnection);
        connect(ui->pb_Reset, &QToolButton::clicked, this, &CSettingsFontComponent::clearQssAndResetFont, Qt::QueuedConnection);

        // only after the complete startup style sheet font overrides are available
        connect(sGui, &CGuiApplication::startUpCompleted, this, &CSettingsFontComponent::initValues);
    }

    CSettingsFontComponent::~CSettingsFontComponent()
    {}

    void CSettingsFontComponent::setMode(CSettingsFontComponent::Mode m)
    {
        m_mode = m;
        if (m == CSettingsFontComponent::DirectUpdate)
        {
            ui->pb_CancelOrReset->setText("reset");
            ui->pb_Reset->setVisible(false);
        }
        else
        {
            ui->pb_CancelOrReset->setText("cancel");
            ui->pb_Reset->setVisible(true);
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

    void CSettingsFontComponent::setStyleSheetDefaultColor()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        m_noColorDefault = false;
        this->initValues();
    }

    void CSettingsFontComponent::changeFont()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        const QString fontSize = ui->cb_SettingsGuiFontSize->currentText().append("pt");
        const QString fontFamily = ui->cb_SettingsGuiFont->currentFont().family();
        const QString fontStyleCombined = ui->cb_SettingsGuiFontStyle->currentText();

        QString fontColor = m_selectedColor.name();
        if (!m_selectedColor.isValid() || m_selectedColor.name().isEmpty())
        {
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
        const QColor c = QColorDialog::getColor(m_selectedColor.isValid() ? m_selectedColor : m_cancelColor, this, "Font color");
        if (c == m_selectedColor) { return; }
        m_selectedColor = c;
        ui->le_SettingsGuiFontColor->setText(m_selectedColor.name());
    }

    void CSettingsFontComponent::noColor()
    {
        m_selectedColor = QColor(); // invalid color
        m_noColorDefault = true;
        ui->le_SettingsGuiFontColor->clear();
    }

    void CSettingsFontComponent::initValues()
    {
        // Font
        if (!sGui) { return; }
        m_cancelFont = this->font();
        const QString colorString(sGui->getStyleSheetUtility().fontColorString());
        m_cancelColor = colorString.isEmpty() ? QColor() : QColor(colorString);
        this->initUiValues(m_cancelFont, m_noColorDefault ? QColor() : m_cancelColor);
    }

    void CSettingsFontComponent::initUiValues(const QFont &font, const QColor &color)
    {
        ui->cb_SettingsGuiFontStyle->setCurrentText(CStyleSheetUtility::fontAsCombinedWeightStyle(font));
        ui->cb_SettingsGuiFont->setCurrentFont(font);
        ui->cb_SettingsGuiFontSize->setCurrentText(QString::number(font.pointSize()));

        const bool valid = color.isValid();
        m_selectedColor = color; // color
        ui->le_SettingsGuiFontColor->setText(valid ? m_selectedColor.name() : "");
        m_qss.clear();
    }

    void CSettingsFontComponent::resetFont()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        this->initUiValues(m_cancelFont, m_noColorDefault ? QColor() : m_cancelColor);
        if (m_mode == CSettingsFontComponent::DirectUpdate)
        {
            sGui->resetFont();
        }
    }

    void CSettingsFontComponent::resetFontAndReject()
    {
        this->resetFont();
        emit this->reject();
    }

    void CSettingsFontComponent::clearQssAndResetFont()
    {
        m_qss.clear();
        this->resetFont();
        emit this->accept();
    }
} // namespace swift::gui::components
