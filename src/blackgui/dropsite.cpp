/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dropsite.h"
#include "stylesheetutility.h"
#include <QDragEnterEvent>

using namespace BlackMisc;

namespace BlackGui
{
    CDropSite::CDropSite(QWidget *parent) : QLabel(parent)
    {
        setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
        setAlignment(Qt::AlignCenter);
        setAcceptDrops(true);
        this->setInfoText(tr("<drop content>"));
        connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CDropSite::ps_onStyleSheetsChanged);
        this->ps_onStyleSheetsChanged();
    }

    void CDropSite::setInfoText(const QString &dropSiteText)
    {
        this->m_infoText = dropSiteText;
        this->resetText();
    }

    void CDropSite::allowDrop(bool allowed)
    {
        CDropBase::allowDrop(allowed);
        this->setEnabled(allowed);
        this->setVisible(allowed);
    }

    void CDropSite::resetText()
    {
        setText(this->m_infoText);
    }

    void CDropSite::dragEnterEvent(QDragEnterEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        setBackgroundRole(QPalette::Highlight);
        event->acceptProposedAction();
    }

    void CDropSite::dragMoveEvent(QDragMoveEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        setBackgroundRole(QPalette::Highlight);
        event->acceptProposedAction();
    }

    void CDropSite::dragLeaveEvent(QDragLeaveEvent *event)
    {
        if (!event || !m_allowDrop) { return; }
        resetText();
        event->accept();
    }

    void CDropSite::dropEvent(QDropEvent *event)
    {
        if (!event || !acceptDrop(event->mimeData())) { return; }
        CVariant valueVariant(toCVariant(event->mimeData()));
        if (valueVariant.isValid())
        {
            emit droppedValueObject(valueVariant);
        }
        this->resetText();
    }

    void CDropSite::ps_onStyleSheetsChanged()
    {
        // style sheet changes go here
    }

    void CDropSite::paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        CStyleSheetUtility::useStyleSheetInDerivedWidget(this, QStyle::PE_Widget);
    }

} // ns
