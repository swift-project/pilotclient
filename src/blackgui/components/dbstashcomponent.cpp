/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbstashcomponent.h"
#include "ui_dbstashcomponent.h"
#include "blackmisc/icons.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Components
    {
        CDbStashComponent::CDbStashComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbStashComponent)
        {
            ui->setupUi(this);
            this->ui->tvp_StashAircraftModels->setAircraftModelMode(CAircraftModelListModel::StashModel);

            connect(this->ui->pb_Unstash, &QPushButton::pressed, this, &CDbStashComponent::ps_onUnstashPressed);
            connect(this->ui->pb_Validate, &QPushButton::pressed, this, &CDbStashComponent::ps_onValidatePressed);

            ui->tvp_StashAircraftModels->setCustomMenu(new CStashModelsMenu(this, true));
        }

        CDbStashComponent::~CDbStashComponent()
        { }

        void CDbStashComponent::setProvider(IWebDataServicesProvider *provider)
        {
            CWebDataServicesAware::setProvider(provider);
        }

        void CDbStashComponent::gracefulShutdown()
        {
            // shutdown
        }

        CStatusMessage CDbStashComponent::validateStashModel(const CAircraftModel &model) const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation()}));
            if (this->ui->tvp_StashAircraftModels->container().containsModelStringOrId(model))
            {
                const QString msg("Model \"%1\" already stashed");
                return CStatusMessage(cats, CStatusMessage::SeverityError, msg.arg(model.getModelString()));
            }
            return CStatusMessage();
        }

        CStatusMessage CDbStashComponent::stashModel(const CAircraftModel &model)
        {
            CStatusMessage m(validateStashModel(model));
            if (!m.isWarningOrAbove())
            {
                this->ui->tvp_StashAircraftModels->insert(model);
            }
            return m;
        }

        CStatusMessageList CDbStashComponent::stashModels(const CAircraftModelList &models)
        {
            if (models.isEmpty()) { return CStatusMessageList(); }
            CStatusMessageList msgs;
            for (const CAircraftModel &model : models)
            {
                CStatusMessage m(stashModel(model));
                if (m.isWarningOrAbove()) { msgs.push_back(m); }
            }
            return msgs;
        }

        const Views::CAircraftModelView *CDbStashComponent::getView() const
        {
            return ui->tvp_StashAircraftModels;
        }

        void CDbStashComponent::ps_onUnstashPressed()
        {
            this->ui->tvp_StashAircraftModels->removeSelectedRows();
        }

        void CDbStashComponent::ps_onValidatePressed()
        {
            if (this->ui->tvp_StashAircraftModels->isEmpty()) { return; }
        }

        void CDbStashComponent::CStashModelsMenu::customMenu(QMenu &menu) const
        {
            menu.addAction(CIcons::database16(), "Unstash", this->parent(), SLOT(ps_onUnstashPressed()));
            nestedCustomMenu(menu);
        }

    } // ns
} // ns
