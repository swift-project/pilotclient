/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_GUIUTILITY_H
#define BLACKGUI_GUIUTILITY_H

#include "blackgui/blackguiexport.h"
#include <QWidgetList>
#include "enableforframelesswindow.h"

namespace BlackGui
{
    //! GUI utilities
    class BLACKGUI_EXPORT CGuiUtility
    {

    public:

        //! Main application window
        static CEnableForFramelessWindow *mainApplicationWindow();

        //! Main application window widget
        static QWidget *mainApplicationWindowWidget();

        //! Is main window frameless?
        static bool isMainWindowFrameless();

        //! Top level widgets with names
        static QWidgetList topLevelApplicationWidgetsWithName();

        //! Position of main window
        static QPoint mainWindowPosition();

        //! Position of intro window
        static QPoint introWindowPosition();

        //! During initialization, when main window position might not be set set
        static QPoint assumedMainWindowPosition();

        //! Replace count in name such as "stations (4)"
        static QString replaceTabCountValue(const QString &oldName, int count);

        //! Delete hierarchy of layouts
        static void deleteLayout(QLayout *layout, bool deleteWidgets);

    private:
        //! Constructor, use static methods only
        CGuiUtility() {}
    };

}

#endif // guard
