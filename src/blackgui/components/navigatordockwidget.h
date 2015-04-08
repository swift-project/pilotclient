/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_NAVIGATORDOCKWIDGET_H
#define BLACKGUI_NAVIGATORDOCKWIDGET_H

#include "blackgui/dockwidgetinfoarea.h"
#include <QDockWidget>
#include <QScopedPointer>

namespace Ui { class CNavigatorDockWidget; }

namespace BlackGui
{
    namespace Components
    {

        //! Dock widget for navigators
        class CNavigatorDockWidget : public BlackGui::CDockWidgetInfoArea
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CNavigatorDockWidget(QWidget *parent = nullptr);

            //! Destructor
            ~CNavigatorDockWidget();

        private:
            QScopedPointer<Ui::CNavigatorDockWidget> ui;
        };

    } // ns
} // ns

#endif // guard
