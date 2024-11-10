// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "countryselector.h"
#include "ui_countryselector.h"

#include "gui/guiapplication.h"
#include "gui/uppercasevalidator.h"
#include "core/webdataservices.h"
#include "misc/countrylist.h"
#include <QCompleter>

using namespace swift::misc;
using namespace swift::core;

namespace swift::gui::components
{
    CCountrySelector::CCountrySelector(QWidget *parent) : QFrame(parent),
                                                          ui(new Ui::CCountrySelector)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        ui->setupUi(this);
        ui->le_Iso->setValidator(new CUpperCaseValidator(ui->le_Iso));

        connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbAllDataRead, this, &CCountrySelector::onCountriesLoaded);
        connect(ui->cb_Country, &QComboBox::currentTextChanged, this, &CCountrySelector::onCountryNameChanged);
        connect(ui->le_Iso, &QLineEdit::editingFinished, this, &CCountrySelector::onIsoChanged);

        this->onCountriesLoaded();
    }

    CCountrySelector::~CCountrySelector()
    {}

    void CCountrySelector::setCountry(const CCountry &country)
    {
        if (country == m_current) { return; }
        m_current = country;
        if (country.getIsoCode() != ui->le_Iso->text()) { ui->le_Iso->setText(country.getIsoCode()); }
        if (country.getName() != ui->cb_Country->currentText()) { ui->cb_Country->setCurrentText(country.getName()); }
        emit this->countryChanged(m_current);
    }

    void CCountrySelector::onCountriesLoaded()
    {
        if (!sGui || !sGui->hasWebDataServices()) { return; }
        const CCountryList countries = sGui->getWebDataServices()->getCountries();

        ui->le_Iso->setCompleter(new QCompleter(countries.toIsoList(true)));
        ui->cb_Country->clear();
        ui->cb_Country->addItems(countries.toNameList(true));
    }

    void CCountrySelector::onIsoChanged()
    {
        if (!sGui || !sGui->hasWebDataServices()) { return; }
        const QString iso(ui->le_Iso->text().trimmed().toUpper());
        if (m_current.getIsoCode() == iso) { return; }
        this->setCountry(sGui->getWebDataServices()->getCountryForIsoCode(iso));
    }

    void CCountrySelector::onCountryNameChanged(const QString &name)
    {
        if (!sGui || !sGui->hasWebDataServices()) { return; }
        if (m_current.getName() == name) { return; }
        this->setCountry(sGui->getWebDataServices()->getCountryForName(name));
    }
} // ns
