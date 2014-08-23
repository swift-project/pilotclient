/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorcomponent.h"
#include "ui_simulatorcomponent.h"
#include "blackmisc/iconlist.h"

namespace BlackGui
{
    namespace Components
    {
        CSimulatorComponent::CSimulatorComponent(QWidget *parent) :
            QTabWidget(parent), ui(new Ui::CSimulatorComponent)
        {
            ui->setupUi(this);
            this->ui->tvp_LiveData->setIconMode(true);
            this->addOrUpdateByName("info", "no data yet", CIcons::StandardIconWarning16);
        }

        CSimulatorComponent::~CSimulatorComponent()
        {
            delete ui;
        }

        void CSimulatorComponent::addOrUpdateByName(const QString &name, const QString &value, const CIcon &icon)
        {
            this->ui->tvp_LiveData->addOrUpdateByName(name, value, icon);
        }

        void CSimulatorComponent::addOrUpdateByName(const QString &name, const QString &value, CIcons::IconIndex iconIndex)
        {
            this->addOrUpdateByName(name, value, CIconList::iconForIndex(iconIndex));
        }

        int CSimulatorComponent::rowCount() const
        {
            return this->ui->tvp_LiveData->rowCount();
        }

        void CSimulatorComponent::clear()
        {
            this->ui->tvp_LiveData->clear();
        }
    }
}
