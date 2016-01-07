/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "liveryfilterbar.h"
#include "ui_liveryfilterbar.h"
#include "blackgui/uppercasevalidator.h"

using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Filters
    {
        CLiveryFilterBar::CLiveryFilterBar(QWidget *parent) :
            CFilterWidget(parent),
            ui(new Ui::CLiveryFilterBar)
        {
            ui->setupUi(this);
            this->setButtonsAndCount(this->ui->filter_Buttons);

            connect(ui->le_AirlineIcaoCode, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_Description, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_LiveryCode, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->color_Fuselage, &CColorSelector::colorChanged, this, &CLiveryFilterBar::ps_colorChanged);
            connect(ui->color_Tail, &CColorSelector::colorChanged, this, &CLiveryFilterBar::ps_colorChanged);
            connect(ui->cb_Airlines, &QCheckBox::released, this, &CFilterWidget::triggerFilter);
            connect(ui->cb_Colors, &QCheckBox::released, this, &CFilterWidget::triggerFilter);
            connect(ui->hs_ColorDistance, &QSlider::valueChanged, this, &CLiveryFilterBar::ps_colorDistanceChanged);

            CUpperCaseValidator *ucv = new CUpperCaseValidator(this);
            ui->le_AirlineIcaoCode->setValidator(ucv);
            ui->le_LiveryCode->setValidator(ucv);
        }

        CLiveryFilterBar::~CLiveryFilterBar()
        { }

        std::unique_ptr<BlackGui::Models::IModelFilter<CLiveryList> > CLiveryFilterBar::createModelFilter() const
        {
            double maxColorDistance = ui->hs_ColorDistance->value() / 100.0;
            return std::unique_ptr<CLiveryFilter>(
                       new CLiveryFilter(
                           this->ui->le_LiveryCode->text(),
                           this->ui->le_Description->text(),
                           this->ui->le_AirlineIcaoCode->text(),
                           this->ui->color_Fuselage->getColor(),
                           this->ui->color_Tail->getColor(),
                           maxColorDistance,
                           this->ui->cb_Colors->isChecked(),
                           this->ui->cb_Airlines->isChecked()
                       ));
        }

        void CLiveryFilterBar::setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider)
        {
            Q_UNUSED(webDataReaderProvider);
        }

        void CLiveryFilterBar::filter(const CLivery &livery)
        {
            if (livery.hasCombinedCode())
            {
                this->ui->le_LiveryCode->setText(livery.getCombinedCode());
            }
            else if (livery.hasValidAirlineDesignator())
            {
                this->ui->le_AirlineIcaoCode->setText(livery.getAirlineIcaoCodeDesignator());
            }
            else
            {
                return;
            }
            this->ui->filter_Buttons->clickButton(CFilterBarButtons::Filter);
        }

        void CLiveryFilterBar::onRowCountChanged(int count, bool withFilter)
        {
            this->ui->filter_Buttons->onRowCountChanged(count, withFilter);
        }

        void CLiveryFilterBar::clearForm()
        {
            ui->le_AirlineIcaoCode->clear();
            ui->le_LiveryCode->clear();
            ui->le_Description->clear();
            ui->color_Fuselage->clear();
            ui->color_Tail->clear();
            ui->hs_ColorDistance->setValue(0.25 *  100.0);
        }

        void CLiveryFilterBar::ps_colorChanged(const BlackMisc::CRgbColor &color)
        {
            this->triggerFilter();
            Q_UNUSED(color);
        }

        void CLiveryFilterBar::ps_colorDistanceChanged(int distance)
        {
            this->triggerFilter();
            Q_UNUSED(distance);
        }

    } // ns
} // ns
