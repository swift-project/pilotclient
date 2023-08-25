// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_SHOWHIDEBAR_H
#define BLACKGUI_SHOWHIDEBAR_H

#include "blackgui/blackguiexport.h"
#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CShowHideBar;
}
namespace BlackGui
{
    //! Show or hide (expand / collapse) another section
    class BLACKGUI_EXPORT CShowHideBar : public QFrame
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

} // namespace
#endif // guard
