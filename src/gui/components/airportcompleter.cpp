// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "airportcompleter.h"

#include <QCompleter>

#include "ui_airportcompleter.h"

#include "core/webdataservices.h"
#include "gui/guiapplication.h"
#include "gui/uppercasevalidator.h"
#include "misc/aviation/airportlist.h"

using namespace swift::core;
using namespace swift::misc::aviation;

namespace swift::gui::components
{
    CAirportCompleter::CAirportCompleter(QWidget *parent) : QFrame(parent), ui(new Ui::CAirportCompleter)
    {
        ui->setupUi(this);
        ui->le_Icao->setValidator(new CUpperCaseValidator(ui->le_Icao));

        connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbAllDataRead, this,
                &CAirportCompleter::onAirportsChanged);
        connect(ui->le_Icao, &QLineEdit::editingFinished, this, &CAirportCompleter::onIcaoChanged);
        connect(ui->le_Name, &QLineEdit::editingFinished, this, &CAirportCompleter::onNameChanged);
        connect(ui->le_Location, &QLineEdit::editingFinished, this, &CAirportCompleter::onLocationChanged);
        this->onAirportsChanged();
    }

    CAirportCompleter::~CAirportCompleter() = default;

    void CAirportCompleter::setAirport(const CAirport &airport)
    {
        if (m_current == airport) { return; }
        m_current = airport;
        if (airport.getIcaoAsString() != ui->le_Icao->text()) { ui->le_Icao->setText(airport.getIcaoAsString()); }
        if (airport.getDescriptiveName() != ui->le_Name->text()) { ui->le_Name->setText(airport.getDescriptiveName()); }
        if (airport.getLocation() != ui->le_Location->text()) { ui->le_Location->setText(airport.getLocation()); }
        emit this->changedAirport(m_current);
    }

    QString CAirportCompleter::getIcaoText() const { return ui->le_Icao->text().trimmed().toUpper(); }

    void CAirportCompleter::setReadOnly(bool readOnly)
    {
        ui->le_Icao->setReadOnly(readOnly);
        ui->le_Location->setReadOnly(readOnly);
        ui->le_Name->setReadOnly(readOnly);
    }

    void CAirportCompleter::clear()
    {
        m_current = CAirport();
        ui->le_Icao->clear();
        ui->le_Location->clear();
        ui->le_Name->clear();
    }

    void CAirportCompleter::onIcaoChanged()
    {
        const QString icao(this->getIcaoText());
        if (m_current.getIcaoAsString() == icao) { return; }
        const CAirport airport(sGui->getWebDataServices()->getAirportForIcaoDesignator(icao));
        this->setAirport(airport);
    }

    void CAirportCompleter::onNameChanged()
    {
        const QString name(ui->le_Name->text());
        if (m_current.getDescriptiveName() == name) { return; }
        const CAirport airport(sGui->getWebDataServices()->getAirportForNameOrLocation(name));
        this->setAirport(airport);
    }

    void CAirportCompleter::onLocationChanged()
    {
        const QString location(ui->le_Location->text());
        if (m_current.getLocation() == location) { return; }
        const CAirport airport(sGui->getWebDataServices()->getAirportForNameOrLocation(location));
        this->setAirport(airport);
    }

    void CAirportCompleter::onAirportsChanged()
    {
        if (!sGui || !sGui->hasWebDataServices()) { return; }
        const CAirportList airports = sGui->getWebDataServices()->getAirports();
        ui->le_Icao->setCompleter(new QCompleter(airports.allIcaoCodes(true), ui->le_Icao));
        ui->le_Name->setCompleter(new QCompleter(airports.allDescriptivesNames(true), ui->le_Name));
        ui->le_Location->setCompleter(
            new QCompleter(airports.allLocationsPlusOptionalDescription(true), ui->le_Location));

        if (ui->le_Icao->completer()->popup())
        {
            ui->le_Icao->completer()->setCaseSensitivity(Qt::CaseInsensitive);
            ui->le_Icao->completer()->popup()->setObjectName("AirportCompleter.Icao");
            const int w5chars =
                ui->le_Icao->completer()->popup()->fontMetrics().size(Qt::TextSingleLine, "FooBa").width();
            ui->le_Icao->completer()->popup()->setMinimumWidth(w5chars * 5);
        }
        if (ui->le_Name->completer()->popup())
        {
            ui->le_Name->completer()->setCaseSensitivity(Qt::CaseInsensitive);
            ui->le_Name->completer()->popup()->setObjectName("AirportCompleter.Name");
            const int w5chars =
                ui->le_Name->completer()->popup()->fontMetrics().size(Qt::TextSingleLine, "FooBa").width();
            ui->le_Name->completer()->popup()->setMinimumWidth(w5chars * 10);
        }
        if (ui->le_Location->completer()->popup())
        {
            ui->le_Location->completer()->setCaseSensitivity(Qt::CaseInsensitive);
            ui->le_Location->completer()->popup()->setObjectName("AirportCompleter.Location");
            const int w5chars =
                ui->le_Location->completer()->popup()->fontMetrics().size(Qt::TextSingleLine, "FooBa").width();
            ui->le_Location->completer()->popup()->setMinimumWidth(w5chars * 10);
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
} // namespace swift::gui::components
