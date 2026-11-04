// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniMiscSettingsDialog.h"

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

using namespace swift::gui::settings;

CMiniMiscSettingsDialog::CMiniMiscSettingsDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle("Miscellaneous");
    this->resize(350, 150);

    QGroupBox *groupAtcSort = new QGroupBox("Sortierung ATC", this);
    m_rbSortByDistance = new QRadioButton("nach Entfernung", groupAtcSort);
    m_rbSortByCategory = new QRadioButton("nach Kategorie", groupAtcSort);

    QVBoxLayout *groupLayout = new QVBoxLayout(groupAtcSort);
    groupLayout->addWidget(m_rbSortByDistance);
    groupLayout->addWidget(m_rbSortByCategory);
    groupAtcSort->setLayout(groupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(groupAtcSort);
    mainLayout->addStretch(1);
    this->setLayout(mainLayout);

    // aktuellen Wert aus den Settings laden
    const int currentMode = m_atcSortSetting.get();
    if (currentMode == SortByCategory) { m_rbSortByCategory->setChecked(true); }
    else { m_rbSortByDistance->setChecked(true); }

    connect(m_rbSortByDistance, &QRadioButton::toggled, this, &CMiniMiscSettingsDialog::onAtcSortModeChanged);
    connect(m_rbSortByCategory, &QRadioButton::toggled, this, &CMiniMiscSettingsDialog::onAtcSortModeChanged);
}

CMiniMiscSettingsDialog::~CMiniMiscSettingsDialog() {}

void CMiniMiscSettingsDialog::onAtcSortModeChanged()
{
    const int mode =
        m_rbSortByCategory->isChecked() ? static_cast<int>(SortByCategory) : static_cast<int>(SortByDistance);
    m_atcSortSetting.setAndSave(mode);
}