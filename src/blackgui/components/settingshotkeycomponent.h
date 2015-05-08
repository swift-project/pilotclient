/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSHOTKEYCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSHOTKEYCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/enableforruntime.h"
#include <QFrame>

namespace Ui { class CSettingsHotkeyComponent; }

namespace BlackGui
{
    namespace Components
    {

        //! Define hotkeys
        class BLACKGUI_EXPORT CSettingsHotkeyComponent :
            public QFrame,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSettingsHotkeyComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CSettingsHotkeyComponent();

            //! Reload settings
            void reloadSettings();

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            virtual void runtimeHasBeenSet() override;

        private slots:

            //! Settings have been changed
            void ps_changedSettings(uint typeValue);

            //! Save the Hotkeys
            void ps_saveHotkeys();

            //! Clear single hotkey
            void ps_clearHotkey();

        private:
            QScopedPointer<Ui::CSettingsHotkeyComponent> ui;
        };

    } // ns
} // ns

#endif // guard
