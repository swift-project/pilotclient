// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINIMISCSETTINGSDIALOG_H
#define SWIFTGUIMINI_MINIMISCSETTINGSDIALOG_H

#include <QDialog>

#include "gui/settings/minisettings.h"

class QRadioButton;
class QComboBox;

//! Sammel-Dialog fuer verschiedene, noch kommende "Miscellaneous"-Einstellungen der Mini-GUI.
//! Aktuell enthalten: Sortierung der ATC-Liste (nach Entfernung / nach Kategorie) und Sprache der Mini-GUI.
class CMiniMiscSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor
    explicit CMiniMiscSettingsDialog(QWidget *parent = nullptr);

    //! Destructor
    ~CMiniMiscSettingsDialog() override;

private:
    //! Radiobutton-Auswahl hat sich geaendert, Einstellung speichern
    void onAtcSortModeChanged();

    //! Sprachauswahl hat sich geaendert, Einstellung speichern und Hinweis auf Neustart anzeigen
    void onLanguageChanged(int index);

    QRadioButton *m_rbSortByDistance = nullptr;
    QRadioButton *m_rbSortByCategory = nullptr;
    QComboBox *m_languageSelector = nullptr;

    swift::misc::CSetting<swift::gui::settings::TMiniAtcSortMode> m_atcSortSetting { this };
    swift::misc::CSetting<swift::gui::settings::TMiniLanguage> m_languageSetting { this };
};

#endif // SWIFTGUIMINI_MINIMISCSETTINGSDIALOG_H