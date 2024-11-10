// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/dockwidgetinfoarea.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/guiutility.h"
#include "blackgui/infoarea.h"

#include <QDockWidget>
#include <QMetaObject>
#include <QObject>
#include <QtGlobal>
#include <QTimer>
#include <QPointer>

using namespace swift::misc::aviation;
using namespace BlackGui;

namespace BlackGui
{
    CEnableForDockWidgetInfoArea::CEnableForDockWidgetInfoArea(CDockWidgetInfoArea *parentInfoArea)
    {
        // it the parent is already an info area at this time, we keep it
        // otherwise we expect the info area to set it later
        m_parentDockableInfoArea = parentInfoArea;
    }

    bool CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
    {
        // sanity check
        if (m_parentDockableInfoArea)
        {
            // we already have a value
            // changes should not happen
            Q_ASSERT_X(m_parentDockableInfoArea == parentDockableWidget, Q_FUNC_INFO, "Reassigned parent dock widget area");
            return m_parentDockableInfoArea == parentDockableWidget;
        }

        m_parentDockableInfoArea = parentDockableWidget;
        QMetaObject::Connection con = QDockWidget::connect(parentDockableWidget, &QDockWidget::destroyed, [this] {
            // break dependency to dockable widget
            m_parentDockableInfoArea = nullptr;
        });
        Q_ASSERT_X(con, Q_FUNC_INFO, "Connection failed");
        m_connections.append(con);
        return true;
    }

    CInfoArea *CEnableForDockWidgetInfoArea::getParentInfoArea() const
    {
        Q_ASSERT(m_parentDockableInfoArea);
        if (!m_parentDockableInfoArea) { return nullptr; }
        return m_parentDockableInfoArea->getParentInfoArea();
    }

    bool CEnableForDockWidgetInfoArea::isParentDockWidgetFloating() const
    {
        if (!m_parentDockableInfoArea) { return false; } // not floating if not yet initialized
        return m_parentDockableInfoArea->isFloating();
    }

    bool CEnableForDockWidgetInfoArea::isVisibleWidget() const
    {
        if (!m_parentDockableInfoArea) { return false; } // can happen function is used while dock widget not yet fully initialized
        return m_parentDockableInfoArea->isVisibleWidget();
    }

    CEnableForFramelessWindow *CEnableForDockWidgetInfoArea::mainApplicationWindow() const
    {
        CEnableForFramelessWindow *mw = CGuiUtility::mainFramelessEnabledWindow();
        return mw;
    }

    QWidget *CEnableForDockWidgetInfoArea::mainApplicationWindowWidget() const
    {
        CEnableForFramelessWindow *mw = this->mainApplicationWindow();
        return mw ? mw->getWidget() : nullptr;
    }

    void CEnableForDockWidgetInfoArea::displayMyself()
    {
        // if we are already visible, we are done
        if (this->isVisibleWidget()) { return; }

        // select myself
        if (this->getParentInfoArea())
        {
            this->getParentInfoArea()->selectArea(this->getDockWidgetInfoArea());
        }
    }
} // namespace
