// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "aircraftmodelform.h"
#include "ui_aircraftmodelform.h"

using namespace swift::misc;

namespace swift::gui::editors
{
    CAircraftModelForm::CAircraftModelForm(QWidget *parent) : CForm(parent),
                                                              ui(new Ui::CAircraftModelForm)
    {
        ui->setupUi(this);
        this->setFocusProxy(ui->editor_AircraftIcao);
    }

    CAircraftModelForm::~CAircraftModelForm()
    {}

    void CAircraftModelForm::setReadOnly(bool readOnly)
    {
        ui->editor_AircraftIcao->setReadOnly(readOnly);
        ui->editor_Distributor->setReadOnly(readOnly);
        ui->editor_Livery->setReadOnly(readOnly);
        this->forceStyleSheetUpdate();
    }

    void CAircraftModelForm::setSelectOnly()
    {
        ui->editor_AircraftIcao->setSelectOnly();
        ui->editor_Distributor->setSelectOnly();
        ui->editor_Livery->setSelectOnly();
    }

    CStatusMessageList CAircraftModelForm::validate(bool withNestedForms) const
    {
        CStatusMessageList msgs;
        msgs.push_back(ui->editor_AircraftIcao->validate(withNestedForms));
        msgs.push_back(ui->editor_Distributor->validate(withNestedForms));
        msgs.push_back(ui->editor_Livery->validate(withNestedForms));
        return msgs;
    }

    CStatusMessageList CAircraftModelForm::validateLivery(bool withNestedForms) const
    {
        return ui->editor_Livery->validate(withNestedForms);
    }

    CStatusMessageList CAircraftModelForm::validateAircraftIcao(bool withNestedForms) const
    {
        return ui->editor_AircraftIcao->validate(withNestedForms);
    }

    CStatusMessageList CAircraftModelForm::validateDistributor(bool withNestedForms) const
    {
        return ui->editor_Distributor->validate(withNestedForms);
    }

    aviation::CLivery CAircraftModelForm::getLivery() const
    {
        return ui->editor_Livery->getValue();
    }

    aviation::CAircraftIcaoCode CAircraftModelForm::getAircraftIcao() const
    {
        return ui->editor_AircraftIcao->getValue();
    }

    simulation::CDistributor CAircraftModelForm::getDistributor() const
    {
        return ui->editor_Distributor->getValue();
    }

    void CAircraftModelForm::allowDrop(bool allowDrop)
    {
        ui->editor_AircraftIcao->allowDrop(allowDrop);
        ui->editor_Distributor->allowDrop(allowDrop);
        ui->editor_Livery->allowDrop(allowDrop);
    }

    bool CAircraftModelForm::setLivery(const swift::misc::aviation::CLivery &livery)
    {
        return ui->editor_Livery->setValue(livery);
    }

    bool CAircraftModelForm::setAircraftIcao(const swift::misc::aviation::CAircraftIcaoCode &icao)
    {
        return ui->editor_AircraftIcao->setValue(icao);
    }

    bool CAircraftModelForm::setDistributor(const swift::misc::simulation::CDistributor &distributor)
    {
        return ui->editor_Distributor->setValue(distributor);
    }

    void CAircraftModelForm::clear()
    {
        this->clearLivery();
        this->clearAircraftIcao();
        this->clearDistributor();
    }

    void CAircraftModelForm::clearLivery()
    {
        ui->editor_Livery->clear();
    }

    void CAircraftModelForm::clearAircraftIcao()
    {
        ui->editor_AircraftIcao->clear();
    }

    void CAircraftModelForm::clearDistributor()
    {
        ui->editor_Distributor->clear();
    }
} // ns
