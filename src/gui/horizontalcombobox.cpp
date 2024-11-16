// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "horizontalcombobox.h"

namespace swift::gui
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

    void CHorizontalComboBox::setPopupWidth(int w) { m_popupWidth = w; }
} // namespace swift::gui
