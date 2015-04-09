/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGSFSXCOMPONENT_H
#define BLACKGUI_SETTINGSFSXCOMPONENT_H

#include "blackgui/components/enableforruntime.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CSettingsFsxComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! Settings for FSX
        class CSettingsFsxComponent : public QFrame, public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSettingsFsxComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CSettingsFsxComponent();

        private slots:
            //! Test the SIM connect connectivity
            void testSimConnectConnection();

            //! Save a simconnect.cfg file for FSX
            void saveSimConnectCfg();

            //! simConnect.cfg: open, exists? delete
            void simConnectCfgFile();

        private:
            QScopedPointer<Ui::CSettingsFsxComponent> ui;
        };
    }
}

#endif // guard
