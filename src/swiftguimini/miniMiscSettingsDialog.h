// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINIMISCSETTINGSDIALOG_H
#define SWIFTGUIMINI_MINIMISCSETTINGSDIALOG_H

#include <QDialog>

#include "gui/settings/minisettings.h"

class QRadioButton;
class QComboBox;
class QCheckBox;
class QSlider;
class QLabel;

//! Collection dialog for various (future) "Miscellaneous" settings of the Mini-GUI.
//! Currently included: sorting of the ATC list (by distance / by category) and Mini-GUI language.
class CMiniMiscSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor
    explicit CMiniMiscSettingsDialog(QWidget *parent = nullptr);

    //! Destructor
    ~CMiniMiscSettingsDialog() override;

private:
    //! Radio button selection has changed, save the setting
    void onAtcSortModeChanged();

    //! Language selection has changed, save the setting and show a restart hint
    void onLanguageChanged(int index);

    //! "Require flight plan for connect" checkbox has changed, save the setting
    void onRequireFlightPlanChanged();

    //! "Minimize opacity" slider has changed, save the setting and update the label
    void onMinimizeOpacityChanged(int value);

    QRadioButton *m_rbSortByDistance = nullptr;
    QRadioButton *m_rbSortByCategory = nullptr;
    QComboBox *m_languageSelector = nullptr;
    QCheckBox *m_cbRequireFlightPlan = nullptr;
    QSlider *m_slMinimizeOpacity = nullptr;
    QLabel *m_lblMinimizeOpacityValue = nullptr;

    swift::misc::CSetting<swift::gui::settings::TMiniAtcSortMode> m_atcSortSetting { this };
    swift::misc::CSetting<swift::gui::settings::TMiniLanguage> m_languageSetting { this };
    swift::misc::CSetting<swift::gui::settings::TMiniRequireFlightPlanForConnect> m_requireFlightPlanSetting { this };
    swift::misc::CSetting<swift::gui::settings::TMiniMinimizeOpacity> m_minimizeOpacitySetting { this };
};

#endif // SWIFTGUIMINI_MINIMISCSETTINGSDIALOG_H