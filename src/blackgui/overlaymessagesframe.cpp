// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/overlaymessages.h"
#include "blackgui/dockwidgetinfoarea.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/guiutility.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/verify.h"

#include <QKeyEvent>
#include <QPoint>
#include <QRect>
#include <QStyle>
#include <Qt>
#include <algorithm>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui
{
    COverlayMessagesFrame::COverlayMessagesFrame(QWidget *parent, Qt::WindowFlags f) : COverlayMessagesBase(parent, f)
    {}

    COverlayMessagesFrameEnableForDockWidgetInfoArea::COverlayMessagesFrameEnableForDockWidgetInfoArea(QWidget *parent, Qt::WindowFlags f) : COverlayMessagesFrame(parent, f)
    {}

    bool COverlayMessagesFrameEnableForDockWidgetInfoArea::isForwardingOverlayMessages() const
    {
        if (!this->hasDockWidgetArea()) { return false; }
        if (this->isParentDockWidgetFloating()) { return false; }
        return true;
    }

    COverlayMessagesTabWidget::COverlayMessagesTabWidget(QWidget *parent) : COverlayMessagesBase(parent)
    {}

    COverlayMessagesWizardPage::COverlayMessagesWizardPage(QWidget *parent) : COverlayMessagesBase(parent)
    {}

    COverlayMessagesDockWidget::COverlayMessagesDockWidget(QWidget *parent) : COverlayMessagesBase(parent)
    {}

    COverlayMessagesTableView::COverlayMessagesTableView(QWidget *parent) : COverlayMessagesBase(parent)
    {}

    COverlayMessagesTreeView::COverlayMessagesTreeView(QWidget *parent) : COverlayMessagesBase<QTreeView>(parent)
    {}

} // ns
