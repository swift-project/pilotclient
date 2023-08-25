// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
