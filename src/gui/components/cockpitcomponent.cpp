// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/cockpitcomponent.h"

#include <QDockWidget>
#include <QtGlobal>

#include "ui_cockpitcomponent.h"

#include "gui/components/cockpitcomcomponent.h"
#include "gui/components/cockpitinfoareacomponent.h"
#include "gui/dockwidgetinfoarea.h"
#include "gui/showhidebar.h"

namespace swift::gui::components
{
    CCockpitComponent::CCockpitComponent(QWidget *parent)
        : COverlayMessagesFrameEnableForDockWidgetInfoArea(parent), ui(new Ui::CCockpitComponent)
    {
        ui->setupUi(this);
        ui->wip_CockpitComPanelShowHideBar->setVisible(false);
        m_minHeightInfoArea = ui->comp_CockpitInfoArea->minimumHeight();

        connect(ui->wip_CockpitComPanelShowHideBar, &CShowHideBar::toggleShowHide, this,
                &CCockpitComponent::onToggleShowHideDetails, Qt::QueuedConnection);
        connect(ui->comp_CockpitComComponent, &CCockpitComComponent::requestCom1TextMessage, this,
                &CCockpitComponent::onRequestTextMessageCom1, Qt::QueuedConnection);
        connect(ui->comp_CockpitComComponent, &CCockpitComComponent::requestCom2TextMessage, this,
                &CCockpitComponent::onRequestTextMessageCom2, Qt::QueuedConnection);
    }

    CCockpitComponent::~CCockpitComponent() = default;

    bool CCockpitComponent::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
    {
        Q_ASSERT(parentDockableWidget);
        bool ok = CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(parentDockableWidget);
        if (ok && parentDockableWidget)
        {
            ok = connect(parentDockableWidget, &QDockWidget::topLevelChanged, this,
                         &CCockpitComponent::onToggleFloating, Qt::QueuedConnection);
        }
        return ok;
    }

    bool CCockpitComponent::isInfoAreaShown() const { return ui->wip_CockpitComPanelShowHideBar->isShown(); }

    void CCockpitComponent::setSelectedTransponderModeStateIdent()
    {
        ui->comp_CockpitComComponent->setTransponderModeStateIdent();
    }

    void CCockpitComponent::showAudio()
    {
        ui->comp_CockpitInfoArea->selectArea(CCockpitInfoAreaComponent::InfoAreaAudio);
    }

    void CCockpitComponent::onToggleShowHideDetails(bool show)
    {
        // use the toggle method to set the sizes
        this->toggleShowHideDetails(show, true);
    }

    void CCockpitComponent::toggleShowHideDetails(bool show, bool considerCurrentSize)
    {
        Q_ASSERT(this->isParentDockWidgetFloating()); // show hide should not be visible if docked
        Q_ASSERT(this->window());
        if (!this->isParentDockWidgetFloating()) { return; }

        // manually setting size, all other approaches failed
        static const QSize defaultSizeShown(500, 600);
        static const QSize defaultSizeHidden(300, 150);

        // keep old size
        const QSize manuallySetSize = this->window()->size();

        // hide area
        ui->comp_CockpitInfoArea->setVisible(show);

        // adjust size
        if (show)
        {
            ui->comp_CockpitInfoArea->setMinimumHeight(m_minHeightInfoArea);
            if (m_sizeFloatingShown.isValid())
            {
                this->window()->resize(m_sizeFloatingShown);
                if (considerCurrentSize) { m_sizeFloatingHidden = manuallySetSize; } // for next time
            }
            else
            {
                // manually setting size, all other approaches failed
                this->window()->resize(defaultSizeShown);
                m_sizeFloatingShown = this->window()->size();
            }
        }
        else
        {
            ui->comp_CockpitInfoArea->setMinimumHeight(0);
            this->window()->setMinimumSize(defaultSizeHidden);
            if (m_sizeFloatingHidden.isValid())
            {
                this->window()->resize(m_sizeFloatingHidden);
                if (considerCurrentSize) { m_sizeFloatingShown = manuallySetSize; }
            }
            else
            {
                // manually setting size, all other approaches failed
                this->window()->resize(defaultSizeHidden);
                m_sizeFloatingHidden = this->window()->size();
            }
        }
    }

    void CCockpitComponent::mouseDoubleClickEvent(QMouseEvent *event)
    {
        if (event) { emit requestTextMessageEntryTab(TextMessagesAll); }
        COverlayMessagesFrame::mouseDoubleClickEvent(event);
    }

    void CCockpitComponent::onRequestTextMessageCom1() { emit requestTextMessageEntryTab(TextMessagesCom1); }

    void CCockpitComponent::onRequestTextMessageCom2() { emit requestTextMessageEntryTab(TextMessagesCom2); }

    void CCockpitComponent::onATCStationsChanged()
    {
        // void
    }

    void CCockpitComponent::onToggleFloating(bool floating)
    {
        ui->wip_CockpitComPanelShowHideBar->setVisible(floating);
        if (floating)
        {
            // use the toggle method to set the sizes
            this->toggleShowHideDetails(this->isInfoAreaShown(), false);
        }
        else
        {
            const QSize sizeMinimum(200, 100); // set when docked, must fit into parent info area
            ui->comp_CockpitInfoArea->setVisible(true);
            this->window()->setMinimumSize(sizeMinimum);
        }
    }
} // namespace swift::gui::components
