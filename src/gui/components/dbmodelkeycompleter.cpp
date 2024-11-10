// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "dbmodelkeycompleter.h"
#include "gui/guiapplication.h"
#include "gui/uppercasevalidator.h"
#include "core/webdataservices.h"
#include <QCompleter>

using namespace swift::core;
using namespace swift::misc::simulation;

namespace swift::gui::components
{
    CDbModelKeyCompleter::CDbModelKeyCompleter(QWidget *parent) : QLineEdit(parent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        Q_ASSERT_X(sGui->hasWebDataServices(), Q_FUNC_INFO, "Need web services");

        this->setValidator(new CUpperCaseValidator(this));
        this->initCompleter();
        connect(sGui->getWebDataServices(), &CWebDataServices::swiftDbModelsRead, this, &CDbModelKeyCompleter::onModelsRead);
    }

    void CDbModelKeyCompleter::setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator)
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
