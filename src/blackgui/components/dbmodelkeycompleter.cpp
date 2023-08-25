// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "dbmodelkeycompleter.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackcore/webdataservices.h"
#include <QCompleter>

using namespace BlackCore;
using namespace BlackMisc::Simulation;

namespace BlackGui::Components
{
    CDbModelKeyCompleter::CDbModelKeyCompleter(QWidget *parent) : QLineEdit(parent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        Q_ASSERT_X(sGui->hasWebDataServices(), Q_FUNC_INFO, "Need web services");

        this->setValidator(new CUpperCaseValidator(this));
        this->initCompleter();
        connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbModelsRead, this, &CDbModelKeyCompleter::onModelsRead);
    }

    void CDbModelKeyCompleter::setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator)
    {
        if (m_simulator == simulator) { return; }
        m_simulator = simulator;
        this->initCompleter();
    }

    CAircraftModel CDbModelKeyCompleter::getAircraftModel() const
    {
        return sGui->getWebDataServices()->getModelForModelString(this->text().toUpper().trimmed());
    }

    void CDbModelKeyCompleter::onModelsRead()
    {
        this->initCompleter();
    }

    void CDbModelKeyCompleter::initCompleter()
    {
        const QStringList models(sGui->getWebDataServices()->getModelCompleterStrings(true, m_simulator));
        if (models.isEmpty()) { return; }
        this->setCompleter(new QCompleter(models, this));
    }
} // ns
