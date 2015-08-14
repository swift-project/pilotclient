/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKINPUT_JOYSTICKMAC_H
#define BLACKINPUT_JOYSTICKMAC_H

//! \file

#include "blackinput/joystick.h"
#include "blackmisc/input/joystickbutton.h"
#include "blackmisc/collection.h"

namespace BlackInput
{
    //! OSX implemenation of IJoystick
    //! \todo Not implmeneted yet
    class CJoystickMac : public IJoystick
    {
        Q_OBJECT

    public:

        //! Copy Constructor
        CJoystickMac(CJoystickMac const &) = delete;

        //! Assignment operator
        CJoystickMac &operator=(CJoystickMac const &) = delete;

        //! \brief Destructor
        virtual ~CJoystickMac();

    private:
        friend class IJoystick;

        //! Destructor
        CJoystickMac(QObject *parent = nullptr);
    };

} // namespace BlackInput

#endif // BLACKINPUT_JOYSTICKMAC_H
