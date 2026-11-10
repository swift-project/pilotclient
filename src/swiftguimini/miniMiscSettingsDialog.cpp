// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "miniMiscSettingsDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QSlider>
#include <QVBoxLayout>

using namespace swift::gui::settings;

CMiniMiscSettingsDialog::CMiniMiscSettingsDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Miscellaneous"));
    this->resize(350, 200);

    QGroupBox *groupAtcSort = new QGroupBox(tr("ATC sorting"), this);
    m_rbSortByDistance = new QRadioButton(tr("by distance"), groupAtcSort);
    m_rbSortByCategory = new QRadioButton(tr("by category"), groupAtcSort);

    QVBoxLayout *groupLayout = new QVBoxLayout(groupAtcSort);
    groupLayout->addWidget(m_rbSortByDistance);
    groupLayout->addWidget(m_rbSortByCategory);
    groupAtcSort->setLayout(groupLayout);

    QGroupBox *groupLanguage = new QGroupBox(tr("Language"), this);
    QVBoxLayout *languageLayout = new QVBoxLayout(groupLanguage);
    QLabel *languageHint = new QLabel(tr("A restart of the application is required after the change."), groupLanguage);
    languageHint->setWordWrap(true);
    m_languageSelector = new QComboBox(groupLanguage);
    m_languageSelector->addItem(tr("System language"), QStringLiteral("system"));
    m_languageSelector->addItem(tr("German"), QStringLiteral("de_DE"));
    m_languageSelector->addItem(tr("English"), QStringLiteral("en_US"));
    m_languageSelector->addItem(tr("Spanish"), QStringLiteral("es_ES"));
    m_languageSelector->addItem(tr("French"), QStringLiteral("fr_FR"));
    m_languageSelector->addItem(tr("Chinese"), QStringLiteral("zh_CN"));
    languageLayout->addWidget(m_languageSelector);
    languageLayout->addWidget(languageHint);
    groupLanguage->setLayout(languageLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(groupAtcSort);
    mainLayout->addWidget(groupLanguage);

    QGroupBox *groupConnect = new QGroupBox(tr("Connection"), this);
    QVBoxLayout *connectLayout = new QVBoxLayout(groupConnect);
    m_cbRequireFlightPlan = new QCheckBox(tr("Require a flight plan before connecting"), groupConnect);
    connectLayout->addWidget(m_cbRequireFlightPlan);
    groupConnect->setLayout(connectLayout);
    mainLayout->addWidget(groupConnect);

    QGroupBox *groupMinimize = new QGroupBox(tr("Minimize"), this);
    QVBoxLayout *minimizeLayout = new QVBoxLayout(groupMinimize);
    QHBoxLayout *opacityRowLayout = new QHBoxLayout();
    m_slMinimizeOpacity = new QSlider(Qt::Horizontal, groupMinimize);
    m_slMinimizeOpacity->setRange(10, 100);
    m_slMinimizeOpacity->setSingleStep(5);
    m_slMinimizeOpacity->setTickInterval(10);
    m_slMinimizeOpacity->setTickPosition(QSlider::TicksBelow);
    m_lblMinimizeOpacityValue = new QLabel(groupMinimize);
    m_lblMinimizeOpacityValue->setMinimumWidth(40);
    opacityRowLayout->addWidget(new QLabel(tr("Opacity:"), groupMinimize));
    opacityRowLayout->addWidget(m_slMinimizeOpacity, 1);
    opacityRowLayout->addWidget(m_lblMinimizeOpacityValue);
    minimizeLayout->addLayout(opacityRowLayout);
    groupMinimize->setLayout(minimizeLayout);
    mainLayout->addWidget(groupMinimize);

    mainLayout->addStretch(1);
    this->setLayout(mainLayout);

    // load the current value from settings
    const int currentMode = m_atcSortSetting.get();
    if (currentMode == SortByCategory) { m_rbSortByCategory->setChecked(true); }
    else { m_rbSortByDistance->setChecked(true); }

    const QString currentLanguage = m_languageSetting.get();
    const int languageIndex = m_languageSelector->findData(currentLanguage);
    m_languageSelector->setCurrentIndex(languageIndex >= 0 ? languageIndex : 0);

    m_cbRequireFlightPlan->setChecked(m_requireFlightPlanSetting.get());

    const int currentOpacity = m_minimizeOpacitySetting.get();
    m_slMinimizeOpacity->setValue(currentOpacity);
    m_lblMinimizeOpacityValue->setText(QStringLiteral("%1 %").arg(currentOpacity));

    connect(m_rbSortByDistance, &QRadioButton::toggled, this, &CMiniMiscSettingsDialog::onAtcSortModeChanged);
    connect(m_rbSortByCategory, &QRadioButton::toggled, this, &CMiniMiscSettingsDialog::onAtcSortModeChanged);
    connect(m_languageSelector, &QComboBox::currentIndexChanged, this, &CMiniMiscSettingsDialog::onLanguageChanged);
    connect(m_cbRequireFlightPlan, &QCheckBox::toggled, this, &CMiniMiscSettingsDialog::onRequireFlightPlanChanged);
    connect(m_slMinimizeOpacity, &QSlider::valueChanged, this, &CMiniMiscSettingsDialog::onMinimizeOpacityChanged);
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

    QMessageBox::information(this, tr("Language changed"),
                             tr("The new language will become active after restarting the application."));
}

void CMiniMiscSettingsDialog::onRequireFlightPlanChanged()
{
    m_requireFlightPlanSetting.setAndSave(m_cbRequireFlightPlan->isChecked());
}

void CMiniMiscSettingsDialog::onMinimizeOpacityChanged(int value)
{
    m_lblMinimizeOpacityValue->setText(QStringLiteral("%1 %").arg(value));
    m_minimizeOpacitySetting.setAndSave(value);
}