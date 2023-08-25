// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/dockwidgetinfoarea.h"
#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/infoarea.h"

#include <QMenu>
#include <QString>
#include <QWidget>
#include <QtGlobal>

using namespace BlackGui::Components;

namespace BlackGui
{
    CDockWidgetInfoArea::CDockWidgetInfoArea(QWidget *parent) : CDockWidget(parent)
    {
        // void
    }

    const CInfoArea *CDockWidgetInfoArea::getParentInfoArea() const
    {
        const CInfoArea *ia = qobject_cast<const CInfoArea *>(this->parent());
        Q_ASSERT(ia);
        return ia;
    }

    CInfoArea *CDockWidgetInfoArea::getParentInfoArea()
    {
        CInfoArea *ia = qobject_cast<CInfoArea *>(this->parent());
        Q_ASSERT(ia);
        return ia;
    }

    bool CDockWidgetInfoArea::isSelectedDockWidget() const
    {
        const CInfoArea *ia = getParentInfoArea();
        if (!ia) { return false; }
        return ia->isSelectedDockWidgetInfoArea(this);
    }

    bool CDockWidgetInfoArea::isVisibleWidget() const
    {
        // if the widget is invisible we are done
        // but if it is visible, there is no guarantee it can be seen by the user
        if (!this->isVisible()) { return false; }

        // further checks
        if (this->isFloating())
        {
            if (this->isMinimized()) { return false; }
            return true;
        }
        else
        {
            return isSelectedDockWidget();
        }
    }

    void CDockWidgetInfoArea::addToContextMenu(QMenu *contextMenu) const
    {
        QList<const CInfoArea *> parentInfoAreas = this->findParentInfoAreas();
        Q_ASSERT(!parentInfoAreas.isEmpty());
        if (parentInfoAreas.isEmpty()) return;

        // Dockable widget's context menu
        CDockWidget::addToContextMenu(contextMenu);
        if (!contextMenu->isEmpty()) { contextMenu->addSeparator(); }

        // first info area, myself's direct parent info area
        parentInfoAreas.first()->addToContextMenu(contextMenu);

        // top info areas other than direct parent
        // (parent's parent when nested info areas are used)
        if (parentInfoAreas.size() < 2) { return; }
        contextMenu->addSeparator();
        for (int i = 1; i < parentInfoAreas.size(); i++)
        {
            const CInfoArea *infoArea = parentInfoAreas.at(i);
            QString title(infoArea->windowTitle());
            if (title.isEmpty())
            {
                title = infoArea->objectName();
            }
            QMenu *m = contextMenu->addMenu(title);
            infoArea->addToContextMenu(m);
        }
    }

    void CDockWidgetInfoArea::initialFloating()
    {
        CDockWidget::initialFloating(); // initial floating to init position & size

        // set the top level dock widget area to all children
        QList<CEnableForDockWidgetInfoArea *> infoAreaDockWidgets = this->findEmbeddedDockWidgetInfoAreaComponents();
        for (CEnableForDockWidgetInfoArea *dwia : infoAreaDockWidgets)
        {
            Q_ASSERT_X(dwia, Q_FUNC_INFO, "Missing info area");
            dwia->setParentDockWidgetInfoArea(this);
        }
    }

    QList<CEnableForDockWidgetInfoArea *> CDockWidgetInfoArea::findEmbeddedDockWidgetInfoAreaComponents()
    {
        QList<QWidget *> widgets = this->findChildren<QWidget *>(); // must not use Qt::FindDirectChildrenOnly here
        QList<CEnableForDockWidgetInfoArea *> widgetsWithDockWidgetInfoAreaComponent;
        for (QWidget *w : widgets)
        {
            Q_ASSERT(w);

            // CEnableForDockWidgetInfoArea is no QObject, so we use dynamic_cast
            CEnableForDockWidgetInfoArea *dwc = dynamic_cast<CEnableForDockWidgetInfoArea *>(w);
            if (dwc)
            {
                widgetsWithDockWidgetInfoAreaComponent.append(dwc);
            }
        }
        QList<CDockWidgetInfoArea *> nestedInfoAreas = this->findNestedInfoAreas();
        if (nestedInfoAreas.isEmpty()) return widgetsWithDockWidgetInfoAreaComponent;

        // we have to exclude the nested embedded areas
        for (CDockWidgetInfoArea *ia : nestedInfoAreas)
        {
            QList<CEnableForDockWidgetInfoArea *> nestedInfoAreaComponents = ia->findEmbeddedDockWidgetInfoAreaComponents();
            if (nestedInfoAreaComponents.isEmpty()) { continue; }
            for (CEnableForDockWidgetInfoArea *iac : nestedInfoAreaComponents)
            {
                const bool r = widgetsWithDockWidgetInfoAreaComponent.removeOne(iac);
                Q_ASSERT(r); // why is the nested component not in the child list?
                Q_UNUSED(r)
            }
        }
        return widgetsWithDockWidgetInfoAreaComponent;
    }

    QList<CDockWidgetInfoArea *> CDockWidgetInfoArea::findNestedInfoAreas()
    {
        // must not use Qt::FindDirectChildrenOnly here
        QList<CDockWidgetInfoArea *> nestedInfoAreas = this->findChildren<CDockWidgetInfoArea *>();
        return nestedInfoAreas;
    }

    QList<const CInfoArea *> CDockWidgetInfoArea::findParentInfoAreas() const
    {
        QList<const CInfoArea *> parents;
        QWidget *currentWidget = this->parentWidget();
        while (currentWidget)
        {
            const CInfoArea *ia = qobject_cast<CInfoArea *>(currentWidget);
            if (ia) { parents.append(ia); }
            currentWidget = currentWidget->parentWidget();
        }
        return parents;
    }
} // namespace
