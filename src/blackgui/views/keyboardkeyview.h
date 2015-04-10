/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_KEYBOARDKEYVIEW_H
#define BLACKGUI_KEYBOARDKEYVIEW_H

//! \file

#include "blackgui/blackguiexport.h"
#include "viewbase.h"
#include "../models/keyboardkeylistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! Keyboard key view
        class BLACKGUI_EXPORT CKeyboardKeyView : public CViewBase<Models::CKeyboardKeyListModel, BlackMisc::Settings::CSettingKeyboardHotkeyList, BlackMisc::Settings::CSettingKeyboardHotkey>
        {

        public:

            //! Constructor
            explicit CKeyboardKeyView(QWidget *parent = nullptr);
        };
    }
}
#endif // guard
