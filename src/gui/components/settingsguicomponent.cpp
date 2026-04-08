// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingsguicomponent.h"

#include <QColorDialog>
#include <QFont>
#include <QFontComboBox>
#include <QMessageBox>
#include <QStyleFactory>

#include "ui_settingsguicomponent.h"

#include "core/context/contextnetwork.h"
#include "gui/guiapplication.h"
#include "misc/logmessage.h"

using namespace swift::misc;
using namespace swift::gui::settings;
using namespace swift::core::context;

namespace swift::gui::components
{
    CSettingsGuiComponent::CSettingsGuiComponent(QWidget *parent) : QFrame(parent), ui(new Ui::CSettingsGuiComponent)
    {
        ui->setupUi(this);

        ui->cb_SettingsGuiWidgetStyle->clear();
        ui->cb_SettingsGuiWidgetStyle->insertItems(0, QStyleFactory::keys());

        // Language selector
        ui->cb_SettingsGuiLanguage->clear();
        ui->cb_SettingsGuiLanguage->addItem(tr("System default"),       QString());
        ui->cb_SettingsGuiLanguage->addItem(tr("English"),              QStringLiteral("en"));
        ui->cb_SettingsGuiLanguage->addItem(tr("Deutsch (German)"),     QStringLiteral("de"));
        ui->cb_SettingsGuiLanguage->addItem(tr("Français (French)"),    QStringLiteral("fr"));
        ui->cb_SettingsGuiLanguage->addItem(tr("Español (Spanish)"),    QStringLiteral("es"));
        ui->cb_SettingsGuiLanguage->addItem(tr("简体中文 (Simplified Chinese)"), QStringLiteral("zh_CN"));

        // Widget style
        connect(ui->hs_SettingsGuiOpacity, &QSlider::valueChanged, this, &CSettingsGuiComponent::changedWindowsOpacity);
        connect(ui->cb_SettingsGuiWidgetStyle, qOverload<int>(&QComboBox::currentIndexChanged), this,
                &CSettingsGuiComponent::widgetStyleChanged, Qt::QueuedConnection);
        connect(ui->cb_SettingsGuiLanguage, qOverload<int>(&QComboBox::currentIndexChanged), this,
                &CSettingsGuiComponent::languageChanged, Qt::QueuedConnection);

        ui->comp_SettingsFonts->setStyleSheetDefaultColor();

        // selection
        connect(ui->rb_PreferExtendedSelection, &QRadioButton::released, this,
                &CSettingsGuiComponent::selectionChanged);
        connect(ui->rb_PreferMultiSelection, &QRadioButton::released, this, &CSettingsGuiComponent::selectionChanged);

        this->guiSettingsChanged();
    }

    CSettingsGuiComponent::~CSettingsGuiComponent() = default;

    void CSettingsGuiComponent::hideOpacity(bool hide)
    {
        ui->hs_SettingsGuiOpacity->setVisible(!hide);
        ui->lbl_SettingsGuiOpacity->setVisible(!hide);
    }

    void CSettingsGuiComponent::setGuiOpacity(double value)
    {
        ui->hs_SettingsGuiOpacity->setValue(static_cast<int>(value));
    }

    void CSettingsGuiComponent::selectionChanged()
    {
        QAbstractItemView::SelectionMode sm = QAbstractItemView::NoSelection;
        if (ui->rb_PreferExtendedSelection->isChecked()) { sm = QAbstractItemView::ExtendedSelection; }
        else if (ui->rb_PreferMultiSelection->isChecked()) { sm = QAbstractItemView::MultiSelection; }
        if (sm == m_guiSettings.get().getPreferredSelection()) { return; }
        const CStatusMessage m =
            m_guiSettings.setAndSaveProperty(CGeneralGuiSettings::IndexPreferredSelection, CVariant::fromValue(sm));
        CLogMessage::preformatted(m);
    }

    void CSettingsGuiComponent::guiSettingsChanged()
    {
        const CGeneralGuiSettings settings(m_guiSettings.getThreadLocal());

        // Widget style
        const int styleIndex = ui->cb_SettingsGuiWidgetStyle->findText(settings.getWidgetStyle());
        if (styleIndex != ui->cb_SettingsGuiWidgetStyle->currentIndex())
        {
            ui->cb_SettingsGuiWidgetStyle->setCurrentIndex(styleIndex);
        }

        // Language
        const int langIndex = ui->cb_SettingsGuiLanguage->findData(settings.getUiLanguage());
        const int langTarget = (langIndex < 0) ? 0 : langIndex;
        if (langTarget != ui->cb_SettingsGuiLanguage->currentIndex())
        {
            ui->cb_SettingsGuiLanguage->setCurrentIndex(langTarget);
        }

        // Selection mode
        switch (settings.getPreferredSelection())
        {
        case QAbstractItemView::ExtendedSelection: ui->rb_PreferExtendedSelection->setChecked(true); break;
        case QAbstractItemView::MultiSelection: ui->rb_PreferMultiSelection->setChecked(true); break;
        default: break;
        }
    }

    void CSettingsGuiComponent::languageChanged(int index)
    {
        const QString lang = ui->cb_SettingsGuiLanguage->itemData(index).toString();
        if (lang == m_guiSettings.getThreadLocal().getUiLanguage()) { return; }

        const int ret = QMessageBox::information(this, tr("Change language?"),
                                                 tr("Changing the UI language requires a restart.\n"
                                                    "Changes will be visible at the next start.\n"
                                                    "Do you want to save your changes?"),
                                                 QMessageBox::Ok | QMessageBox::Cancel);
        if (ret != QMessageBox::Ok)
        {
            this->guiSettingsChanged(); // revert combo to saved value
            return;
        }
        const CStatusMessage m = m_guiSettings.setAndSaveProperty(CGeneralGuiSettings::IndexUiLanguage, lang);
        CLogMessage::preformatted(m);
    }

    void CSettingsGuiComponent::widgetStyleChanged(int index)
    {
        const QString widgetStyle = ui->cb_SettingsGuiWidgetStyle->itemText(index);
        const CGeneralGuiSettings settings = m_guiSettings.getThreadLocal();
        if (!settings.isDifferentValidWidgetStyle(widgetStyle)) { return; }

        /** because of crash (chnage style crashes UI) we require restart
        const int ret = QMessageBox::information(this,
                        tr("Change style?"),
                        tr("Changing style is slow.\nThe GUI will hang for some seconds.\nDo you want to save your
        changes?"), QMessageBox::Ok | QMessageBox::Cancel);

        **/
        const int ret = QMessageBox::information(this, tr("Change style?"),
                                                 tr("Changing style requires a restart.\nChanges will be visible a the "
                                                    "next start.\nDo you want to save your changes?"),
                                                 QMessageBox::Ok | QMessageBox::Cancel);

        if (ret != QMessageBox::Ok)
        {
            ui->cb_SettingsGuiWidgetStyle->setCurrentText(settings.getWidgetStyle());
            return;
        }

        if (sGui->getIContextNetwork() && sGui->getIContextNetwork()->isConnected())
        {
            // Style changes freeze the GUI, must not be done in flight mode
            CLogMessage(this).validationError(u"Cannot change style while connected to network");
            ui->cb_SettingsGuiWidgetStyle->setCurrentText(settings.getWidgetStyle());
            return;
        }
        const CStatusMessage m = m_guiSettings.setAndSaveProperty(CGeneralGuiSettings::IndexWidgetStyle, widgetStyle);
        CLogMessage::preformatted(m);
    }
} // namespace swift::gui::components
