/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/overlaymessages.h"
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
    { }

    COverlayMessagesTabWidget::COverlayMessagesTabWidget(QWidget *parent) : COverlayMessagesBase(parent)
    { }

    COverlayMessagesWizardPage::COverlayMessagesWizardPage(QWidget *parent) : COverlayMessagesBase(parent)
    { }

    COverlayMessagesDockWidget::COverlayMessagesDockWidget(QWidget *parent) : COverlayMessagesBase(parent)
    { }

} // ns


