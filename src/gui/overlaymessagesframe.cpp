// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/overlaymessagesframe.h"

#include <algorithm>

#include <QKeyEvent>
#include <QPoint>
#include <QRect>
#include <QStyle>
#include <Qt>

#include "gui/dockwidgetinfoarea.h"
#include "gui/guiutility.h"
#include "gui/overlaymessages.h"
#include "gui/stylesheetutility.h"
#include "misc/network/textmessage.h"
#include "misc/verify.h"

using namespace swift::misc;
using namespace swift::misc::network;

namespace swift::gui
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

} // namespace swift::gui
