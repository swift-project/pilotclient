// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/dropsite.h"

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFrame>
#include <QPalette>
#include <QStyle>
#include <Qt>
#include <QtGlobal>

#include "gui/guiapplication.h"
#include "gui/stylesheetutility.h"

using namespace swift::misc;

namespace swift::gui
{
    CDropSite::CDropSite(QWidget *parent) : QLabel(parent)
    {
        this->setFrameStyle(static_cast<int>(QFrame::Sunken) | QFrame::StyledPanel);
        this->setAlignment(Qt::AlignCenter);
        this->setAcceptDrops(true);
        this->setTextFormat(Qt::RichText);
        this->setInfoText("drop data here");
        this->onStyleSheetsChanged();
        connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CDropSite::onStyleSheetsChanged, Qt::QueuedConnection);
    }

    void CDropSite::setInfoText(const QString &dropSiteText)
    {
        m_infoText = dropSiteText;
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
        const QString html = "<img src=':/own/icons/own/drophere16.png'>&nbsp;&nbsp;" + m_infoText.toHtmlEscaped();
        setText(html);
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

    void CDropSite::onStyleSheetsChanged()
    {
        // style sheet changes go here
    }
} // namespace swift::gui
