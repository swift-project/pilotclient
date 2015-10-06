/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMCONNECT_SETTINGS_COMPONENT_H
#define BLACKSIMPLUGIN_SIMCONNECT_SETTINGS_COMPONENT_H

#include <QFrame>
#include <QScopedPointer>
#include "blackgui/components/enableforruntime.h"

namespace Ui {
class CSimConnectSettingsComponent;
}

namespace BlackSimPlugin
{
    namespace Fsx
    {
        /**
         * A component that gathers all SimConnect-related settings.
         */
        class CSimConnectSettingsComponent : public QFrame, public BlackGui::Components::CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Ctor
            explicit CSimConnectSettingsComponent(QWidget *parent = nullptr);

            //! Dtor
            ~CSimConnectSettingsComponent();

        private slots:
            //! Open simConnect.cfg using default application
            void openSimConnectCfgFile();

            //! Delete simConnect.cfg file
            void deleteSimConnectCfgFile();

            //! Check whether the simConnect.cfg file exists
            void checkSimConnectCfgFile();

            //! Test the SimConnect connectivity
            void testSimConnectConnection();

            //! Save a simconnect.cfg file for FSX
            void saveSimConnectCfgFile();

        private:
            QScopedPointer<Ui::CSimConnectSettingsComponent> ui;

        };

    }
}

#endif // guard
