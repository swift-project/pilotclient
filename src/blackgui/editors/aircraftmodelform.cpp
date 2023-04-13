/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "aircraftmodelform.h"
#include "ui_aircraftmodelform.h"

using namespace BlackMisc;

namespace BlackGui::Editors
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

    Aviation::CLivery CAircraftModelForm::getLivery() const
    {
        return ui->editor_Livery->getValue();
    }

    Aviation::CAircraftIcaoCode CAircraftModelForm::getAircraftIcao() const
    {
        return ui->editor_AircraftIcao->getValue();
    }

    Simulation::CDistributor CAircraftModelForm::getDistributor() const
    {
        return ui->editor_Distributor->getValue();
    }

    void CAircraftModelForm::allowDrop(bool allowDrop)
    {
        ui->editor_AircraftIcao->allowDrop(allowDrop);
        ui->editor_Distributor->allowDrop(allowDrop);
        ui->editor_Livery->allowDrop(allowDrop);
    }

    bool CAircraftModelForm::setLivery(const BlackMisc::Aviation::CLivery &livery)
    {
        return ui->editor_Livery->setValue(livery);
    }

    bool CAircraftModelForm::setAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao)
    {
        return ui->editor_AircraftIcao->setValue(icao);
    }

    bool CAircraftModelForm::setDistributor(const BlackMisc::Simulation::CDistributor &distributor)
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
