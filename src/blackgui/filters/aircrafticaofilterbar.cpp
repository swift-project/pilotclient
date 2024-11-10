// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/aircraftcombinedtypeselector.h"
#include "blackgui/filters/aircrafticaofilterbar.h"
#include "blackgui/filters/filterbarbuttons.h"
#include "blackgui/models/aircrafticaofilter.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/aircrafticaocodelist.h"
#include "ui_aircrafticaofilterbar.h"

#include <QLineEdit>
#include <QString>
#include <QCompleter>

using namespace swift::misc::aviation;
using namespace BlackGui;
using namespace BlackGui::Models;
using namespace BlackCore;

namespace BlackGui::Filters
{
    CAircraftIcaoFilterBar::CAircraftIcaoFilterBar(QWidget *parent) : CFilterWidget(parent),
                                                                      ui(new Ui::CAircraftIcaoFilterBar)
    {
        ui->setupUi(this);
        this->setButtonsAndCount(ui->filter_Buttons);
        connect(ui->le_Designator, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_Manufacturer, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_Description, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_Id, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_Family, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);

        ui->le_Designator->setValidator(new CUpperCaseValidator(ui->le_Designator));
        ui->le_Family->setValidator(new CUpperCaseValidator(ui->le_Family));
        ui->le_Manufacturer->setValidator(new CUpperCaseValidator(ui->le_Manufacturer));
        ui->le_Id->setValidator(new QIntValidator(ui->le_Id));

        if (sGui && sGui->hasWebDataServices())
        {
            connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbAircraftIcaoRead, this, &CAircraftIcaoFilterBar::initCompleters);
            this->initCompleters();
        }

        // reset form
        this->clearForm();
    }

    CAircraftIcaoFilterBar::~CAircraftIcaoFilterBar()
    {}

    std::unique_ptr<BlackGui::Models::IModelFilter<CAircraftIcaoCodeList>> CAircraftIcaoFilterBar::createModelFilter() const
    {
        return std::make_unique<CAircraftIcaoFilter>(
            convertDbId(ui->le_Id->text()),
            ui->le_Designator->text(),
            ui->le_Family->text(),
            ui->le_Manufacturer->text(),
            ui->le_Description->text(),
            ui->combinedType_Selector->getCombinedType());
    }

    void CAircraftIcaoFilterBar::filter(const CAircraftIcaoCode &icao)
    {
        this->clearForm();
        bool filter = false;
        if (icao.hasDesignator())
        {
            ui->le_Designator->setText(icao.getDesignator());
            filter = true;
        }
        if (icao.hasManufacturer())
        {
            ui->le_Manufacturer->setText(icao.getManufacturer());
            filter = true;
        }
        if (icao.hasModelDescription())
        {
            ui->le_Description->setText("*" + icao.getModelDescription() + "*");
            filter = true;
        }
        if (filter)
        {
            ui->filter_Buttons->clickButton(CFilterBarButtons::Filter);
        }
        else
        {
            ui->filter_Buttons->clickButton(CFilterBarButtons::RemoveFilter);
        }
    }

    void CAircraftIcaoFilterBar::hideDescriptionField(bool hide)
    {
        ui->le_Description->setVisible(!hide);
    }

    void CAircraftIcaoFilterBar::onRowCountChanged(int count, bool withFilter)
    {
        ui->filter_Buttons->onRowCountChanged(count, withFilter);
    }

    void CAircraftIcaoFilterBar::clearForm()
    {
        ui->le_Id->clear();
        ui->le_Designator->clear();
        ui->le_Manufacturer->clear();
        ui->le_Description->clear();
        ui->combinedType_Selector->clear();
    }

    void CAircraftIcaoFilterBar::initCompleters()
    {
        if (m_hasCompleters) { return; }
        if (!sGui || !sGui->hasWebDataServices()) { return; }

        if (sGui->getWebDataServices()->getAircraftIcaoCodesCount() < 1) { return; }
        m_hasCompleters = true;
        const CAircraftIcaoCodeList codes = sGui->getWebDataServices()->getAircraftIcaoCodes();
        const QStringList designators = codes.allDesignators().values();
        ui->le_Designator->setCompleter(new QCompleter(designators, ui->le_Designator));
        const QStringList families = codes.allFamilies().values();
        ui->le_Family->setCompleter(new QCompleter(families, ui->le_Family));
        const QStringList manufacturers = codes.allManufacturers().values();
        ui->le_Manufacturer->setCompleter(new QCompleter(manufacturers, ui->le_Manufacturer));
    }
} // nss
