/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/colorselector.h"
#include "blackgui/filters/filterbarbuttons.h"
#include "blackgui/filters/liveryfilterbar.h"
#include "blackgui/models/liveryfilter.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "ui_liveryfilterbar.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QSlider>
#include <QtGlobal>

using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;
using namespace BlackGui::Components;

namespace BlackGui::Filters
{
    CLiveryFilterBar::CLiveryFilterBar(QWidget *parent) :
        CFilterWidget(parent),
        ui(new Ui::CLiveryFilterBar)
    {
        ui->setupUi(this);
        this->setButtonsAndCount(ui->filter_Buttons);

        connect(ui->le_AirlineIcaoCode, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_Description, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_LiveryCode, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_Id, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->color_Fuselage, &CColorSelector::colorChanged, this, &CLiveryFilterBar::onColorChanged);
        connect(ui->color_Tail, &CColorSelector::colorChanged, this, &CLiveryFilterBar::onColorChanged);
        connect(ui->cb_Airlines, &QCheckBox::released, this, &CFilterWidget::triggerFilter);
        connect(ui->cb_Colors, &QCheckBox::released, this, &CFilterWidget::triggerFilter);
        connect(ui->hs_ColorDistance, &QSlider::valueChanged, this, &CLiveryFilterBar::onColorDistanceChanged);

        ui->le_AirlineIcaoCode->setValidator(new CUpperCaseValidator(ui->le_AirlineIcaoCode));
        ui->le_LiveryCode->setValidator(new CUpperCaseValidator(ui->le_LiveryCode));
        ui->le_Id->setValidator(new QIntValidator(ui->le_Id));

        // reset form
        this->clearForm();
    }

    CLiveryFilterBar::~CLiveryFilterBar()
    { }

    std::unique_ptr<Models::IModelFilter<CLiveryList> > CLiveryFilterBar::createModelFilter() const
    {
        const double maxColorDistance = ui->hs_ColorDistance->value() / 100.0;
        return std::make_unique<CLiveryFilter>(
                    convertDbId(ui->le_Id->text()),
                    ui->le_LiveryCode->text(),
                    ui->le_Description->text(),
                    ui->le_AirlineIcaoCode->text(),
                    ui->color_Fuselage->getColor(),
                    ui->color_Tail->getColor(),
                    maxColorDistance,
                    ui->cb_Colors->isChecked(),
                    ui->cb_Airlines->isChecked()
                );
    }

    void CLiveryFilterBar::filter(const CLivery &livery)
    {
        this->clearForm();
        bool filter = false;
        if (livery.hasCombinedCode())
        {
            ui->le_LiveryCode->setText(livery.getCombinedCode());
            filter = true;
        }
        if (livery.hasValidAirlineDesignator())
        {
            ui->le_AirlineIcaoCode->setText(livery.getAirlineIcaoCodeDesignator());
            filter = true;
        }
        if (livery.isColorLivery())
        {
            ui->color_Fuselage->setColor(livery.getColorFuselage());
            ui->color_Tail->setColor(livery.getColorTail());
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

    void CLiveryFilterBar::filter(const CAirlineIcaoCode &airlineIcao)
    {
        if (!airlineIcao.hasValidDesignator()) { return; }
        ui->le_AirlineIcaoCode->setText(airlineIcao.getVDesignator());
        ui->filter_Buttons->clickButton(CFilterBarButtons::Filter);
    }

    void CLiveryFilterBar::onRowCountChanged(int count, bool withFilter)
    {
        ui->filter_Buttons->onRowCountChanged(count, withFilter);
    }

    void CLiveryFilterBar::clearForm()
    {
        ui->le_Id->clear();
        ui->le_AirlineIcaoCode->clear();
        ui->le_LiveryCode->clear();
        ui->le_Description->clear();
        ui->color_Fuselage->clear();
        ui->color_Tail->clear();
        ui->hs_ColorDistance->setValue(qRound(0.25 *  100.0));
        ui->cb_Airlines->setChecked(true);
        ui->cb_Colors->setChecked(true);
    }

    void CLiveryFilterBar::onColorChanged(const BlackMisc::CRgbColor &color)
    {
        this->triggerFilter();
        Q_UNUSED(color);
    }

    void CLiveryFilterBar::onColorDistanceChanged(int distance)
    {
        this->triggerFilter();
        Q_UNUSED(distance);
    }

} // ns
