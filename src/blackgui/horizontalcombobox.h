/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_HORIZONTALCOMBOBOX_H
#define BLACKGUI_HORIZONTALCOMBOBOX_H

#include <QComboBox>
#include <QListView>

namespace BlackGui
{
    //! A combobox where the dropdown is horizontally and not vertically
    class CHorizontalComboBox : public QComboBox
    {
        Q_OBJECT

    public:
        //! ComboBox displaying the values horizontally
        explicit CHorizontalComboBox(QWidget *parent = nullptr);

        //! \copydoc QComboBox::showPopup
        virtual void showPopup() override;

        //! Width of the popup
        void setPopupWidth(int w);

    private:
        QListView *m_view = nullptr;
        int m_popupWidth = -1;
    };
} // ns

#endif // guard
