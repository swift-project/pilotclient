/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_LOGCOMPONENT_H
#define BLACKGUI_LOGCOMPONENT_H

#include "runtimebasedcomponent.h"
#include <QFrame>

namespace Ui { class CLogComponent; }

namespace BlackGui
{
    namespace Components
    {

        //! GUI displaying log and status messages
        class CLogComponent : public QFrame, public CRuntimeBasedComponent
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CLogComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CLogComponent();

        private:
            Ui::CLogComponent *ui;
        };
    }
}
#endif // guard
