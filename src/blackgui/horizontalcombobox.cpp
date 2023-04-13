/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "horizontalcombobox.h"

namespace BlackGui
{
    CHorizontalComboBox::CHorizontalComboBox(QWidget *parent) : QComboBox(parent)
    {
        m_view = new QListView(this);
        m_view->setFlow(QListView::LeftToRight);
        this->setView(m_view);
        for (QWidget *o : findChildren<QWidget *>())
        {
            if (o->inherits("QComboBoxPrivateContainer"))
            {
                // popup
                o->setFixedHeight(view()->height());
                break;
            }
        }
    }

    void CHorizontalComboBox::showPopup()
    {
        QComboBox::showPopup();
        int w = m_popupWidth;
        if (w < 0)
        {
            w = 0;
            for (int i = 0; i < count(); i++)
            {
                const QModelIndex ix = model()->index(i, modelColumn(), rootModelIndex());
                w += view()->visualRect(ix).width();
            }
        }
        view()->setFixedWidth(w);
    }

    void CHorizontalComboBox::setPopupWidth(int w)
    {
        m_popupWidth = w;
    }
} // ns
