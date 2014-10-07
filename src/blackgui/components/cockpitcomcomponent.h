/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_COCKPITCOMCOMPONENT_H
#define BLACKGUI_COCKPITCOMCOMPONENT_H

#include "dockwidgetinfoareacomponent.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CCockpitMainComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! The main cockpit area
        class CCockpitComComponent :
            public QFrame,
            public CDockWidgetInfoAreaComponent
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CCockpitComComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CCockpitComComponent();

            //! \copydoc QWidget::paintEvent
            virtual void paintEvent(QPaintEvent *event) override;

        private:
            QScopedPointer<Ui::CCockpitMainComponent> ui;
        };

    } // namespace
} // namespace

#endif // guard
