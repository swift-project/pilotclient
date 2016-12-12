/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsguicomponent.h"
#include "blackcore/context/contextnetwork.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/logmessage.h"
#include "ui_settingsguicomponent.h"
#include <QColorDialog>
#include <QFont>
#include <QFontComboBox>
#include <QStyleFactory>

using namespace BlackMisc;
using namespace BlackGui::Settings;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CSettingsGuiComponent::CSettingsGuiComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsGuiComponent)
        {
            ui->setupUi(this);

            ui->cb_SettingsGuiWidgetStyle->clear();
            ui->cb_SettingsGuiWidgetStyle->insertItems(0, QStyleFactory::keys());

            // Font
            const QFont font = this->font();
            this->m_fontColor = QColor(sGui->getStyleSheetUtility().fontColor());
            ui->cb_SettingsGuiFontStyle->setCurrentText(CStyleSheetUtility::fontAsCombinedWeightStyle(font));
            ui->cb_SettingsGuiFont->setCurrentFont(font);
            ui->cb_SettingsGuiFontSize->setCurrentText(QString::number(font.pointSize()));
            ui->le_SettingsGuiFontColor->setText(this->m_fontColor.name());

            connect(ui->tb_SettingsGuiFontColor, &QToolButton::clicked, this, &CSettingsGuiComponent::ps_fontColorDialog);
            bool connected = this->connect(ui->cb_SettingsGuiFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            connected = connect(ui->cb_SettingsGuiFontSize, SIGNAL(currentIndexChanged(QString)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            connected = connect(ui->cb_SettingsGuiFontStyle, SIGNAL(currentIndexChanged(QString)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);

            // Widget style and rest
            connect(ui->hs_SettingsGuiOpacity, &QSlider::valueChanged, this, &CSettingsGuiComponent::changedWindowsOpacity);
            connect(ui->cb_SettingsGuiWidgetStyle, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
                    this, &CSettingsGuiComponent::widgetStyleChanged);
            connect(ui->tb_ResetFont, &QToolButton::pressed, this, &CSettingsGuiComponent::ps_resetFont);

            // selection
            connect(ui->rb_PreferExtendedSelection, &QRadioButton::released, this, &CSettingsGuiComponent::ps_selectionChanged);
            connect(ui->rb_PreferMultiSelection, &QRadioButton::released, this, &CSettingsGuiComponent::ps_selectionChanged);

            this->guiSettingsChanged();
            Q_UNUSED(connected);
        }

        CSettingsGuiComponent::~CSettingsGuiComponent()
        { }

        void CSettingsGuiComponent::hideOpacity(bool hide)
        {
            ui->hs_SettingsGuiOpacity->setVisible(!hide);
            ui->lbl_SettingsGuiOpacity->setVisible(!hide);
        }

        void CSettingsGuiComponent::setGuiOpacity(double value)
        {
            ui->hs_SettingsGuiOpacity->setValue(static_cast<int>(value));
        }

        void CSettingsGuiComponent::ps_fontChanged()
        {
            const QString fontSize = ui->cb_SettingsGuiFontSize->currentText().append("pt");
            const QString fontFamily = ui->cb_SettingsGuiFont->currentFont().family();
            const QString fontStyleCombined = ui->cb_SettingsGuiFontStyle->currentText();
            QString fontColor = this->m_fontColor.name();
            if (!this->m_fontColor.isValid() || this->m_fontColor.name().isEmpty())
            {
                fontColor = sGui->getStyleSheetUtility().fontColor();
            }
            ui->le_SettingsGuiFontColor->setText(fontColor);
            const bool ok = sGui->updateFont(fontFamily, fontSize, CStyleSheetUtility::fontStyle(fontStyleCombined), CStyleSheetUtility::fontWeight(fontStyleCombined), fontColor);
            if (ok)
            {
                CLogMessage(this).info("Updated font style");
            }
            else
            {
                CLogMessage(this).info("Updating style failed");
            }
        }

        void CSettingsGuiComponent::ps_fontColorDialog()
        {
            const QColor c =  QColorDialog::getColor(this->m_fontColor, this, "Font color");
            if (c == this->m_fontColor) return;
            this->m_fontColor = c;
            ui->le_SettingsGuiFontColor->setText(this->m_fontColor.name());
            this->ps_fontChanged();
        }

        void CSettingsGuiComponent::ps_resetFont()
        {
            sGui->resetFont();
        }

        void CSettingsGuiComponent::ps_selectionChanged()
        {
            QAbstractItemView::SelectionMode sm = QAbstractItemView::NoSelection;
            if (ui->rb_PreferExtendedSelection->isChecked())
            {
                sm = QAbstractItemView::ExtendedSelection;
            }
            else if (ui->rb_PreferMultiSelection->isChecked())
            {
                sm = QAbstractItemView::MultiSelection;
            }
            if (sm == this->m_guiSettings.get().getPreferredSelection()) { return; }
            const CStatusMessage m = this->m_guiSettings.setAndSaveProperty(CGeneralGuiSettings::IndexPreferredSelection, CVariant::fromValue(sm));
            CLogMessage::preformatted(m);
        }

        void CSettingsGuiComponent::guiSettingsChanged()
        {
            const CGeneralGuiSettings settings(m_guiSettings.getThreadLocal());
            const int index = ui->cb_SettingsGuiWidgetStyle->findText(settings.getWidgetStyle());
            if (index != ui->cb_SettingsGuiWidgetStyle->currentIndex())
            {
                ui->cb_SettingsGuiWidgetStyle->setCurrentIndex(index);
            }

            switch (settings.getPreferredSelection())
            {
            case QAbstractItemView::ExtendedSelection: ui->rb_PreferExtendedSelection->setChecked(true); break;
            case QAbstractItemView::MultiSelection: ui->rb_PreferMultiSelection->setChecked(true); break;
            default: break;
            }
        }

        void CSettingsGuiComponent::widgetStyleChanged(const QString &widgetStyle)
        {
            const CGeneralGuiSettings settings = m_guiSettings.getThreadLocal();
            if (!settings.isDifferentValidWidgetStyle(widgetStyle)) { return; }
            if (sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected())
            {
                // Style changes freeze the GUI, must not be done in flight mode
                CLogMessage(this).validationError("Cannot change style while connected to network");
                ui->cb_SettingsGuiWidgetStyle->setCurrentText(settings.getWidgetStyle());
                return;
            }
            const CStatusMessage m = this->m_guiSettings.setAndSaveProperty(CGeneralGuiSettings::IndexWidgetStyle, widgetStyle);
            CLogMessage::preformatted(m);
        }
    } // ns
} // ns
