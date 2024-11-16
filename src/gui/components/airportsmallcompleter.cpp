// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "airportsmallcompleter.h"

#include <QCompleter>

#include "ui_airportsmallcompleter.h"

#include "core/webdataservices.h"
#include "gui/components/airportdialog.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "gui/uppercasevalidator.h"
#include "misc/aviation/airportlist.h"

using namespace swift::core;
using namespace swift::misc::aviation;

namespace swift::gui::components
{
    CAirportSmallCompleter::CAirportSmallCompleter(QWidget *parent) : QFrame(parent), ui(new Ui::CAirportSmallCompleter)
    {
        ui->setupUi(this);
        ui->le_Icao->setValidator(new CUpperCaseValidator(ui->le_Icao));
        this->setFocusProxy(ui->le_Icao);

        connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbAllDataRead, this,
                &CAirportSmallCompleter::onAirportsChanged);
        connect(ui->le_Icao, &QLineEdit::editingFinished, this, &CAirportSmallCompleter::onIcaoChanged);
        connect(ui->le_Icao, &QLineEdit::editingFinished, this, &CAirportSmallCompleter::editingFinished);
        connect(ui->pb_Dialog, &QPushButton::clicked, this, &CAirportSmallCompleter::showAirportsDialog);
        this->onAirportsChanged();
    }

    CAirportSmallCompleter::~CAirportSmallCompleter() {}

    void CAirportSmallCompleter::setAirport(const CAirport &airport)
    {
        if (m_current == airport) { return; }
        m_current = airport;
        if (airport.getIcaoAsString() != ui->le_Icao->text()) { ui->le_Icao->setText(airport.getIcaoAsString()); }
        emit this->changedAirport(m_current);
    }

    void CAirportSmallCompleter::setAirportIcaoCode(const CAirportIcaoCode &airportCode)
    {
        if (sGui && sGui->hasWebDataServices())
        {
            const CAirport airport = sGui->getWebDataServices()->getAirportForIcaoDesignator(airportCode.asString());
            if (!airport.isNull())
            {
                this->setAirport(airport);
                return;
            }
        }
        if (ui->le_Icao->text() != airportCode.asString()) { ui->le_Icao->setText(airportCode.asString()); }
    }

    CAirportIcaoCode CAirportSmallCompleter::getAirportIcaoCode() const
    {
        if (m_current.hasValidIcaoCode()) { return m_current.getIcao(); }
        return CAirportIcaoCode(this->getIcaoText());
    }

    QString CAirportSmallCompleter::getIcaoText() const { return ui->le_Icao->text().trimmed().toUpper(); }

    bool CAirportSmallCompleter::isEmpty() const { return ui->le_Icao->text().isEmpty(); }

    void CAirportSmallCompleter::setReadOnly(bool readOnly)
    {
        ui->le_Icao->setReadOnly(readOnly);
        ui->pb_Dialog->setVisible(!readOnly);
    }

    void CAirportSmallCompleter::clear()
    {
        ui->le_Icao->clear();
        m_current = CAirport();
    }

    void CAirportSmallCompleter::clearIfInvalidCode(bool strictValidation)
    {
        const CAirportIcaoCode icao = this->getAirportIcaoCode();
        if (!icao.hasValidIcaoCode(strictValidation)) { this->clear(); }
    }

    void CAirportSmallCompleter::onIcaoChanged()
    {
        if (!sGui || !sGui->hasWebDataServices()) { return; }
        const CAirport airport = sGui->getWebDataServices()->getAirportForIcaoDesignator(this->getIcaoText());
        if (!airport.isNull()) { this->setAirport(airport); }
    }

    void CAirportSmallCompleter::onAirportsChanged()
    {
        if (!sGui || !sGui->hasWebDataServices()) { return; }
        const CAirportList airports = sGui->getWebDataServices()->getAirports();
        ui->le_Icao->setCompleter(new QCompleter(airports.allIcaoCodes(true), ui->le_Icao));

        if (ui->le_Icao->completer()->popup())
        {
            ui->le_Icao->completer()->popup()->setObjectName("AirportSmallCompleter.ICAO");
            ui->le_Icao->completer()->popup()->setMinimumWidth(75);
            ui->le_Icao->completer()->setCaseSensitivity(Qt::CaseInsensitive);
        }

        // turn into airport when it was not possible before
        if (m_current.isNull())
        {
            const QString icao = this->getIcaoText();
            if (CAirportIcaoCode::isValidIcaoDesignator(icao, true))
            {
                const CAirport airport = sGui->getWebDataServices()->getAirportForIcaoDesignator(icao);
                if (!airport.isNull()) { this->setAirport(airport); }
            }
        }
    }

    void CAirportSmallCompleter::showAirportsDialog()
    {
        if (!m_airportsDialog)
        {
            m_airportsDialog.reset(new CAirportDialog(this));
            m_airportsDialog->setModal(true);
        }
        m_airportsDialog->setAirport(this->getAirport());
        const int rv = m_airportsDialog->exec();
        if (rv != QDialog::Accepted) { return; }

        const CAirport airport = m_airportsDialog->getAirport();
        if (airport.isNull()) { return; }
        this->setAirport(airport);
    }
} // namespace swift::gui::components
