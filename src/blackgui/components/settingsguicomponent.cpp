/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
#include <QMessageBox>

using namespace BlackMisc;
using namespace BlackGui::Settings;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CSettingsGuiComponent::CSettingsGuiComponent(QWidget *parent) : QFrame(parent),
                                                                    BlackGui::CSingleApplicationUi(this),
                                                                    ui(new Ui::CSettingsGuiComponent)
    {
        ui->setupUi(this);

        ui->cb_SettingsGuiWidgetStyle->clear();
        ui->cb_SettingsGuiWidgetStyle->insertItems(0, QStyleFactory::keys());

        // Widget style
        connect(ui->hs_SettingsGuiOpacity, &QSlider::valueChanged, this, &CSettingsGuiComponent::changedWindowsOpacity);
        connect(ui->cb_SettingsGuiWidgetStyle, qOverload<const QString &>(&QComboBox::currentIndexChanged), this, &CSettingsGuiComponent::widgetStyleChanged, Qt::QueuedConnection);

        ui->comp_SettingsFonts->setStyleSheetDefaultColor();

        // selection
        connect(ui->rb_PreferExtendedSelection, &QRadioButton::released, this, &CSettingsGuiComponent::selectionChanged);
        connect(ui->rb_PreferMultiSelection, &QRadioButton::released, this, &CSettingsGuiComponent::selectionChanged);

        this->guiSettingsChanged();
    }

    CSettingsGuiComponent::~CSettingsGuiComponent()
    {}

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
        if (ui->rb_PreferExtendedSelection->isChecked())
        {
            sm = QAbstractItemView::ExtendedSelection;
        }
        else if (ui->rb_PreferMultiSelection->isChecked())
        {
            sm = QAbstractItemView::MultiSelection;
        }
        if (sm == m_guiSettings.get().getPreferredSelection()) { return; }
        const CStatusMessage m = m_guiSettings.setAndSaveProperty(CGeneralGuiSettings::IndexPreferredSelection, CVariant::fromValue(sm));
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

        /** because of crash (chnage style crashes UI) we require restart
        const int ret = QMessageBox::information(this,
                        tr("Change style?"),
                        tr("Changing style is slow.\nThe GUI will hang for some seconds.\nDo you want to save your changes?"),
                        QMessageBox::Ok | QMessageBox::Cancel);

        **/
        const int ret = QMessageBox::information(this,
                                                 tr("Change style?"),
                                                 tr("Changing style requires a restart.\nChanges will be visible a the next start.\nDo you want to save your changes?"),
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
} // ns
