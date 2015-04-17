/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKINPUT_KEYMAPPING_MAC_H
#define BLACKINPUT_KEYMAPPING_MAC_H

#include "blackmisc/hardware/keyboardkey.h"

namespace BlackInput
{
    //! \brief This class provides methods to map between Mac OS X virtual keys and CKeyboardKey
    class CKeyMappingMac
    {
    public:
        /*!
         * \brief Convert to Qt key
         * \param virtualKey
         * \return
         */
        static Qt::Key convertToKey(unsigned int virtualKey);
    };

} // namespace BlackInput

#endif // KEYMAPPING_MAC_H
