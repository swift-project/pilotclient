/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_MODELBROWSERCOMPONENT_H
#define BLACKGUI_COMPONENTS_MODELBROWSERCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CModelBrowserComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Allow to browse thru the model set
        class CModelBrowserComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CModelBrowserComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CModelBrowserComponent() override;

        private:
            QScopedPointer <Ui::CModelBrowserComponent> ui;
        };
    } // ns
} // ns

#endif // guard
