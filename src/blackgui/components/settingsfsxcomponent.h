/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_SETTINGSFSXCOMPONENT_H
#define BLACKGUI_SETTINGSFSXCOMPONENT_H

//! \file

#include "blackgui/components/runtimebasedcomponent.h"
#include <QFrame>

namespace Ui { class CSettingsFsxComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! Settings for FSX
        class CSettingsFsxComponent : public QFrame, public CRuntimeBasedComponent
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
            Ui::CSettingsFsxComponent *ui;
        };
    }
}

#endif // guard
