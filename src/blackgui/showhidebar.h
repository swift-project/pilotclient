/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
