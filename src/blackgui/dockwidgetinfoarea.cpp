/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dockwidgetinfoarea.h"
#include "infoarea.h"
#include "components/maininfoareacomponent.h"
#include "blackmisc/icons.h"

using namespace BlackGui::Components;

namespace BlackGui
{
    CDockWidgetInfoArea::CDockWidgetInfoArea(QWidget *parent) : CDockWidget(parent)
    {
        // void
    }

    const CInfoArea *CDockWidgetInfoArea::getParentInfoArea() const
    {
        const CInfoArea *ia = dynamic_cast<const CInfoArea *>(this->parent());
        Q_ASSERT(ia);
        return ia;
    }

    CInfoArea *CDockWidgetInfoArea::getParentInfoArea()
    {
        CInfoArea *ia = dynamic_cast<CInfoArea *>(this->parent());
        Q_ASSERT(ia);
        return ia;
    }

    bool CDockWidgetInfoArea::isSelectedDockWidget() const
    {
        const CInfoArea *ia = getParentInfoArea();
        if (!ia) return false;
        return ia->isSelectedInfoArea(this);
    }

    bool CDockWidgetInfoArea::isVisibleWidget() const
    {
        // if the widget is invisible we are done
        // but if it is visible, there is no guarantee it can be seen by the user
        if (!this->isVisible()) return false;

        // further checks
        if (this->isFloating())
        {
            if (this->isMinimized()) return false;
            return true;
        }
        else
        {
            return isSelectedDockWidget();
        }
    }

    void CDockWidgetInfoArea::addToContextMenu(QMenu *contextMenu) const
    {
        Components::CMainInfoAreaComponent *mainWidget = qobject_cast<CMainInfoAreaComponent *>(parentWidget());
        Q_ASSERT(mainWidget);
        if (!mainWidget) return;

        // Dockable widget's context menu
        CDockWidget::addToContextMenu(contextMenu);

        // from main component (info area)
        contextMenu->addSeparator();
        mainWidget->addToContextMenu(contextMenu);
    }

    void CDockWidgetInfoArea::initalFloating()
    {
        CDockWidget::initalFloating(); // initial floating to init position & size
        QList<CDockWidgetInfoAreaComponent *> infoAreaDockWidgets = this->findEmbeddedDockWidgetInfoAreaComponents();
        foreach(CDockWidgetInfoAreaComponent * dwia, infoAreaDockWidgets)
        {
            // KWB: potentially a risk when this object is deleted
            //      put under normal situations the child object will be deleted as well, and we have
            //      no multi-threaded GUI
            dwia->setParentDockWidgetInfoArea(this);
        }
    }

    QList<CDockWidgetInfoAreaComponent *> CDockWidgetInfoArea::findEmbeddedDockWidgetInfoAreaComponents()
    {
        QList<QWidget *> widgets = this->findChildren<QWidget *>();
        QList<CDockWidgetInfoAreaComponent *> widgetsWithDockWidgetInfoAreaComponent;
        foreach(QWidget * w, widgets)
        {
            CDockWidgetInfoAreaComponent *dwc = dynamic_cast<Components::CDockWidgetInfoAreaComponent *>(w);
            if (dwc)
            {
                widgetsWithDockWidgetInfoAreaComponent.append(dwc);
            }
        }
        QList<CDockWidgetInfoArea *> nestedInfoAreas = this->findNestedInfoAreas();
        if (nestedInfoAreas.isEmpty()) return widgetsWithDockWidgetInfoAreaComponent;

        // we have to exclude the nested embedded areas
        foreach(CDockWidgetInfoArea * ia, nestedInfoAreas)
        {
            QList<CDockWidgetInfoAreaComponent *> nestedInfoAreaComponents = ia->findEmbeddedDockWidgetInfoAreaComponents();
            if (nestedInfoAreaComponents.isEmpty()) continue;
            foreach(CDockWidgetInfoAreaComponent * iac, nestedInfoAreaComponents)
            {
                bool r = widgetsWithDockWidgetInfoAreaComponent.removeOne(iac);
                Q_ASSERT(r); // why is the nested component not in the child list?
                Q_UNUSED(r);
            }
        }
        return widgetsWithDockWidgetInfoAreaComponent;
    }

    QList<CDockWidgetInfoArea *> CDockWidgetInfoArea::findNestedInfoAreas()
    {
        QList<CDockWidgetInfoArea *> nestedInfoAreas = this->findChildren<CDockWidgetInfoArea *>();
        return nestedInfoAreas;
    }
}
