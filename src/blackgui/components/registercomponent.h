/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_COMPONENTS_REGISTERCOMPONENT_H
#define BLACKGUI_COMPONENTS_REGISTERCOMPONENT_H

#include "blackgui/components/updatetimer.h"
#include <QFrame>

namespace Ui { class CRegisterComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! Register components in the GUI
        class BLACKGUI_EXPORT CRegisterComponent :
            public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CRegisterComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CRegisterComponent();

        private slots:
            //! Update data
            void ps_update();

        private:
            QScopedPointer<Ui::CRegisterComponent> ui;
            QScopedPointer<CUpdateTimer> m_updateTimer;
        };

    } // ns
} // ns

#endif // guard
