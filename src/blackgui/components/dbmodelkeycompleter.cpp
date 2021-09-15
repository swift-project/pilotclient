/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
