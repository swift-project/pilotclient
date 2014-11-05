/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "cockpitcomponent.h"
#include "../showhidebar.h"
#include "../dockwidgetinfoarea.h"
#include "ui_cockpitcomponent.h"

namespace BlackGui
{
    namespace Components
    {

        CCockpitComponent::CCockpitComponent(QWidget *parent) :
            QWidget(parent),
            CEnableForDockWidgetInfoArea(),
            ui(new Ui::CCockpitComponent)
        {
            ui->setupUi(this);
            connect(ui->wip_CockpitComPanelShowHideBar, &BlackGui::CShowHideBar::toggleShowHide, this, &CCockpitComponent::ps_onToggleShowHideDetails);
        }

        CCockpitComponent::~CCockpitComponent()
        { }

        bool CCockpitComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            Q_ASSERT(parentDockableWidget);
            bool ok = CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
            if (ok && parentDockableWidget)
            {
                ok = connect(parentDockableWidget, &QDockWidget::topLevelChanged, this, &CCockpitComponent::ps_onToggleFloating);
            }
            return ok;
        }

        bool CCockpitComponent::isInfoAreaShown() const
        {
            return this->ui->wip_CockpitComPanelShowHideBar->isShown();
        }

        void CCockpitComponent::setSelectedTransponderModeStateIdent()
        {
            this->ui->comp_CockpitComComponent->setSelectedTransponderModeStateIdent();
        }

        void CCockpitComponent::ps_onToggleShowHideDetails(bool show)
        {
            Q_ASSERT(this->isParentDockWidgetFloating()); // show hide should not be visible if docked
            Q_ASSERT(this->window());
            if (!this->isParentDockWidgetFloating()) { return; }

            // keep old size
            QSize oldSize = this->window()->size();

            // hide area
            this->ui->comp_CockpitInfoArea->setVisible(show);

            // adjust size
            if (show)
            {
                if (!this->m_sizeFloatingShown.isValid())
                {
                    // manually setting size, all other approaches failed
                    QSize m(300, 400);
                    this->window()->resize(m);
                    this->m_sizeFloatingShown = this->window()->size();
                }
                else
                {
                    this->window()->resize(m_sizeFloatingShown);
                }

                if (this->m_sizeFloatingHidden.isValid())
                {
                    // was already initialized, override
                    this->m_sizeFloatingHidden = oldSize;
                }
            }
            else
            {
                if (!this->m_sizeFloatingHidden.isValid())
                {
                    // manually setting size, all other approaches failed
                    QSize m(300, 150);
                    this->window()->setMinimumSize(m);
                    this->window()->resize(m);
                    this->m_sizeFloatingHidden = this->window()->size();
                }
                else
                {
                    this->window()->setMinimumSize(m_sizeFloatingHidden);
                    this->window()->resize(m_sizeFloatingHidden);
                }

                if (this->m_sizeFloatingShown.isValid())
                {
                    // was already initialized, override
                    this->m_sizeFloatingShown = oldSize;
                }
            }
        }

        void CCockpitComponent::ps_onToggleFloating(bool floating)
        {
            this->ui->wip_CockpitComPanelShowHideBar->setVisible(floating);
            if (floating)
            {
                // use the toggle method to set the sizes
                this->ps_onToggleShowHideDetails(this->isInfoAreaShown());
            }
            else
            {
                this->ui->comp_CockpitInfoArea->setVisible(true);
            }
        }

    } // namespace
} // namespace
