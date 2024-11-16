// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SHOWHIDEBAR_H
#define SWIFT_GUI_SHOWHIDEBAR_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CShowHideBar;
}
namespace swift::gui
{
    //! Show or hide (expand / collapse) another section
    class SWIFT_GUI_EXPORT CShowHideBar : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CShowHideBar(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CShowHideBar();

        //! Is shown?
        bool isShown() const;

    signals:
        //! Show or hide cockpit details
        void toggleShowHide(bool show);

    private:
        QScopedPointer<Ui::CShowHideBar> ui;
    };

} // namespace swift::gui
#endif // guard
