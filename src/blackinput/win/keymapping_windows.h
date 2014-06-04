/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef BLACKINPUT_KEYMAPPING_WINDOWS_H
#define BLACKINPUT_KEYMAPPING_WINDOWS_H

#include "blackmisc/hwkeyboardkey.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace BlackInput
{
    //! \brief This class provides methods to map between windows virtual keys and CKeyboardKey
    class CKeyMappingWindows
    {
    public:
        /*!
         * \brief Converts a set of windows virtual keys to a CKeySet object
         * \param virtualKey
         * \return
         */
        static BlackMisc::Hardware::CKeyboardKey::Modifier convertToModifier(WPARAM virtualKey);

        /*!
         * \brief Convert to Qt key
         * \param virtualKey
         * \return
         */
        static Qt::Key convertToKey(WPARAM virtualKey);

        /*!
         * \brief Checks if its a modifier key
         * \param vkcode
         * \return
         */
        static bool isModifier(WPARAM vkcode);
    };

} // namespace BlackInput
#endif // guard
