// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/ticklabel.h"
#include "misc/icons.h"

using namespace swift::misc;

namespace swift::gui
{
    CTickLabel::CTickLabel(QWidget *parent) : QLabel(parent)
    {
        this->setText("");
        this->setMargin(0);
        this->setMaximumSize(CIcons::tick16().size());
        this->setLabel();
    }

    void CTickLabel::setTicked(bool ticked)
    {
        if (m_isPixmapTicked == ticked) { return; }
        this->m_isPixmapTicked = ticked;
        this->setLabel();

        emit tickChanged(this->m_isPixmapTicked);
    }

    void swift::gui::CTickLabel::setToolTips(const QString &ticked, const QString &unticked)
    {
        m_toolTipTicked = ticked;
        m_toolTipUnticked = unticked;
        this->setLabel();
    }

    void CTickLabel::setPixmapTicked(const QPixmap &pixmap)
    {
        m_pixmapTicked = pixmap;
        this->setLabel();
    }

    void CTickLabel::setPixmapUnticked(const QPixmap &pixmap)
    {
        m_pixmapUnticked = pixmap;
        this->setLabel();
    }

    void CTickLabel::setLabel()
    {
        this->setText("");
        this->setPixmap(this->m_isPixmapTicked ? m_pixmapTicked : m_pixmapUnticked);
        this->setToolTip(this->m_isPixmapTicked ? m_toolTipTicked : m_toolTipUnticked);
    }
} // namespace
