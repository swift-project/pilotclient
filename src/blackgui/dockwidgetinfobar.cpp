/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/dockwidgetinfobar.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/icons.h"

#include <QIcon>
#include <QString>

using namespace BlackMisc;
using namespace BlackGui::Components;

namespace BlackGui
{
    CDockWidgetInfoBar::CDockWidgetInfoBar(QWidget *parent) : CDockWidget(parent)
    {
        this->setWindowTitle("Info status bar");
        this->setWindowIcon(CIcons::swift24());
        this->onStyleSheetsChanged(); // margins from infobar
    }

    void CDockWidgetInfoBar::addToContextMenu(QMenu *contextMenu) const
    {
        // Dockable widget's context menu
        CDockWidget::addToContextMenu(contextMenu);
    }

    void CDockWidgetInfoBar::onStyleSheetsChanged()
    {
        const QString st = sGui->getStyleSheetUtility().style(CStyleSheetUtility::fileNameInfoBar());
        this->setStyleSheet(st);
    }
}
