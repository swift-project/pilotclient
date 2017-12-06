/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKINPUT_JOYSTICKMACOS_H
#define BLACKINPUT_JOYSTICKMACOS_H

//! \file

#include "blackinput/joystick.h"
#include "blackmisc/input/joystickbutton.h"
#include "blackmisc/collection.h"

namespace BlackInput
{
    //! MacOS implemenation of IJoystick
    //! \todo Not implmeneted yet
    class CJoystickMacOS : public IJoystick
    {
        Q_OBJECT

    public:

        //! Copy Constructor
        CJoystickMacOS(CJoystickMacOS const &) = delete;

        //! Assignment operator
        CJoystickMacOS &operator=(CJoystickMacOS const &) = delete;

        //! \brief Destructor
        virtual ~CJoystickMacOS();

    private:
        friend class IJoystick;

        //! Destructor
        CJoystickMacOS(QObject *parent = nullptr);
    };

} // namespace BlackInput

#endif // BLACKINPUT_JOYSTICKMACOS_H
