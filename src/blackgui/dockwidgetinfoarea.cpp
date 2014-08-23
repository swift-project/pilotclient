/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dockwidgetinfoarea.h"
#include "components/maininfoareacomponent.h"
#include "blackmisc/icons.h"

using namespace BlackGui::Components;

namespace BlackGui
{
    CDockWidgetInfoArea::CDockWidgetInfoArea(QWidget *parent) : CDockWidget(parent) { }

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
}
