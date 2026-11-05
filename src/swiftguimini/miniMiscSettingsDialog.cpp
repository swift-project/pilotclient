// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniMiscSettingsDialog.h"

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QVBoxLayout>

using namespace swift::gui::settings;

CMiniMiscSettingsDialog::CMiniMiscSettingsDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Miscellaneous"));
    this->resize(350, 200);

    QGroupBox *groupAtcSort = new QGroupBox(tr("Sortierung ATC"), this);
    m_rbSortByDistance = new QRadioButton(tr("nach Entfernung"), groupAtcSort);
    m_rbSortByCategory = new QRadioButton(tr("nach Kategorie"), groupAtcSort);

    QVBoxLayout *groupLayout = new QVBoxLayout(groupAtcSort);
    groupLayout->addWidget(m_rbSortByDistance);
    groupLayout->addWidget(m_rbSortByCategory);
    groupAtcSort->setLayout(groupLayout);

    QGroupBox *groupLanguage = new QGroupBox(tr("Sprache"), this);
    QVBoxLayout *languageLayout = new QVBoxLayout(groupLanguage);
    QLabel *languageHint = new QLabel(tr("Nach Aenderung ist ein Neustart der Anwendung erforderlich."), groupLanguage);
    languageHint->setWordWrap(true);
    m_languageSelector = new QComboBox(groupLanguage);
    m_languageSelector->addItem(tr("Systemsprache"), QStringLiteral("system"));
    m_languageSelector->addItem(tr("Deutsch"), QStringLiteral("de_DE"));
    m_languageSelector->addItem(tr("Englisch"), QStringLiteral("en_US"));
    m_languageSelector->addItem(tr("Spanisch"), QStringLiteral("es_ES"));
    m_languageSelector->addItem(tr("Franzoesisch"), QStringLiteral("fr_FR"));
    m_languageSelector->addItem(tr("Chinesisch"), QStringLiteral("zh_CN"));
    languageLayout->addWidget(m_languageSelector);
    languageLayout->addWidget(languageHint);
    groupLanguage->setLayout(languageLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(groupAtcSort);
    mainLayout->addWidget(groupLanguage);
    mainLayout->addStretch(1);
    this->setLayout(mainLayout);

    // aktuellen Wert aus den Settings laden
    const int currentMode = m_atcSortSetting.get();
    if (currentMode == SortByCategory) { m_rbSortByCategory->setChecked(true); }
    else { m_rbSortByDistance->setChecked(true); }

    const QString currentLanguage = m_languageSetting.get();
    const int languageIndex = m_languageSelector->findData(currentLanguage);
    m_languageSelector->setCurrentIndex(languageIndex >= 0 ? languageIndex : 0);

    connect(m_rbSortByDistance, &QRadioButton::toggled, this, &CMiniMiscSettingsDialog::onAtcSortModeChanged);
    connect(m_rbSortByCategory, &QRadioButton::toggled, this, &CMiniMiscSettingsDialog::onAtcSortModeChanged);
    connect(m_languageSelector, &QComboBox::currentIndexChanged, this, &CMiniMiscSettingsDialog::onLanguageChanged);
}

CMiniMiscSettingsDialog::~CMiniMiscSettingsDialog() {}

void CMiniMiscSettingsDialog::onAtcSortModeChanged()
{
    const int mode =
        m_rbSortByCategory->isChecked() ? static_cast<int>(SortByCategory) : static_cast<int>(SortByDistance);
    m_atcSortSetting.setAndSave(mode);
}

void CMiniMiscSettingsDialog::onLanguageChanged(int index)
{
    if (index < 0 || !m_languageSelector) { return; }
    const QString languageCode = m_languageSelector->itemData(index).toString();
    m_languageSetting.setAndSave(languageCode);

    QMessageBox::information(this, tr("Sprache geaendert"),
                             tr("Die neue Sprache wird nach einem Neustart der Anwendung aktiv."));
}