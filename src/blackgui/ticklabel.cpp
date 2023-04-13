/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/ticklabel.h"
#include "blackmisc/icons.h"

using namespace BlackMisc;

namespace BlackGui
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

    void BlackGui::CTickLabel::setToolTips(const QString &ticked, const QString &unticked)
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
